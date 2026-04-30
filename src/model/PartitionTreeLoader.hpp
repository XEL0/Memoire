#ifndef MEMOIRE_PARTITIONTREELOADER_HPP
#define MEMOIRE_PARTITIONTREELOADER_HPP

#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <memory>
#include <map>
#include <optional>

#include "Graph.hpp"

class DrawableComparabilityBigraph;
using json = nlohmann::json;


struct PartitionTreeNode {
    std::shared_ptr<ComparabilityBigraph> graph;
    std::vector<std::shared_ptr<PartitionTreeNode>> children;
    unsigned depth;
    double H;
    std::optional<bool> U;
    std::optional<bool> FL;
    std::optional<bool> L;
    std::string label;
    //todo truncable boolean
};

struct GlobalKnowledge {
    std::unordered_map<unsigned, std::vector<unsigned>> embedding;
    std::unordered_map<unsigned, unsigned> coloring;
};

class PartitionTreeLoader {
public:
    PartitionTreeLoader() = default;

    static std::shared_ptr<PartitionTreeNode> loadPartitionTreeFromJson(
        const std::string &filePath,
        const std::shared_ptr<DrawableComparabilityBigraph> &rootGraph = nullptr) {

        std::ifstream file(filePath);
        if (not file.is_open()) throw std::runtime_error("Cannot open file: " + filePath);

        json jsonData;
        file >> jsonData;
        file.close();
        auto rootNode = std::make_shared<PartitionTreeNode>();
        auto globalKnowledge = std::make_shared<GlobalKnowledge>();
        rootNode->depth = 0;
        parseJsonNode(jsonData, rootNode, globalKnowledge, rootGraph);

        return rootNode;
    }

    static void parseJsonNode(
        const json &jsonNode,
        std::shared_ptr<PartitionTreeNode> &node,
        const std::shared_ptr<GlobalKnowledge> &globalKnowledge,
        const std::shared_ptr<DrawableComparabilityBigraph> &rootGraph = nullptr,
        int depth = 0) {
        node->depth = depth;
        std::vector<VertexPointer> vertices;
        unsigned dim = 0;

        if (depth == 0) {
            if (jsonNode.contains("embedding")) {
                auto embedding = jsonNode["embedding"].get<json::object_t>();
                for (const auto &[vertexStr, coords] : embedding) {
                    unsigned vertex = std::stoi(vertexStr);
                    unsigned x = coords[0].get<unsigned>();
                    unsigned y = coords[1].get<unsigned>();
                    globalKnowledge->embedding[vertex] = {x, y};
                }
            }

            if (jsonNode.contains("coloring")) {
                auto coloring = jsonNode["coloring"].get<json::object_t>();
                for (const auto &[vertexStr, color] : coloring) {
                    unsigned vertex = std::stoi(vertexStr);
                    unsigned colorValue = color.get<unsigned>();
                    globalKnowledge->coloring[vertex] = colorValue;
                }
            }
        }

        unsigned p = 0, q = 0;
        if (jsonNode.contains("V") and jsonNode["V"].is_array()) {
            for (const auto &v : jsonNode["V"]) {
                auto color = globalKnowledge->coloring.at(v.get<unsigned>());
                color == 0 ? p++ : q++;
                vertices.push_back(std::make_shared<ColoredEmbeddedVertex>(
                    v.get<unsigned>(),
                    color,
                    globalKnowledge->embedding.at(v.get<unsigned>())
                    ));
            }
        }
        if (jsonNode.contains("H")) node->H = jsonNode["H"].get<double>();
        if (jsonNode.contains("dim")) dim = jsonNode["dim"].get<unsigned>();

        node->U = jsonNode.contains("U") ? jsonNode["U"].get<bool>() : node->U = std::nullopt;
        node->FL = jsonNode.contains("FL") ? jsonNode["FL"].get<bool>() : node->FL = std::nullopt;
        node->L = jsonNode.contains("L") ? jsonNode["L"].get<bool>() : node->L = std::nullopt;

        node->label = jsonNode.value("label", "unknown");
        node->graph = std::make_shared<ComparabilityBigraph>(vertices, p, q, dim, 1000);
        node->graph->constructE(true);

        if (jsonNode.contains("children") and jsonNode["children"].is_array()) {
            const auto &childrenArray = jsonNode["children"];

            for (size_t i = 0; i < childrenArray.size(); ++i) {
                auto childNode = std::make_shared<PartitionTreeNode>();
                parseJsonNode(childrenArray[i], childNode, globalKnowledge, rootGraph, depth + 1);
                node->children.push_back(childNode);
            }
        }
    }

    static void printPartitionTree(const std::shared_ptr<PartitionTreeNode> &node, int indent = 0) {
        std::string indentStr(indent * 2, ' ');

        std::cout << indentStr << "Node: " << node->label << "\n";
        std::cout << indentStr << "  Vertices: ";
        for (auto v : node->graph->enumerate()) {
            std::cout << v->getId() << " ";
        }
        std::cout << "\n";
        std::cout << indentStr << "  Dim: " << node->graph->getDimension() << ", H: " << node->H << "\n";

        auto printOpt = [](std::optional<bool> opt) -> std::string {
            if (!opt.has_value()) return "null";
            return opt.value() ? "true" : "false";
        };

        std::cout << indentStr << "  Flags: U=" << printOpt(node->U)
                  << " FL=" << printOpt(node->FL)
                  << " L=" << printOpt(node->L) << "\n";

        if (!node->children.empty()) {
            std::cout << indentStr << "  Children: " << node->children.size() << "\n";
            for (const auto &child : node->children) {
                printPartitionTree(child, indent + 1);
            }
        }
    }
};

#endif
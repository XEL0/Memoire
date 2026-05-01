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
        const std::shared_ptr<DrawableComparabilityBigraph> &root_graph = nullptr) {

        std::ifstream file(filePath);
        if (not file.is_open()) throw std::runtime_error("Cannot open file: " + filePath);

        json json_data;
        file >> json_data;
        file.close();
        auto root_node = std::make_shared<PartitionTreeNode>();
        auto global_knowledge = std::make_shared<GlobalKnowledge>();
        root_node->depth = 0;
        parseJsonNode(json_data, root_node, global_knowledge, root_graph);

        return root_node;
    }

    static void parseJsonNode(
        const json &json_node,
        const std::shared_ptr<PartitionTreeNode> &node,
        const std::shared_ptr<GlobalKnowledge> &global_knowledge,
        const std::shared_ptr<DrawableComparabilityBigraph> &root_graph = nullptr,
        const int depth = 0) {

        node->depth = depth;
        std::vector<VertexPointer> vertices;
        unsigned dim = 0;
        if (json_node.contains("dim")) dim = json_node["dim"].get<unsigned>();

        if (depth == 0) {
            if (json_node.contains("embedding")) {
                for (auto embedding = json_node["embedding"].get<json::object_t>();
                    const auto &[vertexStr, coords] : embedding) {
                    unsigned vertex = std::stoi(vertexStr);
                    global_knowledge->embedding[vertex] = std::vector<unsigned>();
                    for (const auto& coord : coords) {
                        global_knowledge->embedding[vertex].push_back(coord.get<unsigned>());
                    }
                }
            }

            if (json_node.contains("coloring")) {
                auto coloring = json_node["coloring"].get<json::object_t>();
                for (const auto &[vertexStr, color] : coloring) {
                    unsigned vertex = std::stoi(vertexStr);
                    const unsigned colorValue = color.get<unsigned>();
                    global_knowledge->coloring[vertex] = colorValue;
                }
            }
        }

        unsigned p = 0, q = 0;
        if (json_node.contains("V") and json_node["V"].is_array()) {
            for (const auto &v : json_node["V"]) {
                auto color = global_knowledge->coloring.at(v.get<unsigned>());
                color == 0 ? p++ : q++;
                vertices.push_back(std::make_shared<ColoredEmbeddedVertex>(
                    v.get<unsigned>(),
                    color,
                    global_knowledge->embedding.at(v.get<unsigned>())
                    ));
            }
        }
        if (json_node.contains("H")) node->H = json_node["H"].get<double>();

        node->U = json_node.contains("U") ? json_node["U"].get<bool>() : node->U = std::nullopt;
        node->FL = json_node.contains("FL") ? json_node["FL"].get<bool>() : node->FL = std::nullopt;
        node->L = json_node.contains("L") ? json_node["L"].get<bool>() : node->L = std::nullopt;

        node->label = json_node.value("label", "unknown");
        node->graph = std::make_shared<ComparabilityBigraph>(vertices, p, q, dim, 1000);
        node->graph->constructE(true);

        if (json_node.contains("children") and json_node["children"].is_array()) {
            for (const auto &children_array = json_node["children"]; const auto & i : children_array) {
                auto child_node = std::make_shared<PartitionTreeNode>();
                parseJsonNode(i, child_node, global_knowledge, root_graph, depth + 1);
                node->children.push_back(child_node);
            }
        }
    }

    static void printPartitionTree(const std::shared_ptr<PartitionTreeNode> &node, int indent = 0) {
        std::string indent_str(indent * 2, ' ');

        std::cout << indent_str << "Node: " << node->label << "\n";
        std::cout << indent_str << "  Vertices: ";
        for (auto v : node->graph->enumerate()) {
            std::cout << v->getId() << " ";
        }
        std::cout << "\n";
        std::cout << indent_str << "  Dim: " << node->graph->getDimension() << ", H: " << node->H << "\n";

        auto print_opt = [](std::optional<bool> opt) -> std::string {
            if (!opt.has_value()) return "null";
            return opt.value() ? "true" : "false";
        };

        std::cout << indent_str << "  Flags: U=" << print_opt(node->U)
                  << " FL=" << print_opt(node->FL)
                  << " L=" << print_opt(node->L) << "\n";

        if (!node->children.empty()) {
            std::cout << indent_str << "  Children: " << node->children.size() << "\n";
            for (const auto &child : node->children) {
                printPartitionTree(child, indent + 1);
            }
        }
    }
};

#endif
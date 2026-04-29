#ifndef MEMOIRE_PARTITIONTREELOADER_HPP
#define MEMOIRE_PARTITIONTREELOADER_HPP

#include <nlohmann/json.hpp>
#include <fstream>
#include <vector>
#include <memory>
#include <map>

#include "Graph.hpp"
#include "../view/widgets/DrawableGraph.hpp"
class DrawableComparabilityBigraph;
using json = nlohmann::json;


struct PartitionTreeNode {
    std::vector<VertexPointer> vertices;
    double H;
    int dim;
    bool U, FL, L;
    std::string label;

    std::vector<std::shared_ptr<PartitionTreeNode>> children;
    std::shared_ptr<DrawableComparabilityBigraph> drawable;

    int depth;
    int drawSize;
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
                node->vertices.push_back(std::make_shared<ColoredEmbeddedVertex>(
                    v.get<unsigned>(),
                    color,
                    globalKnowledge->embedding.at(v.get<unsigned>())
                    ));
            }
        }
        if (jsonNode.contains("H")) node->H = jsonNode["H"].get<double>();
        if (jsonNode.contains("dim")) node->dim = jsonNode["dim"].get<unsigned>();
        node->U = jsonNode.value("U", NULL);
        node->FL = jsonNode.value("FL", NULL);
        node->L = jsonNode.value("L", NULL);
        node->label = jsonNode.value("label", "unknown");

        node->drawSize = std::max(100, 300 / (depth + 1));


        // ===== CRÉER LE DRAWABLE POUR CE NŒUD =====
        std::shared_ptr<ComparabilityBigraph> G = std::make_shared<ComparabilityBigraph>(node->vertices, p, q, node->dim, 900);
        node->drawable = std::make_shared<DrawableComparabilityBigraph>();
        node->drawable->linkGraph(G);
        node->drawable->addLine(node->H, 1);

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
        for (auto v : node->vertices) {
            std::cout << v->getId() << " ";
        }
        std::cout << "\n";
        std::cout << indentStr << "  Dim: " << node->dim << ", H: " << node->H << "\n";
        std::cout << indentStr << "  Flags: U=" << node->U << " FL=" << node->FL << " L=" << node->L << "\n";
        std::cout << indentStr << "  Size for drawing: " << node->drawSize << "x" << node->drawSize << "\n";

        if (!node->children.empty()) {
            std::cout << indentStr << "  Children: " << node->children.size() << "\n";
            for (const auto &child : node->children) {
                printPartitionTree(child, indent + 1);
            }
        }
    }
};

#endif

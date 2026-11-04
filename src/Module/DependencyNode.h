#ifndef DEPENDENCY_NODE_H
#define DEPENDENCY_NODE_H

#include <memory>
#include <vector>

class ValueType;

class DependencyNode {
public:
    static std::shared_ptr<DependencyNode> dependencyNode(std::shared_ptr<ValueType>, std::vector<std::shared_ptr<DependencyNode>> childDependencyNodes);

    DependencyNode(std::shared_ptr<ValueType>, std::vector<std::shared_ptr<DependencyNode>> childDependencyNodes);

    std::shared_ptr<ValueType> getValueType();
    std::vector<std::shared_ptr<DependencyNode>> getChildDependencyNodes();
    void appendChildDependencyNodes(std::vector<std::shared_ptr<DependencyNode>> childDependencyNodes);

private:
    static std::vector<std::shared_ptr<DependencyNode>> cachedNodes;

    std::shared_ptr<ValueType> valueType;
    std::vector<std::shared_ptr<DependencyNode>> childDependencyNodes;
    bool isVisited;
};

#endif
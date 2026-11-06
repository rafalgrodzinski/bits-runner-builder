#include "DependencyNode.h"

#include "Parser/ValueType/ValueType.h"
#include "Parser/ValueType/ValueTypeBlob.h"
#include "Parser/ValueType/ValueTypeProto.h"

using namespace std;

// MARK: - Public

shared_ptr<DependencyNode> DependencyNode::dependencyNode(shared_ptr<ValueType> valueType, vector<shared_ptr<DependencyNode>> childDependencyNodes) {
    if (valueType == nullptr)
        return make_shared<DependencyNode>(nullptr, childDependencyNodes);

    // We only care about blobs and protos
    if (!valueType->isBlob() && !valueType->isProto())
        return nullptr;

    // Try matching to one of nodes in the cache
    for (shared_ptr<DependencyNode> cachedNode : cachedNodes) {
        if (valueType->isBlob() && cachedNode->getValueType()->isBlob()) {
            if (valueType->toBlob()->getSymbolName()->isEqual(cachedNode->getValueType()->toBlob()->getSymbolName())) {
                cachedNode->appendChildDependencyNodes(childDependencyNodes);
                return cachedNode;
            }
        } else if (valueType->isProto() && cachedNode->getValueType()->isProto()) {
            if (valueType->toProto()->getSymbolName()->isEqual(cachedNode->getValueType()->toProto()->getSymbolName())) {
                cachedNode->appendChildDependencyNodes(childDependencyNodes);
                return cachedNode;
            }
        }
    }

    // Nothing found in the cache, create a new one
    shared_ptr<DependencyNode> dependencyNode = make_shared<DependencyNode>(valueType, childDependencyNodes);
    cachedNodes.push_back(dependencyNode);

    return dependencyNode;
}

void DependencyNode::resetCache() {
    cachedNodes.clear();
}

DependencyNode::DependencyNode(shared_ptr<ValueType> valueType, vector<shared_ptr<DependencyNode>> childDependencyNodes):
valueType(valueType),
childDependencyNodes(childDependencyNodes),
visitStatus(DependencyNodeVisitStatus::NOT_VISITED) { }

void DependencyNode::appendChildDependencyNodes(vector<shared_ptr<DependencyNode>> childDependencyNodes) {
    for (shared_ptr<DependencyNode> newChildDependencyNode : childDependencyNodes) {
        // For each node, check if we don't already have it in the list
        bool shouldAppend = true;
        for (shared_ptr<DependencyNode> childDependencyNode : this->childDependencyNodes) {
            // For blob
            if (newChildDependencyNode->getValueType()->isBlob() && childDependencyNode->getValueType()->isBlob()) {
                if (newChildDependencyNode->getValueType()->toBlob()->getSymbolName()->isEqual(childDependencyNode->getValueType()->toBlob()->getSymbolName())) {
                    shouldAppend = false;
                    break;
                }
            // For proto
            } else if (newChildDependencyNode->getValueType()->isProto() && childDependencyNode->getValueType()->isProto()) {
                if (newChildDependencyNode->getValueType()->toProto()->getSymbolName()->isEqual(childDependencyNode->getValueType()->toProto()->getSymbolName())) {
                    shouldAppend = false;
                    break;
                }
            }
        }

        // If not in the list, append it
        if (shouldAppend)
            this->childDependencyNodes.push_back(newChildDependencyNode);
    }
}

shared_ptr<ValueType> DependencyNode::getValueType() {
    return valueType;
}

vector<shared_ptr<DependencyNode>> DependencyNode::getChildDependencyNodes() {
    return childDependencyNodes;
}

DependencyNodeVisitStatus DependencyNode::getVisitStatus() {
    return visitStatus;
}

void DependencyNode::setVisitStatus(DependencyNodeVisitStatus visitStatus) {
    this->visitStatus = visitStatus;
}

// MARK: - Private

vector<shared_ptr<DependencyNode>> DependencyNode::cachedNodes;
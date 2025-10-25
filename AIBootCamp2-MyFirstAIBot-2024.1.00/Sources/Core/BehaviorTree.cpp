#include "Core/BehaviorTree.h"

void CompositeNode::AddChild(std::unique_ptr<Node> node) {
	children.push_back(std::move(node));
}

Status Sequence::Update(Agent& agent) const {
	// updates children from left to right until one fails
	for (const auto& child : children) {
		auto childStatus = child->Update(agent);
		if (childStatus != Status::SUCCESS) 
			return childStatus;
	}
	return Status::SUCCESS;
}

Status Selector::Update(Agent& agent) const {
	// updates children from left to right until one succeeds
	for (const auto& child : children) {
		auto childStatus = child->Update(agent);
		if (childStatus != Status::FAILURE)
			return childStatus;
	}
	return Status::FAILURE;
}

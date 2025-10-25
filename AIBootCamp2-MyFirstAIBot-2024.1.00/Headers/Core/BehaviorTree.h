#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include <vector>
#include <memory>
#include <functional>

#include "Agent.h"

enum class Status 
{
    SUCCESS,
    FAILURE,
    RUNNING
};

class Node {
public:
	virtual ~Node() = default;
	[[nodiscard]] virtual Status Update(Agent& agent) const = 0;
};

class Action : Node
{
public:
	explicit Action(std::function<Status(Agent&)> action) : action{std::move(action)} {};
	~Action() override = default;
	[[nodiscard]] Status Update(Agent& agent) const override {
		return action(agent);
	}

private:
	std::function<Status(Agent&)> action;
};

class CompositeNode : Node
{
public:
	~CompositeNode() override = default;
	[[nodiscard]] virtual Status Update(Agent& agent) const = 0;

	void AddChild(std::unique_ptr<Node> node);

protected:
	std::vector<std::unique_ptr<Node>> children{};
};

class Sequence : CompositeNode {
public:
	~Sequence() override = default;
	[[nodiscard]] Status Update(Agent& agent) const override;
};

class Selector : CompositeNode {
public:
	~Selector() override = default;
	[[nodiscard]] Status Update(Agent& agent) const override;
};

#endif
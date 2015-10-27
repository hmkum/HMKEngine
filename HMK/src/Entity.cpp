#include "Entity.h"
using namespace hmk;

Entity::Entity()
{
}

Entity::~Entity()
{
	for(auto &child : children_)
	{
		if(child)
			delete child;
	}
}

void Entity::set_name(const std::string& name)
{
	name_ = name;
}

std::string Entity::get_name() const
{
	return name_;
}

void Entity::set_parent(const std::shared_ptr<Entity> p)
{
	if(parent_ != p)
		parent_ = p;
}

std::shared_ptr<Entity> Entity::get_parent() const
{
	return parent_;
}

bool Entity::add_child(Entity* c)
{
	bool isAlreadyAChild = false;
	for(const auto& child : children_)
	{
		if(child == c)
			isAlreadyAChild = true;
	}

	if(isAlreadyAChild == false)
		children_.push_back(c);

	return (isAlreadyAChild == false);
}

Entity* Entity::get_child(std::string name) const
{
	for(const auto& child : children_)
	{
		if(child->get_name() == name)
			return child;
	}

	return nullptr;
}

std::vector<Entity*> Entity::get_children() const
{
	return children_;
}

bool Entity::has_child() const
{
	return (children_.size() > 0 ? true : false);
}
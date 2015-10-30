#include "Entity.h"
using namespace hmk;

Entity::Entity()
{
}

Entity::~Entity()
{
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

bool Entity::add_child(std::shared_ptr<Entity> c)
{
	// TODO_HMK: Parentinın parentını child olarak eklemeye kalkarsan hata veriyor.
	// scene_models[1]->add_child(scene_models[2]);
	// scene_models[2]->add_child(scene_models[3]);
	// scene_models[3]->add_child(scene_models[1]); // Ekleyememeli
	bool isAlreadyAChild = false;
	for(const auto& child : children_)
	{
		if(child == c)
		{
			isAlreadyAChild = true;
			break;
		}
	}

	if(isAlreadyAChild == false)
	{
		children_.push_back(c);
		c->set_parent(std::make_shared<Entity>(*this));
	}

	return (isAlreadyAChild == false);
}

std::shared_ptr<Entity> Entity::get_child(std::string name) const
{
	for(const auto& child : children_)
	{
		if(child->get_name() == name)
			return child;
	}

	return nullptr;
}

std::vector<std::shared_ptr<Entity>> Entity::get_children() const
{
	return children_;
}

bool Entity::has_child() const
{
	return (children_.size() > 0 ? true : false);
}
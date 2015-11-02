#pragma once
#include <memory>
#include <string>
#include <vector>

namespace hmk
{
class Entity
{
public:
	Entity();
	virtual ~Entity();

	virtual void set_name(const std::string& name);
	virtual std::string get_name() const;

	virtual void set_parent(const std::shared_ptr<Entity> p);
	virtual std::shared_ptr<Entity> get_parent() const;

	virtual bool add_child(std::shared_ptr<Entity> c);
	virtual std::shared_ptr<Entity> get_child(std::string name) const;
	virtual std::vector<std::shared_ptr<Entity>> get_children() const;

	virtual bool has_child() const;

protected:
	std::string name_;
	std::shared_ptr<Entity> parent_;
	std::vector<std::shared_ptr<Entity>> children_;
};

using EntitySPtr = std::shared_ptr<Entity>;
using EntityUPtr = std::unique_ptr<Entity>;
}
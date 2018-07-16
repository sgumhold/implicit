#pragma once

#include "implicit_group.h"

template <typename T>
struct union_node : 
	public v3_func<T,T>,
	public implicit_group
{
	std::vector<v3_func<T,T>*> children;

	union_node()
	{
		set_name("union");
	}
	unsigned int append_child(base_ptr child)
	{
		unsigned int i = implicit_group::append_child(child);
		children.push_back(child->get_interface<v3_func<T,T> >());
		return i;
	}
	
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = children[0]->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<children.size(); ++i) {
			T new_value = children[i]->evaluate(p);
			if (new_value < value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (children.empty())
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (children.empty())
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return children[i]->evaluate_gradient(p);
	}

	void create_gui()
	{
		static std::string heading("union");
		add_view("", heading)->set("color", 0xffff00);
		implicit_group::create_gui();
	}
	std::string get_type_name() const
	{
		return "union";
	}	
};

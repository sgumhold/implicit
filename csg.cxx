#include "scene.h"
#include "implicit_group.h"

template <typename T>
class difference_node : public implicit_group<T>
{
public:
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = func_children[0]->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<func_children.size(); ++i) {
			T new_value = -func_children[i]->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return i == 0 ? func_children[i]->evaluate_gradient(p) : -func_children[i]->evaluate_gradient(p);
	}

	void create_gui()
	{
		add_view("difference", name)->set("color", 0xffff00);
		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "difference_node";
	}	
};

template <typename T>
class union_node : public implicit_group<T>
{
public:
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = func_children[0]->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<func_children.size(); ++i) {
			T new_value = func_children[i]->evaluate(p);
			if (new_value < value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return func_children[i]->evaluate_gradient(p);
	}

	void create_gui()
	{
		add_view("union", name)->set("color", 0xffff00);
		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "union_node";
	}	
};

template <typename T>
class intersection_node : public implicit_group<T>
{
public:
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = func_children[0]->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<func_children.size(); ++i) {
			T new_value = func_children[i]->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return func_children[i]->evaluate_gradient(p);
	}

	void create_gui()
	{
		add_view("intersection", name)->set("color", 0xffff00);
		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "intersection_node";
	}	
};

template <typename T>
class complement_node : public implicit_group<T>
{
public:
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = func_children[0]->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<func_children.size(); ++i) {
			T new_value = func_children[i]->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return -func_children[0]->evaluate(p);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		return -func_children[0]->evaluate_gradient(p);
	}

	void create_gui()
	{
		add_view("complement", name)->set("color", 0xffff00);
		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "complement_node";
	}	
};

scene_factory_registration<difference_node<double> >sfr_difference("-;difference");
scene_factory_registration<union_node<double> >sfr_union("+;union");
scene_factory_registration<intersection_node<double> >sfr_intersect("*;intersection");
scene_factory_registration<complement_node<double> >sfr_complement("!;complement");
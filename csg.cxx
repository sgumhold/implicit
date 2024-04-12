#include "implicit_group.h"

template <typename T>
class difference_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;

	difference_node() { gui_color = 0xffff00; }
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = get_implicit_child(0)->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<get_nr_children(); ++i) {
			T new_value = -get_implicit_child(i)->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return i == 0 ? get_implicit_child(i)->evaluate_gradient(p) : -get_implicit_child(i)->evaluate_gradient(p);
	}
	std::string get_type_name() const {
		return "difference_node";
	}	
};

template <typename T>
class union_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;

	union_node() { gui_color = 0xffff00; }
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = get_implicit_child(0)->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<get_nr_children(); ++i) {
			T new_value = get_implicit_child(i)->evaluate(p);
			if (new_value < value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return get_implicit_child(i)->evaluate_gradient(p);
	}
	std::string get_type_name() const {
		return "union_node";
	}	
};

template <typename T>
class intersection_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;

	intersection_node() { gui_color = 0xffff00; }
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = get_implicit_child(0)->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<get_nr_children(); ++i) {
			T new_value = get_implicit_child(i)->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		unsigned int i;
		return eval_and_get_index(p, i);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		unsigned int i;
		eval_and_get_index(p, i);
		return get_implicit_child(i)->evaluate_gradient(p);
	}

	std::string get_type_name() const {
		return "intersection_node";
	}	
};

template <typename T>
class complement_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;

	complement_node() { gui_color = 0xffff00; }
	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const {
		T value = get_implicit_child(0)->evaluate(p);
		selected_i = 0;
		for (unsigned int i=1; i<get_nr_children(); ++i) {
			T new_value = get_implicit_child(i)->evaluate(p);
			if (new_value > value) {
				selected_i = i;
				value = new_value;
			}
		}
		return value;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		return -get_implicit_child(0)->evaluate(p);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		return -get_implicit_child(0)->evaluate_gradient(p);
	}

	std::string get_type_name() const {
		return "complement_node";
	}	
};

scene_factory_registration<difference_node<double> >sfr_difference("-;difference");
scene_factory_registration<union_node<double> >sfr_union("+;union");
scene_factory_registration<intersection_node<double> >sfr_intersect("*;intersection");
scene_factory_registration<complement_node<double> >sfr_complement("!;complement");

#include "implicit_group.h"
#include "implicit_primitive.h"

/// passes on the update handler to the children
template <typename T>
void implicit_group<T>::set_update_handler(scene_update_handler* uh)
{
	implicit_base<T>::set_update_handler(uh);
	for (size_t i = 0; i<get_nr_children(); ++i)
		get_implicit_child(i)->set_update_handler(uh);
}

/// reflect members to expose them to serialization
template <typename T>
bool implicit_group<T>::self_reflect(cgv::reflect::reflection_handler& rh)
{
	if (rh.reflect_member("cr", color[0]) &&
		rh.reflect_member("cg", color[1]) &&
		rh.reflect_member("cb", color[2]) &&
		rh.reflect_member("ca", color[3])) {

		for (size_t i = 0; i < get_nr_children(); ++i) {
			if (!rh.reflect_member(std::string("child") + cgv::utils::to_string(i), (bool&)(child_visible_in_gui[i])))
				return false;
		}
	}
	return true;
}

/// access to implicit base interface of children
template <typename T>
implicit_base<T>* implicit_group<T>::get_implicit_child(unsigned i)
{
	return get_child(i)->get_interface<implicit_base<T> >();
}
/// const access to implicit base interface of children
template <typename T>
const implicit_base<T>* implicit_group<T>::get_implicit_child(unsigned i) const
{
	return get_child(i)->get_interface<implicit_base<T> >();
}

template <typename T>
void implicit_group<T>::on_set(void* member_ptr)
{
	for (size_t i = 0; i < get_nr_children(); ++i) {
		bool& toggle = ref_tree_node_visible_flag(*get_implicit_child(i));
		if (member_ptr == &child_visible_in_gui[i]) {
			toggle = (bool&)child_visible_in_gui[i];
			update_description();
			return;
		}
		if (member_ptr == &toggle) {
			(bool&)(child_visible_in_gui[i]) = toggle;
			update_description();
			return;
		}
	}
	update_member(member_ptr);
	update_scene();
}


template <typename T>
unsigned int implicit_group<T>::append_child(base_ptr child)
{
	size_t i = group::append_child(child);
	child_visible_in_gui.push_back(1);
	return i;
}

/// overload to compose the colors of the function children
template <typename T>
typename implicit_group<T>::clr_type implicit_group<T>::compose_color(const pnt_type& p) const
{
	size_t n = get_nr_children();
	switch (n) {
	case 0: return color;
	case 1: return get_implicit_child(0)->evaluate_color(p);
	default: {
		clr_type result(0, 0, 0, 0);
		for (size_t i = 0; i < n; ++i)
			result += 1.0f / float(n) * get_implicit_child(i)->evaluate_color(p);
		return result;
	}
	}
}

/// evaluation of surface color based on color_mode
template <typename T>
typename implicit_group<T>::clr_type implicit_group<T>::evaluate_color(const pnt_type& p) const
{
	switch (color_mode) {
	case GCM_REPLACE: return color;
	case GCM_COMPOSE: return compose_color(p);
	default: {
		if (get_nr_children()==0)
			return color;
		unsigned i = color_mode - GCM_CHILD_0;
		if (i >= get_nr_children())
			return color;
		return get_implicit_child(i)->evaluate_color(p);
	}
	}
}

template <typename T>
void implicit_group<T>::create_gui()
{
	align("\a");
	for (size_t i=0; i<get_nr_children(); ++i) {
		implicit_base<T>* child_ptr = get_implicit_child(i);
		ref_tree_node_visible_flag(*child_ptr) = (bool&)child_visible_in_gui[i];

		// add tree node
		std::string label = std::string("[") + cgv::utils::to_string(i) + "]";
		bool node_visible = begin_tree_node(label, *child_ptr, (bool&)child_visible_in_gui[i], "level=3;options='w=20';align=' '");

		// add gui for child name with gui color of child
		std::string& child_name = const_cast<std::string&>(child_ptr->get_base()->get_named()->get_name());
		add_member_control(child_ptr->get_base(), "name", child_name, "", "w=140;label=''", "");
		find_control(child_name)->set("color", child_ptr->gui_color);
		// add gui for color of child
		add_member_control(child_ptr->get_base(), "color", child_ptr->color, "", "w=40;label=''");

		if (node_visible) {
			inline_object_gui(get_child(i));
			end_tree_node(*child_ptr);
		}
	}
	align("\b");
}

template <typename T>
std::string implicit_group<T>::get_type_name() const
{
	return "implicit_group";
}

template <typename T>
bool implicit_group<T>::init(context& ctx)
{
	bool success = true;
	for (size_t i=0; i<get_nr_children(); ++i)
		success = get_implicit_child(i)->init(ctx) && success;
	return success;
}


template class implicit_group<double>;
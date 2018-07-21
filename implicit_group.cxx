#pragma once

#include "implicit_group.h"
/*
struct provider_access : public cgv::gui::provider
{
	cgv::gui::gui_group_ptr get_parent() { return parent_group; }
	/// the gui window sets the parent group through this method
	void set_parent_public(cgv::gui::gui_group_ptr _parent) { set_parent(_parent); }
};
*/

/// reflect members to expose them to serialization
template <typename T>
bool implicit_group<T>::self_reflect(cgv::reflect::reflection_handler& rh)
{
	for (unsigned i = 0; i < get_nr_children(); ++i) {
		if (!rh.reflect_member(std::string("child") + cgv::utils::to_string(i), (bool&)(child_visible_in_gui[i])))
			return false;
	}
	return true;
}

template <typename T>
void implicit_group<T>::on_set(void* member_ptr)
{
	for (unsigned i = 0; i < get_nr_children(); ++i) {
		provider* pp = get_child(i)->get_interface<provider>();
		if (!pp)
			continue;
		bool& toggle = ref_tree_node_visible_flag(*pp);
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
	update_scene();
}


template <typename T>
unsigned int implicit_group<T>::append_child(base_ptr child)
{
	unsigned int i = group::append_child(child);
	func_children.push_back(child->get_interface<v3_func<T,T> >());
	child_visible_in_gui.push_back(1);
	return i;
}

template <typename T>
void implicit_group<T>::create_gui()
{
	align("\a");
	for (unsigned int i=0; i<get_nr_children(); ++i) {
		base_ptr bp = get_child(i);
		provider* pp = bp->get_interface<provider>();
		if (pp) {
			ref_tree_node_visible_flag(*pp) = (bool&)child_visible_in_gui[i];
			if (begin_tree_node(std::string("child ") + cgv::utils::to_string(i), *pp, (bool&)child_visible_in_gui[i], "level=3")) {
				inline_object_gui(bp);
				end_tree_node(*pp);
			}
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
void implicit_group<T>::set_scene(scene* _s)
{
	scene_updater::set_scene(_s);
	for (unsigned int i=0; i<get_nr_children(); ++i) {
		base_ptr bp = get_child(i);
		scene_updater* su = bp->get_interface<scene_updater>();
		if (su)
			su->set_scene(_s);
	}
}

template <typename T>
bool implicit_group<T>::init(context& ctx)
{
	bool success = true;
	for (unsigned int i=0; i<get_nr_children(); ++i) {
		base_ptr bp = get_child(i);
		drawable* dp = bp->get_interface<drawable>();
		if (dp)
			success = dp->init(ctx) && success;
	}
	return success;
}


template class implicit_group<double>;
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

template <typename T>
void implicit_group<T>::on_set(void* member_ptr)
{
	update_scene();
}


template <typename T>
unsigned int implicit_group<T>::append_child(base_ptr child)
{
	unsigned int i = group::append_child(child);
	func_children.push_back(child->get_interface<v3_func<T,T> >());
	return i;
}

template <typename T>
void implicit_group<T>::create_gui()
{
	align("\a");
	for (unsigned int i=0; i<get_nr_children(); ++i) {
		base_ptr bp = get_child(i);
		provider* pp = bp->get_interface<provider>();
		if (pp)
			inline_object_gui(bp);
/*			provider_access* ppa = static_cast<provider_access*>(pp);
			gui_group_ptr my_parent = static_cast<provider_access*>((provider*)this)->get_parent();
			ppa->set_parent_public(my_parent);
			pp->create_gui();
		}*/
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
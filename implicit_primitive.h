#pragma once

#include <cgv/base/named.h>
#include "implicit_base.h"

using namespace cgv::base;

/** base implementation for all group nodes*/
template <typename T>
class implicit_primitive : public named, public implicit_base<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_base<T> base;
		using base::color;
		using base::update_scene;
		using base::update_member;

	/// convert to cgv::base::base pointer
	cgv::base::base* get_base() { return this; }
	/// calls the update_scene method of scene_updater
	void on_set(void* member_ptr);
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh);
	/// returns "implicit_primitive"
	std::string get_type_name() const;
	/// create gui of children. Call this inside implementations of create_gui of derived classes.
	void create_gui();
};

#pragma once

#include <cgv/base/group.h>
#include <cgv/math/mfunc.h>
#include <cgv/gui/provider.h>
#include <cgv/render/drawable.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::render;
using namespace cgv::render::gl;
using namespace cgv::gui;

/** base implementation for all group nodes*/
template <typename T>
class implicit_group : 
	public group, 
	public v3_func<T,T>, 
	public scene_updater,
	public drawable,
	public provider
{
protected:
	/// give access to children of group as trivariate functions
	std::vector<v3_func<T,T>*> func_children;
	/// store for each child a flag whether the child is visible in the gui
	std::vector<int> child_visible_in_gui;
	/// use this type to avoid allocation on the heap
	typedef cgv::math::fvec<double,3> fvec_type;
public:
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh);
	/// calls the update_scene method of scene_updater
	void on_set(void* member_ptr);
	/// append a new child and extract trivariate function
	unsigned int append_child(base_ptr child);
	/// returns "implicit_group"
	std::string get_type_name() const;
	/// passes on init to the children
	bool init(context&);
	/// passes on the scene pointer to the children
	void set_scene(scene* _s);
	/// create gui of children. Call this inside implementations of create_gui of derived classes.
	void create_gui();
};

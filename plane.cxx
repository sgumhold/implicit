#include <cgv/math/mfunc.h>
#include <cgv/gui/provider.h>
#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::gui;

template <typename T>
struct plane : 
	public v3_func<T,T>, 
	public provider,
	public scene_updater,
	public named
{
	/// use this type to avoid allocation on the heap
	typedef cgv::math::fvec<double, 3> fvec_type;

	fvec_type normal;
	double   distance;

	plane() : normal(0, 0, 1), distance(0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("d", distance) &&
			rh.reflect_member("nx", normal(0)) &&
			rh.reflect_member("ny", normal(1)) &&
			rh.reflect_member("nz", normal(2));
	}

	void on_set(void* member_ptr)
	{
		update_member(member_ptr);
		update_scene();
	}
	std::string get_type_name() const { return "plane"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return dot(p, normal.to_vec()) - distance;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		return normal.to_vec();
	}
	void create_gui()
	{
		add_view("plane",name)->set("color",0xFF8888);
		add_gui("normal", normal, "direction", "options='min=-1;max=1;ticks=true'");
		add_member_control(this, "distance", distance, "value_slider", "min=-2;max=2;ticks=true");
	}
};

scene_factory_registration<plane<double> >sfr_plane("plane;P");
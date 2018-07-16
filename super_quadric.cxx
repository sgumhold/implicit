#include <cgv/reflect/reflection_handler.h>
#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::signal;
using namespace cgv::gui;
using namespace cgv::type;


template <typename T>
struct super_quadric : 
	public v3_func<T,T>, 
	public scene_updater,
	public provider,
	public named
{
	/// the two parameters of the super quadric defining the norm
	T p1, p2;
	/// reciprocals of parameters
	T inv_p1, inv_p2;
	/// construct with default values for the p_i
	super_quadric() {
		p1 = p2 = 2; 
		inv_p1 = inv_p2 = T(0.5);
	}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("p1", p1) &&
			rh.reflect_member("p2", p2);
	}
	/// ensure that reciprocals are computed and that scene is updated if parameters change
	void on_set(void* member_ptr)
	{
		if (member_ptr == &p1)
			inv_p1 = (p1 < 1e-12) ? 1e12 : 1 / p1;
		if (member_ptr == &p2)
			inv_p2 = (p2 < 1e-12) ? 1e12 : 1 / p2;
		update_scene();
	}
	/// helper function
	static T signum(const T& v)
	{
		if (abs(v) < 1e-12)
			return 0;
		return v > 0 ? 1 : -1;
	}
	/// overload to return the type name of this object
	std::string get_type_name() const
	{
		return "super_quadric";
	}

	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		T xy = pow(abs(p(0)),p1) + pow(abs(p(1)),p1);
		return pow(pow(xy, inv_p1*p2) + pow(abs(p(2)),p2), inv_p2) - 1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		T xy = pow(abs(p(0)),p1) + pow(abs(p(1)),p1);
		return vec_type(p2*pow(abs(p(0)),p1-1)*signum(p(0))*pow(xy, inv_p1*p2-1), 
			             p2*pow(abs(p(1)),p1-1)*signum(p(1))*pow(xy, inv_p1*p2-1), 
							 p2*pow(abs(p(2)),p1-1)*signum(p(2)));
	}

	void create_gui()
	{
		add_view("superquadric",name)->set("color",0xFF8888);
		add_member_control(this, "p1", p1, "value_slider", "min=0;max=50;ticks=true;log=true");
		add_member_control(this, "p2", p2, "value_slider", "min=0;max=50;ticks=true;log=true");
	}
};

scene_factory_registration<super_quadric<double> >sfr_super_quadric("super_quadric;U");
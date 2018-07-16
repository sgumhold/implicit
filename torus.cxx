#include <cgv/math/mfunc.h>
#include <cgv/base/base.h>
#include <cgv/signal/rebind.h>
#include <cgv/type/variant.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::signal;
using namespace cgv::type;

#define sqr(x) ((x)*(x))

template <typename T>
class torus : 
	public v3_func<T,T>, 
	public provider,
	public scene_updater,
	public named
{
public:
	T r;
	T R;
	std::string get_type_name() const { return "torus"; }
	torus()
	{
		r = (T)0.2;
		R = (T)0.8;
	}
	/// semicolon separated list of property declarations
	std::string get_property_declarations() { return "r:flt64;R:flt64"; }
	/// abstract interface for the setter of a dynamic property, by default it simply returns false
	bool set_void(const std::string& property, const std::string& value_type, const void* value_ptr) {
		if (property == "r") {
			get_variant(r,value_type,value_ptr);
			update_scene();
			return true;
		}
		else if (property == "R") {
			get_variant(R, value_type,value_ptr);
			update_scene();
			return true;
		}
		return false;
	}
	/// abstract interface for the getter of a dynamic property, by default it simply returns false
	bool get_void(const std::string& property, const std::string& value_type, void* value_ptr) {
		if (property == "r") {
			set_variant(r,value_type,value_ptr);
			return true;
		}
		else if (property == "R") {
			set_variant(R,value_type,value_ptr);
			return true;
		}
		return false;
	}
	T evaluate(const pnt_type& p) const {
		return sqr(p.sqr_length()+sqr(R)-sqr(r))-4*sqr(R)*(sqr(p(0))+sqr(p(1)));
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		T t0 = sqr(R); 
		T t1 = 4*(p.sqr_length()+t0-sqr(r));
		T t2 = 8*t0;
		T t3 = t1-t2;
		return vec_type(p(0)*t3, p(1)*t3,p(2)*t1);
	}
	void create_gui()
	{
		add_view("torus",name)->set("color",0xFF8888);
		add_control("r", r, "value_slider", "min=0;max=2;ticks=true");
		add_control("R", R, "value_slider", "min=0;max=3;ticks=true");
		connect_copy(find_control(r)->value_change, rebind( 
			static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(R)->value_change, rebind( 
			static_cast<scene_updater*>(this), &scene_updater::update_scene));
	}
};

scene_factory_registration<torus<double> >sfr_torus('T');
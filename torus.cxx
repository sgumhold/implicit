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
	bool use_euclidean_distance;
	std::string get_type_name() const { return "torus"; }
	torus() : use_euclidean_distance(true), r(T(0.2)), R(T(0.8)) {}
	void on_set(void* member_ptr) { update_scene(); }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("r", r) &&
			rh.reflect_member("R", R);
	}
	T evaluate(const pnt_type& p) const {
		if (use_euclidean_distance) {
			T l_xy = sqrt(sqr(p(0)) + sqr(p(1)));
			if (l_xy < std::numeric_limits<T>::epsilon() * 10)
				return sqrt(sqr(R) + sqr(p(2))) - r;
			return (p - R / l_xy * pnt_type(p(0), p(1), 0)).length() - r;
		}
		else
			return sqr(p.sqr_length()+sqr(R)-sqr(r))-4*sqr(R)*(sqr(p(0))+sqr(p(1)));
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (use_euclidean_distance) {
			T l_xy = sqrt(sqr(p(0)) + sqr(p(1)));
			if (l_xy < std::numeric_limits<T>::epsilon() * 10)
				return pnt_type(0, 0, 0);
			pnt_type grad_vec = p - R / l_xy * pnt_type(p(0), p(1), 0);
			grad_vec.normalize();
			return grad_vec;
		}
		else {
			T t0 = sqr(R);
			T t1 = 4 * (p.sqr_length() + t0 - sqr(r));
			T t2 = 8 * t0;
			T t3 = t1 - t2;
			return vec_type(p(0)*t3, p(1)*t3, p(2)*t1);
		}
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

scene_factory_registration<torus<double> >sfr_torus("torus;T");
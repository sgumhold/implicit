#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;

template <typename T>
struct cylinder : 
	public v3_func<T,T>,
	public cgv::gui::provider,
	public scene_updater,
	public named
{
	bool use_euclidean_distance;
	cylinder() : use_euclidean_distance(true) {}
	std::string get_type_name() const { return "cylinder"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		if (use_euclidean_distance)
			return sqrt(p(0)*p(0)+p(1)*p(1))-1;
		else
			return p(0)*p(0) + p(1)*p(1) - 1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (use_euclidean_distance) {
			T l_xy = sqrt(p(0)*p(0) + p(1)*p(1));
			if (l_xy < std::numeric_limits<T>::epsilon() * 10)
				return vec_type(0, 0, 0);
			return T(1) / l_xy * pnt_type(p(0), p(1), 0);
		}
		else {
			return vec_type(2 * p(0), 2 * p(1), 0);
		}
	}
	void on_set(void* member_ptr) { update_scene(); }
	void create_gui()
	{
		add_view("cylinder", name)->set("color", 0xFF8888);
		add_member_control(this, "use_euclidean_distance", use_euclidean_distance, "check");
	}
};

scene_factory_registration<cylinder<double> >sfr_cylinder("cylinder;Y");
#include "scene.h"
#include <cgv/base/named.h>

template <typename T>
struct sphere : 
	public cgv::math::v3_func<T,T>, 
	public cgv::gui::provider,
	public scene_updater,
	public cgv::base::named
{
	bool use_euclidean_distance;
	sphere():use_euclidean_distance(true) {}
	std::string get_type_name() const { return "sphere"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		if (use_euclidean_distance)
			return p.length() - 1;
		else
			return p.sqr_length() - 1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (use_euclidean_distance) {
			T l = p.length();
			if (l < std::numeric_limits<T>::epsilon() * 10)
				return vec_type(0, 0, 0);
			return (1 / l)*p;
		}
		else
			return T(2) * p;
	}
	void on_set(void* member_ptr) { update_scene(); }
	void create_gui()
	{
		add_view("sphere",name)->set("color",0xFF8888);
		add_member_control(this, "use_euclidean_distance", use_euclidean_distance, "check");
	}
};

scene_factory_registration<sphere<double> > sfr_sphere("sphere;S");
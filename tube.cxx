#include "implicit_primitive.h"

template <typename T>
class tube : public implicit_primitive<T>
{
public:
	fvec<T, 2> r_range;
	fvec<T, 2> z_range;
	std::string get_type_name() const { return "tube"; }
	tube() : r_range(0,1), z_range(-1,1) { gui_color = 0xfA88fA; }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("r", r_range[0]) &&
			rh.reflect_member("R", r_range[1]) &&
			rh.reflect_member("z", z_range[0]) &&
			rh.reflect_member("Z", z_range[1]) &&
			implicit_primitive<T>::self_reflect(rh);
	}
	T evaluate(const pnt_type& p) const {
		T xy2 = sqr(p(0)) + sqr(p(1));
		T xy  = sqrt(xy2);
		T r2  = sqr(r_range[0]);
		T R2  = sqr(r_range[1]);
		T dz;
		// detect outside with respect to z and store amount in dz
		if (p(2) < z_range[0])
			dz = z_range[0] - p(2);
		else if (p(2) > z_range[1])
			dz = p(2) - z_range[1];
		else {
			// here we are inside with respect to z

			// inside of smaller radius
			if (xy2 < r2)
				return r_range[0] - xy;
			// outside of larger radius
			if (xy2 > R2)
				return xy - r_range[1];
			// when in interior of tube, return negated minimum distance to boundary surfaces
			return -std::min(std::min(r_range[1] - xy,xy- r_range[0]),std::min(p(2)-z_range[0],z_range[1]-p(2)));
		}
		if (xy2 < r2)
			return sqrt(sqr(r_range[0] - xy) + sqr(dz));
		if (xy2 > R2)
			return sqrt(sqr(xy - r_range[1]) + sqr(dz));
		return dz;
	}
//	vec_type evaluate_gradient(const pnt_type& p) const {
//	}
	void create_gui()
	{
		add_gui("r_range", r_range, "ascending", "components='rR';options='min=0;max=2;ticks=true'");
		add_gui("z_range", z_range, "ascending", "components='zZ';options='min=-2;max=2;ticks=true'");
	}
};

scene_factory_registration<tube<double> >sfr_torus("tube");
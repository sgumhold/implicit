#include "implicit_primitive.h"

template <typename T>
class torus : public implicit_primitive<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_primitive<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::evaluate_gradient;
		using base::add_member_control;

	T r;
	T R;
	bool use_euclidean_distance;
	std::string get_type_name() const { return "torus"; }
	torus() : use_euclidean_distance(true), r(T(0.2)), R(T(0.8)) { gui_color = 0xFF8888; }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("r", r) &&
			rh.reflect_member("R", R) &&
			implicit_primitive<T>::self_reflect(rh);
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
		add_member_control(this, "r", r, "value_slider", "min=0;max=2;ticks=true");
		add_member_control(this, "R", R, "value_slider", "min=0;max=3;ticks=true");
	}
};

scene_factory_registration<torus<double> > sfr_torus("torus;T");

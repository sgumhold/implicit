#include "implicit_primitive.h"


template <typename T>
struct cylinder :  public implicit_primitive<T>
{
	bool use_euclidean_distance;
	cylinder() : use_euclidean_distance(true) { gui_color = 0xFF8888; }
	std::string get_type_name() const { return "cylinder"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		if (use_euclidean_distance)
			return sqrt(sqr(p(0))+sqr(p(1)))-1;
		else
			return sqr(p(0)) + sqr(p(1)) - 1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (use_euclidean_distance) {
			T l_xy = sqrt(sqr(p(0)) + sqr(p(1)));
			if (l_xy < std::numeric_limits<T>::epsilon() * 10)
				return vec_type(0, 0, 0);
			return T(1) / l_xy * pnt_type(p(0), p(1), 0);
		}
		else {
			return vec_type(2 * p(0), 2 * p(1), 0);
		}
	}
	void create_gui()
	{
		implicit_primitive<T>::create_gui();
		add_member_control(this, "use_euclidean_distance", use_euclidean_distance, "check");
	}
};

scene_factory_registration<cylinder<double> >sfr_cylinder("cylinder;Y");
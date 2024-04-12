#include "implicit_primitive.h"

template <typename T>
struct sphere : public implicit_primitive<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_primitive<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::evaluate_gradient;
		using base::add_member_control;

	bool use_euclidean_distance;
	sphere() : use_euclidean_distance(true) 
	{
		gui_color = 0xFF8888;
	}
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
	void create_gui()
	{
		base::add_member_control(this, "use_euclidean_distance", use_euclidean_distance, "check");
	}
};

scene_factory_registration<sphere<double> > sfr_sphere("sphere;S");

#include "implicit_primitive.h"

template <typename T>
struct heart : public implicit_primitive<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_primitive<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::evaluate_gradient;

	std::string get_type_name() const { return "heart"; }
	T evaluate(const pnt_type& p) const {
		T t1 = p.sqr_length()-1;
		T t2 = ((T)0.2*p(0)*p(0)+p(1)*p(1))*p(2)*p(2)*p(2);
		return t1*t1*t1-t2;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		const T& x = p(0);
		T x2 = x * x;
		const T& y = p(1);
		T y2 = y * y;
		const T& z = p(2);
		T z2 = z * z;
		T t1 = x2+y2+z2 - 1.0;
		T z3 = z2*z;
		return 6 * t1 * t1 * p - vec_type(0.4*x*z3, 2.0*y*z3, 3.0*(0.2*x2+y2)*z2);
	}
};

scene_factory_registration<heart<double> >sfr_heart("heart");

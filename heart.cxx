#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;

template <typename T>
struct heart : 
	public v3_func<T,T>, 
	public named
{
public:
	std::string get_type_name() const { return "heart"; }
	T evaluate(const pnt_type& p) const {
		T t1 = p.sqr_length()-1;
		T t2 = ((T)0.2*p(0)*p(0)+p(1)*p(1))*p(2)*p(2)*p(2);
		return t1*t1*t1-t2;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		const T x = p[0];
		const T y = p[1];
		const T z = p[2];
		const T t1 = x*x;
		const T t3 = y*y;
		const T t4 = z*z;
		const T t5 = 2.0*t1+t3+t4-1.0;
		const T t6 = t5*t5;
		return vec_type(12.0*t6*x-0.4*x*t4*z, 6.0*t6*y-2.0*y*t4*z, 6.0*t6*z-0.6*t1*t4-3.0*t3*t4);
	}
};

scene_factory_registration<heart<double> >sfr_heart('H');
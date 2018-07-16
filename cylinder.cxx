#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;

template <typename T>
struct cylinder : 
	public v3_func<T,T>, 
	public named
{
	std::string get_type_name() const { return "cylinder"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return p(0)*p(0)+p(1)*p(1)-1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		return pnt_type(2*p(0),2*p(1),0);
	}
	void create_gui()
	{
		add_view("cylinder", name)->set("color", 0xFF8888);
	}
};

scene_factory_registration<cylinder<double> >sfr_cylinder("Y;cylinder");
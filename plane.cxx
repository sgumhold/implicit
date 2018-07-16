#include <cgv/math/mfunc.h>
#include <cgv/gui/provider.h>
#include <cgv/base/named.h>
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::gui;

template <typename T>
struct plane : 
	public v3_func<T,T>, 
	public provider,
	public named
{
	std::string get_type_name() const { return "plane"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return p(2);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		return vec_type(0,0,1);
	}
	void create_gui()
	{
		add_view("plane",name)->set("color",0xFF8888);
	}
};

scene_factory_registration<plane<double> >sfr_plane("P;plane");
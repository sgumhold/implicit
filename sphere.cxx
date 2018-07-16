#include "scene.h"
#include <cgv/base/named.h>

template <typename T>
struct sphere : 
	public cgv::math::v3_func<T,T>, 
	public cgv::gui::provider,
	public cgv::base::named
{
	std::string get_type_name() const { return "sphere"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return p.sqr_length()-1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		return 2.0*p;
	}
	void create_gui()
	{
		add_view("sphere",name)->set("color",0xFF8888);
	}
};

scene_factory_registration<sphere<double> > sfr_sphere("S;sphere");
#include "scene.h"
#include <cgv/base/named.h>

template <typename T>
struct box : 
	public cgv::math::v3_func<T,T>, 
	public cgv::gui::provider,
	public cgv::base::named
{
	std::string get_type_name() const { return "box"; }
	static int abs_max_idx(const pnt_type& p)
	{
		if (fabs(p(0)) > fabs(p(1))) {
			if (fabs(p(0)) > fabs(p(2)))
				return 0;
			else
				return 2;
		}
		else {
			if (fabs(p(1)) > fabs(p(2)))
				return 1;
			else
				return 2;
		}
	}
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return abs(p(abs_max_idx(p)))-1;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		vec_type g(0,0,0);
		int i = abs_max_idx(p);
		g(i) = p(i) > 0 ? 1 : -1;
		return g;
	}
	void create_gui()
	{
		add_view("box",name)->set("color",0xFF8888);
	}
};

scene_factory_registration<box<double> >sfr_box("B;box");
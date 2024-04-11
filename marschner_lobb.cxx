#define _USE_MATH_DEFINES
#include<cmath>
#include "implicit_primitive.h"

template <typename T>
struct marschner_lobb :  public implicit_primitive<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_primitive<T> base;
		using typename base::pnt_type;
		using base::gui_color;

	double alpha;
	double fM;
	marschner_lobb() : alpha(0.25), fM(6.0) { gui_color = 0xFF8888; }
	std::string get_type_name() const { return "marschner_lobb"; }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		const T& x = p.x();
		const T& y = p.y();
		const T& z = p.z();
		double rho_r = cos(2.0 * M_PI * fM * cos(0.5 * M_PI * sqrt(x * x + y * y)));
		double v = ((1.0 - sin(0.5 * M_PI * z)) + alpha * (1.0 + rho_r)) / (2.0 * (1 + alpha)) - 0.5;
		return v;
	}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			base::self_reflect(rh) &&
			rh.reflect_member("alpha", alpha) &&
			rh.reflect_member("fM", fM);
	}

	void create_gui()
	{
		base::create_gui();
		base::add_member_control(this, "alpha", alpha, "value_slider", "min=0;max=1;ticks=true");
		base::add_member_control(this, "fM", fM, "value_slider", "min=1;max=100;ticks=true;log=true");
	}
};

scene_factory_registration<marschner_lobb<double> >sfr_marschner("marschner_lobb");
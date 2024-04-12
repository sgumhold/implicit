#include "implicit_primitive.h"

template <typename T>
struct quadric : public implicit_primitive<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_primitive<T> base;
		using typename base::pnt_type;
		using base::gui_color;
		using base::add_member_control;


	T a_xx, a_xy, a_xz, a_xw,
	        a_yy, a_yz, a_yw,
	              a_zz, a_zw,
	                    a_ww;

	quadric() {
		a_xx = a_yy = a_zz = 1;
		a_xy = a_xz = a_yz = a_xw = a_yw = a_zw = 0;
		a_ww = -1;
		gui_color = 0xFF8888;
	}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("a_xx", a_xx) &&
			rh.reflect_member("a_xy", a_xy) &&
			rh.reflect_member("a_xz", a_xz) &&
			rh.reflect_member("a_xw", a_xw) &&
			rh.reflect_member("a_yy", a_yy) &&
			rh.reflect_member("a_yz", a_yz) &&
			rh.reflect_member("a_yw", a_yw) &&
			rh.reflect_member("a_zz", a_zz) &&
			rh.reflect_member("a_zw", a_zw) &&
			rh.reflect_member("a_ww", a_ww) &&
			implicit_primitive<T>::self_reflect(rh);
	}
	/// overload to return the type name of this object
	std::string get_type_name() const
	{
		return "quadric";
	}

	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		return p(0)*(a_xx*p(0)+a_xy*p(1)+a_xz*p(2)+a_xw) + 
			    p(1)*(a_yy*p(1)+a_yz*p(2)+a_yw) + 
			    p(2)*(a_zz*p(2)+a_zw) + 
				 a_ww;
	}
	void create_gui()
	{
		add_member_control(this, "a_xx", a_xx, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_yy", a_yy, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_zz", a_zz, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_ww", a_ww, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_xw", a_xw, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_yw", a_yw, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_zw", a_zw, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_xy", a_xy, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_xz", a_xz, "value_slider", "min=-5;max=5;ticks=true;log=true");
		add_member_control(this, "a_yz", a_yz, "value_slider", "min=-5;max=5;ticks=true;log=true");
	}
};

scene_factory_registration<quadric<double> >sfr_quadric("quadric;Q");

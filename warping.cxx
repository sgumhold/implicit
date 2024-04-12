#define _USE_MATH_DEFINES
#include <math.h>
#include "implicit_group.h"

template <typename T>
class taper_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::pnt_type;
		using typename base::crd_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

protected:
	unsigned taper_coordinate  = 2;
	crd_type r0 = 1, r1 = 2, l = 2;
public:
	taper_node() { gui_color = 0xff8000; }
	pnt_type warp(const pnt_type& q) const {
		unsigned i = (taper_coordinate + 1) % 3, j = (taper_coordinate + 2) % 3;
		crd_type lambda = 0;
		if (q[taper_coordinate] > -l) {
			if (q[taper_coordinate] > l)
				lambda = 1;
			else {
				lambda = (q[taper_coordinate] + l) / (2 * l);
			}
		}
		crd_type r = (1 - lambda) * r0 + lambda * r1;
		pnt_type p;
		p[taper_coordinate] = q[taper_coordinate];
		p[i] = q[i] / r;
		p[j] = q[j] / r;
		return p;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		pnt_type q = warp(p);
		return get_implicit_child(0)->evaluate(q);
	}
	std::string get_type_name() const
	{
		return "taper_node";
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		return base::self_reflect(rh) &&
			rh.reflect_member("taper_coordinate", taper_coordinate)&&
			rh.reflect_member("l", l)&&
			rh.reflect_member("r0", r0)&&
			rh.reflect_member("r1", r1);
	}
	/// simple gui to adjust blending width
	void create_gui()
	{
		add_member_control(this, "taper_coordinate", taper_coordinate, "value_slider", "min=0;max=2;ticks=true");
		add_member_control(this, "l", l, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		add_member_control(this, "r0", r0, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		add_member_control(this, "r1", r1, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		base::create_gui();
	}
};

template <typename T>
class twist_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::pnt_type;
		using typename base::crd_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

protected:
	unsigned twist_coordinate = 2;
	crd_type l = 1, theta = 180;
public:
	twist_node() { gui_color = 0xff8000; }
	pnt_type warp(const pnt_type& q) const {
		unsigned i = (twist_coordinate + 1) % 3, j = (twist_coordinate + 2) % 3;
		crd_type lambda = 0;
		if (q[twist_coordinate] > -l) {
			if (q[twist_coordinate] > l)
				lambda = 1;
			else {
				lambda = (q[twist_coordinate] + l) / (2 * l);
			}
		}
		crd_type alpha = lambda * theta * M_PI / 180;
		crd_type ca = cos(alpha), sa = sin(alpha);
		pnt_type p;
		p[twist_coordinate] = q[twist_coordinate];
		p[i] = ca * q[i] + sa * q[j];
		p[j] = -sa * q[i] + ca * q[j];
		return p;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		pnt_type q = warp(p);
		return get_implicit_child(0)->evaluate(q);
	}
	std::string get_type_name() const
	{
		return "twist_node";
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		return base::self_reflect(rh) &&
			rh.reflect_member("twist_coordinate", twist_coordinate)&&
			rh.reflect_member("theta", theta)&&
			rh.reflect_member("l", l);
	}
	/// simple gui to adjust blending width
	void create_gui()
	{
		add_member_control(this, "twist_coordinate", twist_coordinate, "value_slider", "min=0;max=2;ticks=true");
		add_member_control(this, "theta", theta, "value_slider", "min=-360;max=360;ticks=true");
		add_member_control(this, "l", l, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		base::create_gui();
	}
};

template <typename T>
class bend_node : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::pnt_type;
		using typename base::crd_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

protected:
	crd_type l = 1, bend = 45;
	unsigned preserved_coordinate = 2;
public:
	bend_node() { 
		gui_color = 0xff8000;
	}
	pnt_type warp(const pnt_type& q) const {
		if (abs(bend) < 0.1)
			return q;
		unsigned i = (preserved_coordinate + 1)%3, j = (preserved_coordinate + 2) % 3;
		crd_type u = q[i], v = q[j], x, y;
		pnt_type p;
		p[preserved_coordinate] = q[preserved_coordinate];
		crd_type beta = bend * M_PI / 180;
		crd_type sign = (bend >= 0 ? 1 : -1);
		crd_type alpha = abs(beta);
		crd_type v1 = v - l / beta;
		crd_type r = sqrt(u * u + v1 * v1);
		crd_type theta = atan2(u, -sign*v1);
		if (theta < -alpha) {
			crd_type phi = theta + alpha;
			x = r * sin(phi) - l;
			y = -sign * r * cos(phi) + l / beta;
		}
		else if (theta > alpha) {
			crd_type phi = theta - alpha;
			x = r * sin(phi) + l;
			y = -sign * r * cos(phi) + l / beta;
		}
		else {
			x = l * theta / alpha;
			y = l / beta - sign * r;
		}
		p[i] = x;
		p[j] = y;
		return p;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		pnt_type q = warp(p);
		return get_implicit_child(0)->evaluate(q);
	}
	std::string get_type_name() const
	{
		return "bend_node";
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		return base::self_reflect(rh) &&
			rh.reflect_member("preserved_coordinate", preserved_coordinate)&&
			rh.reflect_member("bend", bend)&&
			rh.reflect_member("l", l);
	}
	/// simple gui to adjust blending width
	void create_gui()
	{
		add_member_control(this, "preserved_coordinate", preserved_coordinate, "value_slider", "min=0;max=2;ticks=true");
		add_member_control(this, "l", l, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		add_member_control(this, "bend", bend, "value_slider", "min=-90;max=90;log=true;ticks=true");
		base::create_gui();
	}
};

scene_factory_registration<taper_node<double> >sfr_tapper("taper");
scene_factory_registration<twist_node<double> >sfr_twist("twist");
scene_factory_registration<bend_node<double> >sfr_bend("bend");

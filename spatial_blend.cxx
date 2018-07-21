#include "implicit_group.h"

/** implements a spatial blend along the x-axis. Only works for (first) two children.
    The blend returns

	f1(x,y,z)                                 for x <= 0
	(1-x/width)*f1(x,y,z) + x/width*f2(x,y,z) for 0 < x < width
	f2(x,y,z)                                 for x >= width

	gradient is computed to

	grad f1(x,y,z)                            for x <= 0

	[-1/width*f1(x,y,z) + 1/width*f2(x,y,z), 0, 0] +

	                     |grad f1(x,y,z)|
	[1-x/width, x/width]*|              |     for 0 < x < width
	                     |grad f2(x,y,z)|

	grad f2(x,y,z)                            for x >= width

	*/
template <typename T>
class spatial_blend_node : public implicit_group<T>
{
protected:
	// width of blending region
	T width;
public:
	// standard constructor
	spatial_blend_node() : width(1) { gui_color = 0xffff00; }
	/// evaluation checks nr children and x-coordinate
	T evaluate(const pnt_type& p) const {
		// not enough children
		if (get_nr_children() == 0)     return 1;
		if (get_nr_children() == 1) return get_implicit_child(0)->evaluate(p);
		// simple cases
		if (p(0) <= 0)     return get_implicit_child(0)->evaluate(p);
		if (p(0) >= width) return get_implicit_child(1)->evaluate(p);
		// spatial blend case
		return (1.0-p(0)/width)*get_implicit_child(0)->evaluate(p)+
			       (p(0)/width)*get_implicit_child(1)->evaluate(p);
	}
	// evaluate gradient with same 
	vec_type evaluate_gradient(const pnt_type& p) const {
		// not enough children
		if (get_nr_children() == 0)     return vec_type(0,0,0);
		if (get_nr_children() == 1) return get_implicit_child(0)->evaluate_gradient(p);
		// simple cases
		if (p(0) <= 0)     return get_implicit_child(0)->evaluate_gradient(p);
		if (p(0) >= width) return get_implicit_child(1)->evaluate_gradient(p);
		// spatial blend case
		vec_type m_p( (get_implicit_child(1)->evaluate(p) - get_implicit_child(0)->evaluate(p))/width, 0.0, 0.0);
		cgv::math::fvec<T,2> m_v(1.0-p(0)/width, p(0)/width);
		cgv::math::fmat<T,3,2> F_p;
		F_p.set_col(0, get_implicit_child(0)->evaluate_gradient(p));
		F_p.set_col(1, get_implicit_child(1)->evaluate_gradient(p));
		return m_p + F_p*m_v;
	}
	/// description of members
	bool self_reflect(cgv::reflect::reflection_handler& rh) { 
		return implicit_group<T>::self_reflect(rh) &&
			rh.reflect_member("width", width); 
	}
	/// simple gui to adjust blending width
	void create_gui()
	{
		add_member_control(this, "width", width, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		implicit_group<T>::create_gui();
	}
	/// return type name
	std::string get_type_name() const
	{
		return "spatial_blend_node";
	}	
};

scene_factory_registration<spatial_blend_node<double> >sfr_spatial_blend("spatial_blend");

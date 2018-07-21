#include "scene.h"
#include "implicit_group.h"

template <typename T>
class blend_node : public implicit_group<T>
{
protected:
	T p;
	std::vector<T> ri;
	bool is_union;
	bool angle_correction;
public:
	blend_node() : p(2), is_union(true), angle_correction(false) {}
	/// append a new child and extract trivariate function
	unsigned int append_child(base_ptr child)
	{
		unsigned int i = implicit_group<T>::append_child(child);
		while (func_children.size() > ri.size())
			ri.push_back(1);
		return i;
	}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		size_t n = ri.size();		
		
		// reflect statically allocated members
		bool do_continue = 
			implicit_group<T>::self_reflect(rh) &&
			rh.reflect_member("n", n) &&
			rh.reflect_member("p", p) &&
			rh.reflect_member("is_union", is_union) &&
			rh.reflect_member("angle_correction", angle_correction);

		// ensure dynamic allocation of radii
		if (rh.is_creative()) {
			while (ri.size() < n)
				ri.push_back(1);
			while (ri.size() > n)
				ri.pop_back();
		}

		// check whether to termine self reflection
		if (!do_continue)
			return false;

		// reflect all radii
		for (size_t i = 0; i<ri.size(); ++i)
			if (!rh.reflect_member(std::string("r") + cgv::utils::to_string(i), ri[i]))
				return false;
		
		return true;
	}
	///
	T evaluate(const pnt_type& q) const {
		if (func_children.empty())
			return 1;
		if (func_children.size() == 1)
			return func_children[0]->evaluate(q);
		
		T fac = 1;
		bool use_opt = false;
		if (angle_correction && func_children.size() == 2) {
			T c = abs(dot(normalize(func_children[0]->evaluate_gradient(q)),
				           normalize(func_children[1]->evaluate_gradient(q))));
			fac = (1-c);
			if (fac < 1e-10) {
				fac = 1;
				use_opt = true;
			}
		}
		T f = 0;
		if (is_union) {
			T d_min = 1e12;
			for (unsigned int i=0; i<func_children.size(); ++i) {
				T di = func_children[i]->evaluate(q)/(fac*ri[i]);
				if (di < d_min)
					d_min = di;
				if (di < 1)
					f += pow(1-di,p);
			}
			f = 1-pow(f,1/p);
			if (use_opt || f >= 1-1e-8)
				f = d_min;
		}
		else {
			T d_max = -1e12;
			for (unsigned int i=0; i<func_children.size(); ++i) {
				T di = func_children[i]->evaluate(q)/(fac*ri[i]);
				if (di > d_max)
					d_max = di;
				if (di > -1)
					f += pow(1+di,p);
			}
			f = pow(f,1/p)-1;
			if (use_opt || f <= -1+1e-8)
				f = d_max;
		}
		return f;
	}
	void create_gui()
	{
		add_view("blend", name)->set("color", 0xffff00);
		add_member_control(this, "is_union", is_union, "check");
		add_member_control(this, "angle_correction", angle_correction, "check");
		add_member_control(this, "p", p, "value_slider", "min=1;max=10;log=true;ticks=true");
		for (unsigned int i=0; i<ri.size(); ++i)
			add_member_control(this, std::string("r")+cgv::utils::to_string(i), ri[i], "value_slider", "min=0;max=10;ticks=true;log=true");

		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "blend_node";
	}	
};

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
	spatial_blend_node() : width(1) {}
	/// evaluation checks nr children and x-coordinate
	T evaluate(const pnt_type& p) const {
		// not enough children
		if (func_children.empty())     return 1;
		if (func_children.size() == 1) return func_children[0]->evaluate(p);
		// simple cases
		if (p(0) <= 0)     return func_children[0]->evaluate(p);
		if (p(0) >= width) return func_children[1]->evaluate(p);
		// spatial blend case
		return (1.0-p(0)/width)*func_children[0]->evaluate(p)+
			       (p(0)/width)*func_children[1]->evaluate(p);
	}
	// evaluate gradient with same 
	vec_type evaluate_gradient(const pnt_type& p) const {
		// not enough children
		if (func_children.empty())     return vec_type(0,0,0);
		if (func_children.size() == 1) return func_children[0]->evaluate_gradient(p);
		// simple cases
		if (p(0) <= 0)     return func_children[0]->evaluate_gradient(p);
		if (p(0) >= width) return func_children[1]->evaluate_gradient(p);
		// spatial blend case
		vec_type m_p( (func_children[1]->evaluate(p) - func_children[0]->evaluate(p))/width, 0.0, 0.0);
		vec_type m_v(1.0-p(0)/width, p(0)/width);
		cgv::math::mat<T> F_p(3,2);
		F_p.set_col(0, func_children[0]->evaluate_gradient(p));
		F_p.set_col(1, func_children[1]->evaluate_gradient(p));
		return m_p + F_p*m_v;
	}
	/// description of members
	bool self_reflect(cgv::reflect::reflection_handler& rh) { 
		return implicit_group<T>::self_reflect(rh) &&
			rh.reflect_member("width", width); 
	}
	///
	void on_set(void* member_ptr) {
		update_scene();
	}
	/// simple gui to adjust blending width
	void create_gui()
	{
		add_view("spatial blend", name)->set("color", 0xffff00);
		add_member_control(this, "width", width, "value_slider", "min=0.1;max=10;log=true;ticks=true");
		implicit_group<T>::create_gui();
	}
	/// return type name
	std::string get_type_name() const
	{
		return "spatial_blend_node";
	}	
};

scene_factory_registration<blend_node<double> >sfr_blend("blend");
scene_factory_registration<spatial_blend_node<double> >sfr_spatial_blend("spatial_blend");

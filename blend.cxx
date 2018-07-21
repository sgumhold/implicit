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
		while (get_nr_children() > ri.size())
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
		if (get_nr_children() == 0)
			return 1;
		if (get_nr_children() == 1)
			return get_implicit_child(0)->evaluate(q);
		
		T fac = 1;
		bool use_opt = false;
		if (angle_correction && get_nr_children() == 2) {
			T c = abs(dot(normalize(get_implicit_child(0)->evaluate_gradient(q)),
				           normalize(get_implicit_child(1)->evaluate_gradient(q))));
			fac = (1-c);
			if (fac < 1e-10) {
				fac = 1;
				use_opt = true;
			}
		}
		T f = 0;
		if (is_union) {
			T d_min = 1e12;
			for (unsigned int i=0; i<get_nr_children(); ++i) {
				T di = get_implicit_child(i)->evaluate(q)/(fac*ri[i]);
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
			for (unsigned int i=0; i<get_nr_children(); ++i) {
				T di = get_implicit_child(i)->evaluate(q)/(fac*ri[i]);
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
	clr_type compose_color(const pnt_type& p) const
	{
		return implicit_group<T>::compose_color(p);
	}
	void create_gui()
	{
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

scene_factory_registration<blend_node<double> >sfr_blend("blend");


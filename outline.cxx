#include "implicit_group.h"

/// level set value manipulator
template <typename T>
class outline : public implicit_group<T>
{
protected:
	/// store the outline
	T o;
public:
	/// return type name
	std::string get_type_name() const { return "outline"; }
	/// initialize level to 0
	outline() : o(0.1) { gui_color = 0x00FFFF; }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh) { 
		return rh.reflect_member("o", o) &&
			implicit_group<T>::self_reflect(rh);
	}
	///
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		T f = get_implicit_child(0)->evaluate(p);
		return fabs(f) - o;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		T f = get_implicit_child(0)->evaluate(p);
		return (f < 0 ? T(-1) : T(1))*get_implicit_child(0)->evaluate_gradient(p);
	}
	void create_gui()
	{
		add_member_control(this, "outline", o, "value_slider", "min=0;max=10;ticks=true;log=true");
		implicit_group<T>::create_gui();
	}
};

scene_factory_registration<outline<double> >sfr_offset("outline");
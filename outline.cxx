#include "scene.h"
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
	outline() : o(0.1) {}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh) { 
		return rh.reflect_member("o", o);
	}
	/// ensure that scene is updated if parameters change
	void on_set(void* member_ptr) {
		update_scene();
	}
	///
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		T f = func_children[0]->evaluate(p);
		return f*f - o*o;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		T f = func_children[0]->evaluate(p);
		return 2*f*func_children[0]->evaluate_gradient(p);
	}
	void create_gui()
	{
		add_view("outline",name)->set("color",0x00FFFF);
		add_member_control(this, "outline", o, "value_slider", "min=0;max=10;ticks=true;log=true");
		implicit_group<T>::create_gui();
	}
};

scene_factory_registration<outline<double> >sfr_offset("o;outline");
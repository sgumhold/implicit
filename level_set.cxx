#include "implicit_group.h"

using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::signal;
using namespace cgv::type;

/// level set value manipulator
template <typename T>
class level_set : public implicit_group<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::find_control;
		using base::add_member_control;

protected:
	/// store the level
	T l;
	/// store the range in which to adjust the level
	T r;
public:
	/// return type name
	std::string get_type_name() const { return "level_set"; }
	/// initialize level to 0
	level_set() : l(0), r(1) { gui_color = 0x00FFFF; }
	///
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return rh.reflect_member("l", l) && rh.reflect_member("r", r) &&
			implicit_group<T>::self_reflect(rh);
	}
	///
	void on_set(void* member_ptr)
	{
		if (member_ptr == &r) {
			if (find_control(l)) {
				find_control(l)->set("min", -r);
				find_control(l)->set("max", r);
			}
			base::update_description();
			return;
		}
		implicit_group<T>::on_set(member_ptr);
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		return get_implicit_child(0)->evaluate(p) - l;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		return get_implicit_child(0)->evaluate_gradient(p);
	}
	void create_gui()
	{
		add_member_control(this, "level", l, "value_slider", "min=-1;max=1;step=0.00001;ticks=true");
		add_member_control(this, "range", r, "value_slider", "min=0.001;max=10;step=0.001;ticks=true;log=true");
		implicit_group<T>::create_gui();
	}
};

scene_factory_registration<level_set<double> >sfr_level_set("level_set");

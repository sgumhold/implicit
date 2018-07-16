#include <cgv/signal/rebind.h>
#include <cgv/type/variant.h>
#include "implicit_group.h"
#include "scene.h"

using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::signal;
using namespace cgv::type;

/// level set value manipulator
template <typename T>
class level_set : public implicit_group<T>
{
protected:
	/// store the level
	T l;
	/// store the range in which to adjust the level
	T r;
public:
	/// return type name
	std::string get_type_name() const { return "level_set"; }
	/// initialize level to 0
	level_set() : l(0), r(1) {}
	/// semicolon separated list of property declarations
	std::string get_property_declarations() { return "l:flt64;r:fltk64"; }
	/// abstract interface for the setter of a dynamic property, by default it simply returns false
	bool set_void(const std::string& property, const std::string& value_type, const void* value_ptr) {
		if (property == "l") {
			l = variant<flt64_type>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		if (property == "r") {
			r = variant<flt64_type>::get(value_type,value_ptr);
			update_range();
			return true;
		}
		return false;
	}
	/// abstract interface for the getter of a dynamic property, by default it simply returns false
	bool get_void(const std::string& property, const std::string& value_type, void* value_ptr) {
		if (property == "l") {
			set_variant(l,value_type,value_ptr);
			return true;
		}
		if (property == "r") {
			set_variant(r,value_type,value_ptr);
			return true;
		}
		return false;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return func_children[0]->evaluate(p) - l;
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		return func_children[0]->evaluate_gradient(p);
	}
	void update_range()
	{
		find_control(l)->set("min", -r);
		find_control(l)->set("max", r);
		find_control(l)->update();
	}
	void create_gui()
	{
		add_view("level_set",name)->set("color",0x00FFFF);
		add_control("level", l, "value_slider", "min=-1;max=1;step=0.00001;ticks=true");
		add_control("range", r, "value_slider", "min=0.001;max=10;step=0.001;ticks=true;log=true");
		connect_copy(find_control(l)->value_change, rebind( 
			static_cast<scene_updater*>(this), 
			&scene_updater::update_scene));
		connect_copy(find_control(l)->value_change, rebind( 
			this, &level_set<T>::update_range));
		implicit_group<T>::create_gui();
	}
};

scene_factory_registration<level_set<double> >sfr_level_set('l');
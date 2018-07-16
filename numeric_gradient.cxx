#include <cgv/signal/rebind.h>
#include <cgv/type/variant.h>
#include "implicit_group.h"
#include "scene.h"

using namespace cgv::math;
using namespace cgv::signal;
using namespace cgv::type;

/// level set value manipulator
template <typename T>
class numeric_gradient : public implicit_group<T>
{
protected:
	/// store the numeric_gradient
	T epsilon;
	/// whether to compute numerically the gradient
	bool numerical;
public:
	/// return type name
	std::string get_type_name() const { return "numeric_gradient"; }
	/// initialize level to 0
	numeric_gradient() : epsilon(1e-6), numerical(false) {}
	/// semicolon separated list of property declarations
	std::string get_property_declarations() { return "epsilon:flt64;numerical:bool"; }
	/// abstract interface for the setter of a dynamic property, by default it simply returns false
	bool set_void(const std::string& property, const std::string& value_type, const void* value_ptr) {
		if (property == "epsilon") {
			epsilon = variant<flt64_type>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		if (property == "numerical") {
			numerical = variant<bool>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		return false;
	}
	/// abstract interface for the getter of a dynamic property, by default it simply returns false
	bool get_void(const std::string& property, const std::string& value_type, void* value_ptr) {
		if (property == "epsilon") {
			set_variant(epsilon,value_type,value_ptr);
			return true;
		}
		if (property == "numerical") {
			set_variant(numerical,value_type,value_ptr);
			return true;
		}
		return false;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return func_children[0]->evaluate(p);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		if (numerical) {
			T inv_2_eps = (T)(0.5/epsilon);
			return vec_type(
				inv_2_eps*(evaluate(pnt_type(p(0)+epsilon,p(1),p(2))) - 
							  evaluate(pnt_type(p(0)-epsilon,p(1),p(2)))),
				inv_2_eps*(evaluate(pnt_type(p(0),p(1)+epsilon,p(2))) - 
							  evaluate(pnt_type(p(0),p(1)-epsilon,p(2)))),
				inv_2_eps*(evaluate(pnt_type(p(0),p(1),p(2)+epsilon)) - 
							  evaluate(pnt_type(p(0),p(1),p(2)-epsilon))));
		}
		return func_children[0]->evaluate_gradient(p);
	}
	void create_gui()
	{
		add_view("numeric_gradient",name)->set("color",0x00FFFF);
		add_control("epsilon", epsilon, "value_slider", "min=0.000000001;max=0.1;step=0.000000001;ticks=true;log=true");
		add_control("numerical", numerical, "check");
		connect_copy(find_control(epsilon)->value_change, rebind( 
			static_cast<scene_updater*>(this), 
			&scene_updater::update_scene));
		connect_copy(find_control(numerical)->value_change, rebind( 
			static_cast<scene_updater*>(this), 
			&scene_updater::update_scene));
		implicit_group<T>::create_gui();
	}
};

scene_factory_registration<numeric_gradient<double> >sfr_numeric_gradient('g');
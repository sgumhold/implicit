#include "implicit_group.h"
#include <cgv/signal/rebind.h>
#include <cgv_gl/gl/gl.h>
#include <cgv/type/variant.h>
#include "scene.h"

using namespace cgv::type;
using namespace cgv::signal;

template <typename T>
struct rotation : public implicit_group<T>
{
	fvec_type axis;
	double   angle;

	rotation() : axis(1,0,0), angle(90) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("a", angle) &&
			rh.reflect_member("nx", axis(0)) &&
			rh.reflect_member("ny", axis(1)) &&
			rh.reflect_member("nz", axis(2));
	}
	vec_type rotate(const vec_type& p, double ang) const
	{
		vec_type axis = this->axis.to_vec();
		vec_type a = dot(p,axis)*axis;
		vec_type x = p-a;
		vec_type y = cross(x,axis);
		return a+cos(ang)*x+sin(ang)*y;
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return func_children[0]->evaluate(rotate(p,angle*(-.1745329252e-1)));
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		T ang = angle*.1745329252e-1;
		return rotate(func_children[0]->evaluate_gradient(rotate(p,-ang)),ang);
	}

	void update_axis(unsigned int i)
	{
		unsigned int j = (i+1)%3;
		unsigned int k = (j+1)%3;
		T cc = axis(j)*axis(j)+axis(k)*axis(k);
		T dd = 1-axis(i)*axis(i);
		if (cc > 2e-8f) {
			T f = sqrt(dd/cc);
			axis(j) *= f;
			axis(k) *= f;
		}
		else {
			cc = 2e-8f;
			T f = sqrt(dd/cc);
			axis(j) = 1e-4f*f;
			axis(k) = 1e-4f*f;
		}
		update_member(&axis(j));
		update_member(&axis(k));
		update_scene();
	}
	void create_gui()
	{
		add_view("rotation",name)->set("color",0x88FF88);
		add_control("a", angle, "value_slider", "min=-180;max=180;ticks=true");
		add_control("nx", axis(0), "value_slider", "min=-1;max=1;ticks=true");
		add_control("ny", axis(1), "value_slider", "min=-1;max=1;ticks=true");
		add_control("nz", axis(2), "value_slider", "min=-1;max=1;ticks=true");

		connect_copy(find_control(angle)->value_change,
			rebind(static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(axis(0))->value_change,
			rebind(this, &rotation<T>::update_axis, 0));
		connect_copy(find_control(axis(1))->value_change,
			rebind(this, &rotation<T>::update_axis, 1));
		connect_copy(find_control(axis(2))->value_change,
			rebind(this, &rotation<T>::update_axis, 2));
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glRotated(angle,axis(0),axis(1),axis(2));
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
	std::string get_type_name() const
	{
		return "rotation";
	}	
};

template <typename T>
struct translation : public implicit_group<T>
{
	fvec_type delta;

	translation() : delta(1,0,0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("dx", delta(0)) &&
			rh.reflect_member("dy", delta(1)) &&
			rh.reflect_member("dz", delta(2));
	}
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return func_children[0]->evaluate(p-delta.to_vec());
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		return func_children[0]->evaluate_gradient(p-delta.to_vec());
	}

	void create_gui()
	{
		add_view("translation",name)->set("color",0x88FF88);
		add_control("dx", delta(0), "value_slider", "min=-3;max=3;ticks=true");
		add_control("dy", delta(1), "value_slider", "min=-3;max=3;ticks=true");
		add_control("dz", delta(2), "value_slider", "min=-3;max=3;ticks=true");

		connect_copy(find_control(delta(0))->value_change,
			rebind(static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(delta(1))->value_change,
			rebind(static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(delta(2))->value_change,
			rebind(static_cast<scene_updater*>(this), &scene_updater::update_scene));
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glTranslated(delta(0),delta(1),delta(2));
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
	std::string get_type_name() const
	{
		return "translation";
	}	
};

template <typename T>
struct scaling : public implicit_group<T>
{
	fvec_type scale;
	fvec_type inv_scale;

	scaling() : scale(1,1,1), inv_scale(1,1,1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("sx", scale(0)) &&
			rh.reflect_member("sy", scale(1)) &&
			rh.reflect_member("sz", scale(2));
	}
	
	void on_set(void*)
	{
		update_helpers();
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		pnt_type q(p(0)*inv_scale(0),p(1)*inv_scale(1),p(2)*inv_scale(2));
		return func_children[0]->evaluate(q);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		pnt_type q(p(0)*inv_scale(0),p(1)*inv_scale(1),p(2)*inv_scale(2));
		vec_type g = func_children[0]->evaluate_gradient(q);
		return vec_type(g(0)*inv_scale(0),g(1)*inv_scale(1),g(2)*inv_scale(2));
	}
	void update_helpers()
	{
		inv_scale(0) = 1/scale(0);
		inv_scale(1) = 1/scale(1);
		inv_scale(2) = 1/scale(2);
		update_scene();
	}
	void create_gui()
	{
		add_view("scaling",name)->set("color",0x88FF88);
		add_control("sx", scale(0), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_control("sy", scale(1), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_control("sz", scale(2), "value_slider", "min=0;max=3;ticks=true;log=true");

		connect_copy(find_control(scale(0))->value_change,
			rebind(this, &scaling<T>::update_helpers));
		connect_copy(find_control(scale(1))->value_change,
			rebind(this, &scaling<T>::update_helpers));
		connect_copy(find_control(scale(2))->value_change,
			rebind(this, &scaling<T>::update_helpers));
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glScaled(scale(0),scale(1),scale(2));
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
	std::string get_type_name() const
	{
		return "scaling";
	}	
};


template <typename T>
struct uniform_scaling : public implicit_group<T>
{
	double scale;
	double inv_scale;

	uniform_scaling() : scale(1), inv_scale(1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return rh.reflect_member("s", scale);
	}
	void on_set(void*)
	{
		update_helpers();
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		return func_children[0]->evaluate(inv_scale*p);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		return inv_scale*func_children[0]->evaluate_gradient(inv_scale*p);
	}
	void update_helpers()
	{
		inv_scale = 1/scale;
		update_scene();
	}
	void create_gui()
	{
		add_view("uniform_scaling",name)->set("color",0x88FF88);
		add_control("s", scale, "value_slider", "min=0;max=3;ticks=true;log=true");

		connect_copy(find_control(scale)->value_change,
			rebind(this, &uniform_scaling<T>::update_helpers));
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glScaled(scale,scale,scale);
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
	std::string get_type_name() const
	{
		return "uniform_scaling";
	}	
};


template <typename T>
struct shear : public implicit_group<T>
{
	double h_xy, h_xz, h_yz;

	shear() : h_xy(0), h_xz(0), h_yz(0) {}

	/// semicolon separated list of property declarations
	std::string get_property_declarations() { return "hxy:flt64;hxz:flt64;hyz:flt64"; }
	/// abstract interface for the setter of a dynamic property, by default it simply returns false
	bool set_void(const std::string& property, const std::string& value_type, const void* value_ptr) {
		if (property == "hxy") {
			h_xy = variant<flt64_type>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		else if (property == "hxz") {
			h_xz = variant<flt64_type>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		else if (property == "hyz") {
			h_yz = variant<flt64_type>::get(value_type,value_ptr);
			update_scene();
			return true;
		}
		return false;
	}
	/// abstract interface for the getter of a dynamic property, by default it simply returns false
	bool get_void(const std::string& property, const std::string& value_type, void* value_ptr) {
		if (property == "hxy") {
			set_variant(h_xy,value_type,value_ptr);
			return true;
		}
		if (property == "hxz") {
			set_variant(h_xz,value_type,value_ptr);
			return true;
		}
		if (property == "hyz") {
			set_variant(h_yz,value_type,value_ptr);
			return true;
		}
		return false;
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (func_children.empty())
			return 1;
		pnt_type q(p(0)-h_xy*p(1)-h_xz*p(2),p(1)-h_yz*p(2), p(2));
		return func_children[0]->evaluate(q);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (func_children.empty())
			return vec_type(0,0,0);
		pnt_type q(p(0)-h_xy*p(1)-h_xz*p(2),p(1)-h_yz*p(2), p(2));
		vec_type g = func_children[0]->evaluate_gradient(q);
		return vec_type(g(0),g(1)-h_xy*g(0),g(2)-h_yz*g(1)-h_xz*g(0));
	}
	void create_gui()
	{
		add_view("shear",name)->set("color",0x88FF88);
		add_control("hxy", h_xy, "value_slider", "min=-3;max=3;ticks=true");
		add_control("hxz", h_xz, "value_slider", "min=-3;max=3;ticks=true");
		add_control("hyz", h_yz, "value_slider", "min=-3;max=3;ticks=true");

		connect_copy(find_control(h_xy)->value_change, rebind(
			static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(h_xz)->value_change, rebind(
			static_cast<scene_updater*>(this), &scene_updater::update_scene));
		connect_copy(find_control(h_yz)->value_change, rebind(
			static_cast<scene_updater*>(this), &scene_updater::update_scene));
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		GLdouble M[16] = {
			1, h_xy, h_xz, 0,
			0,    1, h_yz, 0,
			0,    1,    1, 0,
			0,    0,    0, 1
		};
		glMultMatrixd(M);
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
	std::string get_type_name() const
	{
		return "shear";
	}	
};

scene_factory_registration<rotation<double> >sfr_rotation('r');
scene_factory_registration<translation<double> >sfr_translation('t');
scene_factory_registration<scaling<double> >sfr_scaling('s');
scene_factory_registration<shear<double> >sfr_shear('h');
scene_factory_registration<uniform_scaling<double> >sfr_uniform_scaling('u');
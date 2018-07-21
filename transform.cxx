#include "implicit_group.h"
#include <cgv/signal/rebind.h>
#include <cgv_gl/gl/gl.h>
#include <cgv/type/variant.h>
#include "scene.h"

using namespace cgv::type;
using namespace cgv::signal;

template <typename T>
struct transformation : public implicit_group<T>
{
	bool show_axes;

	transformation() : show_axes(false) {}

	void on_set(void* member_ptr)
	{
		if (member_ptr == &show_axes) {
			update_description();
			post_redraw();
		}
		else
			implicit_group<T>::on_set(member_ptr);
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			implicit_group<T>::self_reflect(rh) &&
			rh.reflect_member("show_axes", show_axes);
	}
	void create_gui()
	{
		add_member_control(this, "show_axes", show_axes, "check");
		implicit_group<T>::create_gui();
	}
	void draw(context& ctx)
	{
		if (show_axes) {
			glPushMatrix();
			glEnable(GL_COLOR_MATERIAL);
			ctx.enable_material();
			glColor3f(0, 0, 1);
			ctx.tesselate_arrow(1.2, 0.025);
			glRotated(90, 0, 1, 0);
			glColor3f(1, 0, 0);
			ctx.tesselate_arrow(1.2, 0.025);
			glRotated(-90, 1, 0, 0);
			glColor3f(0, 1, 0);
			ctx.tesselate_arrow(1.2, 0.025);
			ctx.disable_material();
			glPopMatrix();			
		}
	}
	void finish_draw(context& ctx)
	{
		glPopMatrix();
	}
};


template <typename T>
struct rotation : public transformation<T>
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
			rh.reflect_member("nz", axis(2)) &&
			transformation<T>::self_reflect(rh);
	}
	vec_type rotate(const vec_type& p, double ang) const
	{
		vec_type axis = this->axis.to_vec();
		vec_type a = dot(p,axis)*axis;
		vec_type x = p-a;
		vec_type y = cross(axis,x);
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

	void create_gui()
	{
		add_view("rotation",name)->set("color",0x88FF88);
		add_member_control(this, "a", angle, "value_slider", "min=-180;max=180;ticks=true");
		add_gui("axis", axis, "direction", "options='min=-1;max=1;ticks=true'");
		transformation<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glRotated(angle, axis(0), axis(1), axis(2));
		transformation<T>::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "rotation";
	}	
};

template <typename T>
struct translation : public transformation<T>
{
	fvec_type delta;

	translation() : delta(1,0,0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("dx", delta(0)) &&
			rh.reflect_member("dy", delta(1)) &&
			rh.reflect_member("dz", delta(2)) &&
			transformation<T>::self_reflect(rh);
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
		add_member_control(this, "dx", delta(0), "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "dy", delta(1), "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "dz", delta(2), "value_slider", "min=-3;max=3;ticks=true");
		transformation<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glTranslated(delta(0),delta(1),delta(2));
		transformation<T>::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "translation";
	}	
};

template <typename T>
struct scaling : public transformation<T>
{
	fvec_type scale;
	fvec_type inv_scale;

	scaling() : scale(1,1,1), inv_scale(1,1,1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("sx", scale(0)) &&
			rh.reflect_member("sy", scale(1)) &&
			rh.reflect_member("sz", scale(2)) &&
			transformation<T>::self_reflect(rh);
	}
	
	void on_set(void* member_ptr)
	{		
		for (int i = 0; i < 3; ++i) {
			if (member_ptr == &scale(i)) {
				inv_scale(i) = 1 / scale(i);
				update_scene();
				return;
			}
		}
		transformation<T>::on_set(member_ptr);
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
	void create_gui()
	{
		add_view("scaling",name)->set("color",0x88FF88);
		add_member_control(this, "sx", scale(0), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_member_control(this, "sy", scale(1), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_member_control(this, "sz", scale(2), "value_slider", "min=0;max=3;ticks=true;log=true");
		transformation<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glScaled(scale(0),scale(1),scale(2));
		transformation<T>::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "scaling";
	}	
};


template <typename T>
struct uniform_scaling : public transformation<T>
{
	double scale;
	double inv_scale;

	uniform_scaling() : scale(1), inv_scale(1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return rh.reflect_member("s", scale) &&
			transformation<T>::self_reflect(rh);
	}
	void on_set(void* member_ptr)
	{
		if (member_ptr == &scale) {
			inv_scale = 1 / scale;
			update_scene();
			return;
		}
		transformation<T>::on_set(member_ptr);
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
	void create_gui()
	{
		add_view("uniform_scaling",name)->set("color",0x88FF88);
		add_member_control(this, "s", scale, "value_slider", "min=0;max=3;ticks=true;log=true");
		transformation<T>::create_gui();
	}
	void draw(context& ctx)
	{
		glPushMatrix();
		glScaled(scale,scale,scale);
		transformation<T>::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "uniform_scaling";
	}	
};


template <typename T>
struct shear : public transformation<T>
{
	double h_xy, h_xz, h_yz;

	shear() : h_xy(0), h_xz(0), h_yz(0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return 
			rh.reflect_member("h_xy", h_xy) &&
			rh.reflect_member("h_xz", h_xz) &&
			rh.reflect_member("h_yz", h_yz) &&
			transformation<T>::self_reflect(rh);
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
		add_member_control(this, "hxy", h_xy, "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "hxz", h_xz, "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "hyz", h_yz, "value_slider", "min=-3;max=3;ticks=true");
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
		transformation<T>::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "shear";
	}	
};

scene_factory_registration<rotation<double> >sfr_rotation("rotate;r");
scene_factory_registration<translation<double> >sfr_translation("translate;t");
scene_factory_registration<scaling<double> >sfr_scaling("scale;s");
scene_factory_registration<shear<double> >sfr_shear("shear");
scene_factory_registration<uniform_scaling<double> >sfr_uniform_scaling("scale_uniform;u");
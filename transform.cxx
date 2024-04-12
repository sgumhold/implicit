#include "implicit_group.h"

#include <cgv_gl/gl/gl.h>
#include <cgv/math/ftransform.h>
#include <cgv_gl/arrow_renderer.h>

template <typename T>
struct transformation : public implicit_group<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef implicit_group<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::add_member_control;

	bool show_axes;
	
	transformation() : show_axes(false) { gui_color = 0x88FF88; }

	void on_set(void* member_ptr)
	{
		if (member_ptr == &show_axes) {
			base::update_member(member_ptr);
			base::update_description();
			base::post_redraw();
		}
		else
			base::on_set(member_ptr);
	}
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			base::self_reflect(rh) &&
			rh.reflect_member("show_axes", show_axes);
	}
	void create_gui()
	{
		add_member_control(this, "show_axes", show_axes, "check");
		base::create_gui();
	}
	bool init(context& ctx)
	{
		ref_arrow_renderer(ctx, 1);
		return true;
	}
	void clear(context& ctx)
	{
		ref_arrow_renderer(ctx, -1);
	}

	void draw(context& ctx)
	{
		if (show_axes) {
			static cgv::mat3 I; I.identity();
			static cgv::mat3 Z; Z.zeros();
			static arrow_render_style ars;
			ars.radius_relative_to_length = 0.025f;
			auto& ar = cgv::render::ref_arrow_renderer(ctx);
			ar.set_render_style(ars);
			ar.set_position_array(ctx, &Z.col(0), 3);
			ar.set_direction_array(ctx, &I.col(0), 3);
			ar.set_color_array(ctx, reinterpret_cast<const cgv::rgb*>(&I.col(0)), 3);
			ar.render(ctx, 0, 3);
			//glPushMatrix();
			//glEnable(GL_COLOR_MATERIAL);
			//ctx.enable_material();
			//glColor3f(0, 0, 1);
			//ctx.tesselate_arrow(1.2, 0.025);
			//glRotated(90, 0, 1, 0);
			//glColor3f(1, 0, 0);
			//ctx.tesselate_arrow(1.2, 0.025);
			//glRotated(-90, 1, 0, 0);
			//glColor3f(0, 1, 0);
			//ctx.tesselate_arrow(1.2, 0.025);
			//ctx.disable_material();
			//glPopMatrix();			
		}
	}
	void finish_draw(context& ctx)
	{
		ctx.pop_modelview_matrix();
		//glPopMatrix();
	}
};


template <typename T>
struct rotation : public transformation<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef transformation<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

	vec_type axis;
	double   angle;

	rotation() : axis(1,0,0), angle(90) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("a", angle) &&
			rh.reflect_member("nx", axis(0)) &&
			rh.reflect_member("ny", axis(1)) &&
			rh.reflect_member("nz", axis(2)) &&
			base::self_reflect(rh);
	}
	vec_type rotate(const vec_type& p, double ang) const
	{
		vec_type axis = this->axis;
		vec_type a = dot(p,axis)*axis;
		vec_type x = p-a;
		vec_type y = cross(axis,x);
		return a+cos(ang)*x+sin(ang)*y;
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		return get_implicit_child(0)->evaluate(rotate(p,angle*(-.1745329252e-1)));
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		T ang = angle*.1745329252e-1;
		return rotate(get_implicit_child(0)->evaluate_gradient(rotate(p,-ang)),ang);
	}

	void create_gui()
	{
		add_member_control(this, "a", angle, "value_slider", "min=-180;max=180;ticks=true");
		base::add_gui("axis", axis, "direction", "options='min=-1;max=1;ticks=true'");
		base::create_gui();
	}
	void draw(context& ctx)
	{
		ctx.push_modelview_matrix();
		ctx.mul_modelview_matrix(cgv::math::rotate4<double>(angle, axis));

	//	glPushMatrix();
	//	glRotated(angle, axis(0), axis(1), axis(2));

		base::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "rotation";
	}	
};

template <typename T>
struct translation : public transformation<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef transformation<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

	vec_type delta;

	translation() : delta(1,0,0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("dx", delta(0)) &&
			rh.reflect_member("dy", delta(1)) &&
			rh.reflect_member("dz", delta(2)) &&
			base::self_reflect(rh);
	}
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		return get_implicit_child(0)->evaluate(p-delta);
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		return get_implicit_child(0)->evaluate_gradient(p-delta);
	}

	void create_gui()
	{
		add_member_control(this, "dx", delta(0), "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "dy", delta(1), "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "dz", delta(2), "value_slider", "min=-3;max=3;ticks=true");
		base::create_gui();
	}
	void draw(context& ctx)
	{
		ctx.push_modelview_matrix();
		ctx.mul_modelview_matrix(cgv::math::translate4<double>(delta));
	//	glPushMatrix();
	//	glTranslated(delta(0),delta(1),delta(2));
		base::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "translation";
	}	
};

template <typename T>
struct scaling : public transformation<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef transformation<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

	vec_type scale;
	vec_type inv_scale;

	scaling() : scale(1,1,1), inv_scale(1,1,1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return
			rh.reflect_member("sx", scale(0)) &&
			rh.reflect_member("sy", scale(1)) &&
			rh.reflect_member("sz", scale(2)) &&
			base::self_reflect(rh);
	}
	
	void on_set(void* member_ptr)
	{		
		for (int i = 0; i < 3; ++i) {
			if (member_ptr == &scale(i))
				inv_scale(i) = 1 / scale(i);
		}
		base::on_set(member_ptr);
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		pnt_type q(p(0)*inv_scale(0),p(1)*inv_scale(1),p(2)*inv_scale(2));
		return get_implicit_child(0)->evaluate(q);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		pnt_type q(p(0)*inv_scale(0),p(1)*inv_scale(1),p(2)*inv_scale(2));
		vec_type g = get_implicit_child(0)->evaluate_gradient(q);
		return vec_type(g(0)*inv_scale(0),g(1)*inv_scale(1),g(2)*inv_scale(2));
	}
	void create_gui()
	{
		add_member_control(this, "sx", scale(0), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_member_control(this, "sy", scale(1), "value_slider", "min=0;max=3;ticks=true;log=true");
		add_member_control(this, "sz", scale(2), "value_slider", "min=0;max=3;ticks=true;log=true");
		base::create_gui();
	}
	void draw(context& ctx)
	{
	//	glPushMatrix();
	//	glScaled(scale(0),scale(1),scale(2));
		ctx.push_modelview_matrix();
		ctx.mul_modelview_matrix(cgv::math::scale4<double>(scale));
		base::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "scaling";
	}	
};


template <typename T>
struct uniform_scaling : public transformation<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef transformation<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

	double scale;
	double inv_scale;

	uniform_scaling() : scale(1), inv_scale(1) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return rh.reflect_member("s", scale) &&
			base::self_reflect(rh);
	}
	void on_set(void* member_ptr)
	{
		if (member_ptr == &scale) {
			inv_scale = 1 / scale;
			base::update_scene();
			return;
		}
		base::on_set(member_ptr);
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		return get_implicit_child(0)->evaluate(inv_scale*p);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		return inv_scale*get_implicit_child(0)->evaluate_gradient(inv_scale*p);
	}
	void create_gui()
	{
		add_member_control(this, "s", scale, "value_slider", "min=0;max=3;ticks=true;log=true");
		base::create_gui();
	}
	void draw(context& ctx)
	{
		//glPushMatrix();
		//glScaled(scale,scale,scale);
		ctx.push_modelview_matrix();
		ctx.mul_modelview_matrix(cgv::math::scale4<double>(scale,scale,scale));
		base::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "uniform_scaling";
	}	
};


template <typename T>
struct shear : public transformation<T>
{
	/// main templated superclass which we want to inherit stuff from
	typedef transformation<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::gui_color;
		using base::get_nr_children;
		using base::get_implicit_child;
		using base::evaluate_gradient;
		using base::add_member_control;

	double h_xy, h_xz, h_yz;

	shear() : h_xy(0), h_xz(0), h_yz(0) {}

	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		return 
			rh.reflect_member("h_xy", h_xy) &&
			rh.reflect_member("h_xz", h_xz) &&
			rh.reflect_member("h_yz", h_yz) &&
			base::self_reflect(rh);
	}
	/// apply inverse transformation to the point before evaluation of child
	T evaluate(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return 1;
		pnt_type q(p(0)-h_xy*p(1)-h_xz*p(2),p(1)-h_yz*p(2), p(2));
		return get_implicit_child(0)->evaluate(q);
	}
	/// 
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (get_nr_children() == 0)
			return vec_type(0,0,0);
		pnt_type q(p(0)-h_xy*p(1)-h_xz*p(2),p(1)-h_yz*p(2), p(2));
		vec_type g = get_implicit_child(0)->evaluate_gradient(q);
		return vec_type(g(0),g(1)-h_xy*g(0),g(2)-h_yz*g(1)-h_xz*g(0));
	}
	void create_gui()
	{
		base::add_view("shear", base::name)->set("color", 0x88FF88);
		add_member_control(this, "hxy", h_xy, "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "hxz", h_xz, "value_slider", "min=-3;max=3;ticks=true");
		add_member_control(this, "hyz", h_yz, "value_slider", "min=-3;max=3;ticks=true");
		base::create_gui();
	}
	void draw(context& ctx)
	{
		//glPushMatrix();
		//GLdouble M[16] = {
		//	1, h_xy, h_xz, 0,
		//	0,    1, h_yz, 0,
		//	0,    1,    1, 0,
		//	0,    0,    0, 1
		//};
		//glMultMatrixd(M);
		ctx.push_modelview_matrix();
		cgv::dmat4 M;
		M.identity();
		M(0, 1) = h_xy;
		M(0, 2) = h_xz;
		M(1, 2) = h_yz;
		ctx.mul_modelview_matrix(M);

		base::draw(ctx);
	}
	std::string get_type_name() const
	{
		return "shear";
	}	
};

scene_factory_registration<rotation<double> >sfr_rotation("rotate;r");
scene_factory_registration<translation<double> >sfr_translation("translate;t");
scene_factory_registration<scaling<double> >sfr_scaling("scale;s");
scene_factory_registration<shear<double> >sfr_shear("shear;h");
scene_factory_registration<uniform_scaling<double> >sfr_uniform_scaling("uniform_scaling;u");

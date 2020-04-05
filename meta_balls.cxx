#include "knot_vector.h"
#include <cgv_gl/gl/gl.h>
#include <cgv_gl/sphere_renderer.h>

template <typename T>
class meta_balls :  public knot_vector<T>
{
protected:
	std::vector<T> ri;
	cgv::media::illum::phong_material mat;
	bool show_spheres;
	T r;
public:
	/// standard constructor
	meta_balls() : r(1) {
		mat.set_diffuse(cgv::media::illum::phong_material::color_type(0.5f, 0.5f, 0.5f));
		mat.set_specular(cgv::media::illum::phong_material::color_type(0.5f, 0.5f, 0.5f));
		mat.set_ambient(cgv::media::illum::phong_material::color_type(0.3f, 0.0f, 0.0f));
		mat.set_shininess(100);
		show_spheres = false;
		gui_color = 0xFF8888;
	}
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh)
	{
		if (!knot_vector<T>::self_reflect(rh))
			return false;
		if (!rh.reflect_member("show_spheres", show_spheres))
			return false;

		// reflect all radii
		for (size_t i = 0; i < ri.size(); ++i)
			if (!rh.reflect_member(std::string("r") + cgv::utils::to_string(i), ri[i]))
				return false;

		return true;
	}
	void on_set(void* member_ptr) {
		if (member_ptr == &r) {
			ri[pnt_idx] = r;
		}
		knot_vector<T>::on_set(member_ptr);
	}
	void append_callback(size_t pi) 
	{
		ri.push_back(1);
	}
	void point_index_selection_callback() {
		r = ri[pnt_idx];
		if (find_control(r))
			find_control(r)->update();
	}

	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const
	{
		T f = 0;
		for (unsigned int i=0; i<points.size(); ++i) {
			f += exp(-(p-points[i]).sqr_length()/(ri[i]*ri[i]));
		}
		return exp(-1.0)-f;
	}
	/// overload to return the type name of this object
	std::string get_type_name() const { 
		return "meta_balls"; 
	}
	bool init(context& ctx)
	{
		ref_sphere_renderer(ctx, 1);
		return true;
	}
	void clear(context& ctx)
	{
		ref_sphere_renderer(ctx, -1);
	}
	/// its a drawable
	void draw(context& ctx)
	{
		if (!show_spheres)
			return;
		static sphere_render_style srs;
		srs.surface_color = rgb(0.5f, 0.5f, 0.5f);
		auto& sr = cgv::render::ref_sphere_renderer(ctx);
		sr.set_render_style(srs);
		sr.set_position_array(ctx, points);
		sr.set_radius_array(ctx, ri);
		sr.render(ctx, 0, points.size());
		/*
		ctx.enable_material(mat);
		for (unsigned int i=0; i<points.size(); ++i) {
			glPushMatrix();
			glTranslated(points[i](0),points[i](1),points[i](2));
			glScaled(ri[i],ri[i],ri[i]);
			ctx.tesselate_unit_sphere();
			glPopMatrix();
		}
		ctx.disable_material(mat);
		*/
	}
	void create_gui()
	{
		knot_vector<T>::create_gui();
		add_member_control(this, "r", r, "value_slider", "min=0;max=5;ticks=true;log=true");
		add_member_control(this, "show_spheres", show_spheres, "toggle");
	}
};

scene_factory_registration<meta_balls<double> > sfr_meta_balls("meta_balls;M");

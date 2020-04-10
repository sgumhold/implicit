#include "knot_vector.h"
#include <cgv_gl/gl/gl.h>
#include <cgv/math/ftransform.h>
#include <cgv_gl/sphere_renderer.h>

static bool meta_balls_srs_initialized = false;
static sphere_render_style meta_balls_srs;

template <typename T>
class meta_balls :  public knot_vector<T>
{
protected:
	std::vector<T> ri;
	bool show_spheres;
	T r;
public:
	/// standard constructor
	meta_balls() : r(1) {
		if (!meta_balls_srs_initialized) {
			meta_balls_srs.material.set_diffuse_reflectance(rgb(0.5f, 0.5f, 0.5f));
			meta_balls_srs.material.set_specular_reflectance(rgb(0.5f, 0.5f, 0.5f));
			meta_balls_srs.material.set_ambient_occlusion(0.3f);
			meta_balls_srs.material.set_roughness(0.2f);
			meta_balls_srs.surface_color = rgb(0.5f, 0.5f, 0.5f);
			meta_balls_srs_initialized = true;
		}
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
		
		auto& sr = cgv::render::ref_sphere_renderer(ctx);
		sr.set_render_style(meta_balls_srs);
		std::vector<vec3> P;
		std::vector<float> R;
		for (size_t i = 0; i < points.size(); ++i) {
			P.push_back(points[i]);
			R.push_back(float(ri[i]));
		}
		sr.set_position_array(ctx, P);
		sr.set_radius_array(ctx, R);
		sr.render(ctx, 0, P.size());
	}
	void create_gui()
	{
		knot_vector<T>::create_gui();
		add_member_control(this, "r", r, "value_slider", "min=0;max=5;ticks=true;log=true");
		bool show = begin_tree_node("spheres", show_spheres, false, "level=3;options='w=130';align=' '");
		add_member_control(this, "show", show_spheres, "toggle", "w=62");
		if (show) {
			align("\a");
			add_gui("render_style", meta_balls_srs);
			align("\b");
			end_tree_node(show_spheres);
		}
	}
};

scene_factory_registration<meta_balls<double> > sfr_meta_balls("meta_balls;M");

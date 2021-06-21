#include "gl_implicit_surface_drawable.h"
#include <cgv/utils/progression.h>
#include <cgv/signal/rebind.h>
#include <cgv/base/group.h>
#include <cgv/gui/file_dialog.h>
#include <cgv/gui/key_event.h>
#include <cgv/gui/mouse_event.h>
#include <cgv/base/register.h>
#include <cgv/utils/file.h>
#include <fstream>

using namespace cgv::gui;
using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::signal;
using namespace cgv::render::gl;
using namespace cgv::media;

gl_implicit_surface_drawable::gl_implicit_surface_drawable(const std::string& name) : cgv::base::node(name)
{
}

std::string gl_implicit_surface_drawable::get_type_name() const
{
	return "implicit surface";
}

void gl_implicit_surface_drawable::toggle_range()
{
	std::swap(map_to_zero_value, map_to_one_value);
	update_member(&map_to_zero_value);
	update_member(&map_to_one_value);
}

void gl_implicit_surface_drawable::adjust_range()
{
	// prepare private members
	dvec3 p = box.get_min_pnt();
	dvec3 d = box.get_extent();
	d(0) /= (res - 1); d(1) /= (res - 1); d(2) /= (res - 1);
	
	// prepare progression
	cgv::utils::progression prog;
	prog.init("adjust range", res, 10);
	
	// iterate through all slices
	bool set = false;
	unsigned int i, j, k;
	for (k = 0; k < res; ++k, p(2) += d(2)) {
		prog.step();
		for (j = 0, p(1) = box.get_min_pnt()(1); j < res; ++j, p(1) += d(1)) {
			for (i = 0, p(0) = box.get_min_pnt()(0); i < res; ++i, p(0) += d(0)) {
				double v = func_ptr->evaluate(p.to_vec());
				if (set) {
					if (v < map_to_zero_value)
						map_to_zero_value = v;
					if (v > map_to_one_value)
						map_to_one_value = v;

				}
				else {
					map_to_zero_value = v;
					map_to_one_value = v;
					set = true;
				}
			}
		}
	}
	update_member(&map_to_zero_value);
	update_member(&map_to_one_value);
}

void gl_implicit_surface_drawable::extract_mesh()
{
	gl_implicit_surface_drawable_base::extract_mesh();
	update_member(&srs.radius_scale);
	update_member(&crs.radius_scale);
}

void gl_implicit_surface_drawable::export_volume()
{
	std::string fn = file_save_dialog("choose vox output file", "Obj Files (vox):*.vox|All Files:*.*");
	if (fn.empty())
		return;
	std::string hd_fn = cgv::utils::file::drop_extension(fn) + ".hd";

	std::ofstream os(hd_fn.c_str());
	if (os.fail())
		return;
	os << "Size:      " << res << ", " << res << ", " << res << std::endl;
	dvec3 scaling = box.get_extent(); // / dvec3(res, res, res);

	os << "Spacing:   " << scaling(0) << ", " << scaling(1) << ", " << scaling(2) << std::endl;
	os.close();

	std::vector<unsigned char> data;
	// prepare private members
	dvec3 p = box.get_min_pnt();
	dvec3 d = box.get_extent();
	d(0) /= (res - 1); d(1) /= (res - 1); d(2) /= (res - 1);

	// prepare progression
	cgv::utils::progression prog;
	prog.init("export volume", res, 10);

	// iterate through all slices
	bool set = false;
	unsigned int i, j, k;
	for (k = 0; k < res; ++k, p(2) += d(2)) {
		prog.step();
		for (j = 0, p(1) = box.get_min_pnt()(1); j < res; ++j, p(1) += d(1)) {
			for (i = 0, p(0) = box.get_min_pnt()(0); i < res; ++i, p(0) += d(0)) {
				double v = func_ptr->evaluate(p.to_vec());
				unsigned char value;
				if (map_to_zero_value < map_to_one_value) {
					if (v <= map_to_zero_value)
						value = 0;
					else if (v >= map_to_one_value)
						value = 255;
					else
						value = (unsigned char)(int) (255 * (v - map_to_zero_value) / (map_to_one_value - map_to_zero_value));
				}
				else {
					if (v >= map_to_zero_value)
						value = 0;
					else if (v <= map_to_one_value)
						value = 255;
					else
						value = (unsigned char)(int) (255 * (map_to_zero_value - v) / (map_to_zero_value - map_to_one_value));
				}
				data.push_back(value);
			}
		}
	}
	cgv::utils::file::write(fn, (const char*)&data.front(), data.size());
}

/// callback used to save to obj file
void gl_implicit_surface_drawable::save_interactive()
{
	std::string fn = file_save_dialog("choose obj output file", "Obj Files (obj):*.obj|All Files:*.*");
	if (fn.empty())
		return;
	std::ofstream os(fn.c_str());
	if (os.fail())
		return;
	obj_out = &os;
	normal_index = 0;
	surface_extraction();
	obj_out = 0;
}

void gl_implicit_surface_drawable::surface_extraction()
{
	gl_implicit_surface_drawable_base::surface_extraction();
	update_member(&nr_faces);
	update_member(&nr_vertices);
}

void gl_implicit_surface_drawable::resolution_change()
{
	if (find_control(ix)) {
		find_control(ix)->set("max",res-1);
		find_control(iy)->set("max",res-1);
		find_control(iz)->set("max",res-1);
	}
	post_rebuild();
}

void gl_implicit_surface_drawable::stream_help(std::ostream& os)
{
}

bool gl_implicit_surface_drawable::init(cgv::render::context& ctx)
{
	view_ptr = find_view_as_node();
	return gl_implicit_surface_drawable_base::init(ctx);
}

bool gl_implicit_surface_drawable::handle(cgv::gui::event& e)
{
	// check for mouse click
	if (e.get_kind() == cgv::gui::EID_MOUSE) {
		auto& me = reinterpret_cast<cgv::gui::mouse_event&>(e);
		if (view_ptr) {
			if (me.get_action() == cgv::gui::MA_PRESS) {
				if (me.get_modifiers() == cgv::gui::EM_CTRL) {
					if (me.get_button() == cgv::gui::MB_LEFT_BUTTON) {
						dvec3 p;
						if (get_world_location(me.get_x(), me.get_y(), *view_ptr, p)) {
							// and define mini box location such that it includes the picked location
							dvec3 q = (p - box.get_min_pnt())/(1.0/(res-1)*box.get_extent());
							ix = int(q(0));
							if (ix < 0)
								ix = 0;
							if (ix >= res)
								ix = res - 1;
							iy = int(q(1));
							if (iy < 0)
								iy = 0;
							if (iy >= res)
								iy = res - 1;
							iz = int(q(2));
							if (iz < 0)
								iz = 0;
							if (iz >= res)
								iz = res - 1;
							update_member(&ix);
							update_member(&iy);
							update_member(&iz);
							post_redraw();
						}
					}
				}
			}
		}
	}
	if (e.get_kind() != cgv::gui::EID_KEY)
		return false;
	auto& ke = reinterpret_cast<cgv::gui::key_event&>(e);
	if (ke.get_action() != cgv::gui::KA_PRESS)
		return false;
	switch (ke.get_key()) {
	case 'M':
		if (ke.get_modifiers() != 0)
			return false;
		contouring_type = MARCHING_CUBES;
		on_set(&contouring_type);
		return true;
	case 'D':
		if (ke.get_modifiers() != 0)
			return false;
		contouring_type = DUAL_CONTOURING;
		on_set(&contouring_type);
		return true;
	case 'N':
		if (ke.get_modifiers() == 0) {
			show_gradient_normals = !show_gradient_normals;
			on_set(&show_gradient_normals);
		}
		else {
			show_mesh_normals = !show_mesh_normals;
			on_set(&show_mesh_normals);
		}
		return true;
	case 'B' :
		if (ke.get_modifiers() != 0)
			return false;
		show_box = !show_box;
		on_set(&show_box);
		return true;
	case 'V':
		if (ke.get_modifiers() != 0)
			return false;
		show_vertices = !show_vertices;
		on_set(&show_vertices);
		return true;
	case 'W':
		if (ke.get_modifiers() != 0)
			return false;
		show_wireframe = !show_wireframe;
		on_set(&show_wireframe);
		return true;
	case 'S':
		if (ke.get_modifiers() != 0)
			return false;
		show_surface = !show_surface;
		on_set(&show_surface);
		return true;
	case 'G' :
		if (ke.get_modifiers() != 0)
			return false;
		normal_computation_type = GRADIENT_NORMALS;
		on_set(&normal_computation_type);
		return true;
	case 'F':
		if (ke.get_modifiers() != 0)
			return false;
		normal_computation_type = FACE_NORMALS;
		on_set(&normal_computation_type);
		return true;
	case 'C':
		normal_computation_type = ke.get_modifiers() == cgv::gui::EM_SHIFT ? CORNER_GRADIENTS : CORNER_NORMALS;
		on_set(&normal_computation_type);
		return true;
	case 'Q':
		if (ke.get_modifiers() != 0)
			return false;
		res *= 2;
		on_set(&res);
		return true;
	case 'A':
		if (ke.get_modifiers() != 0 || res < 4)
			return false;
		res /= 2;
		on_set(&res);
		return true;
	}
	return false;
}

/// you must overload this for gui creation
void gl_implicit_surface_drawable::create_gui()
{
	if (begin_tree_node("Tesselation", triangulate)) {
		align("\a");
		connect_copy(add_button("save to obj")->click, rebind(this, &gl_implicit_surface_drawable::save_interactive));
		add_member_control(this, "triangulate", triangulate, "check");
		add_view("nr_vertices", nr_vertices);
		add_view("nr_faces", nr_faces);
		end_tree_node(triangulate);
		align("\b");
	}

	if (begin_tree_node("Volume Export", map_to_zero_value)) {
		align("\a");
		connect_copy(add_button("toggle range")->click, rebind(this, &gl_implicit_surface_drawable::toggle_range));
		connect_copy(add_button("adjust range")->click, rebind(this, &gl_implicit_surface_drawable::adjust_range));
		add_member_control(this, "map to zero", map_to_zero_value, "value_slider");
		add_member_control(this, "map to one", map_to_one_value, "value_slider");
		connect_copy(add_button("save to vox")->click, rebind(this, &gl_implicit_surface_drawable::export_volume));
		end_tree_node(map_to_zero_value);
		align("\b");
	}

	if (begin_tree_node("Contouring", contouring_type)) {
		align("\a");
		add_member_control(this, "normal computation", normal_computation_type, "dropdown", "enums='gradient,face,corner,corner_gradient'");
		add_member_control(this, "threshold", normal_threshold, "value_slider", "min=-1;max=1;ticks=true");
		add_member_control(this, "contouring", contouring_type, "dropdown", "enums='marching cubes,dual contouring'");
		add_member_control(this, "consistency_threshold", consistency_threshold, "value_slider", "min=0.00001;max=1;log=true;ticks=true");
		add_member_control(this, "max_nr_iters", max_nr_iters, "value_slider", "min=1;max=20;ticks=true");
		add_member_control(this, "res", res, "value_slider", "min=4;max=100;log=true;ticks=true");
		add_member_control(this, "epsilon", epsilon, "value_slider", "min=0;max=0.001;log=true;ticks=true");
		add_member_control(this, "grid_epsilon", grid_epsilon, "value_slider", "min=0;max=0.5;log=true;ticks=true");
		end_tree_node(contouring_type);
		align("\b");
	}

	if (begin_tree_node("Visualization", show_wireframe)) {
		align("\a");
		bool show = begin_tree_node("vertices", srs, false, "level=3;options='w=130';align=' '");
		add_member_control(this, "show", show_vertices, "toggle", "w=62");
		if (show) {
			align("\a");
			add_gui("render style", srs);;
			align("\b");
			end_tree_node(srs);
		}

		show = begin_tree_node("wireframe", crs, false, "level=3;options='w=130';align=' '");
		add_member_control(this, "show", show_wireframe, "toggle", "w=62");
		if (show) {
			align("\a");
			add_gui("render style", crs);
			align("\b");
			end_tree_node(crs);
		}
		show = begin_tree_node("surface", material, false, "level=3;options='w=130';align=' '");
		add_member_control(this, "show", show_surface, "toggle", "w=62");
		if (show) {
			align("\a");
			add_gui("material", material);
			align("\b");
			end_tree_node(material);
		}
		show = begin_tree_node("normals", ars, false, "level=3;options='w=80';align=' '");
		add_member_control(this, "gradient", show_gradient_normals, "toggle", "w=62", " ");
		add_member_control(this, "mesh", show_mesh_normals, "toggle", "w=62");
		if (show) {
			align("\a");
			add_gui("render_style", ars);
			align("\b");
			end_tree_node(ars);
		}
		show = begin_tree_node("box", brs, false, "level=3;options='w=60';align=' '");
		add_member_control(this, "show", show_box, "toggle", "w=62", " ");
		add_member_control(this, "mini", show_mini_box, "toggle", "w=62");
		if (show) {
			align("\a");
			add_decorator("box", "heading", "level=4");
			add_gui("box", box, "", "options='w=100;min=-10;max=10;step=0.01;ticks=true;align=\"BL\"';align_col=' '"); align("\n");
			add_decorator("render style", "heading", "level=4");
			add_gui("render style", brs);
			align("\b");
			end_tree_node(material);
		}
		add_member_control(this, "ix", ix, "value_slider", "min=0;max=10;ticks=true");
		add_member_control(this, "iy", iy, "value_slider", "min=0;max=10;ticks=true");
		add_member_control(this, "iz", iz, "value_slider", "min=0;max=10;ticks=true");
		add_member_control(this, "sampling_points", show_sampling_locations, "check");
		add_member_control(this, "sampling_grid", show_sampling_grid, "check");
		add_member_control(this, "sampling_grid_opacity", sampling_grid_alpha, "value_slider", "min=0;max=1;ticks=true");
		end_tree_node(show_wireframe);
		align("\b");
	}
}

bool gl_implicit_surface_drawable::self_reflect(cgv::reflect::reflection_handler& rh)
{
	return 
//		rh.reflect_member("box", box) &&
	//	rh.reflect_member("contouring_type", contouring_type) &&
		rh.reflect_member("normal_threshold", normal_threshold) &&
		rh.reflect_member("consistency_threshold", consistency_threshold) &&
		rh.reflect_member("max_nr_iters", max_nr_iters) &&
//		rh.reflect_member("normal_computation_type", normal_computation_type) &&
		rh.reflect_member("ix", ix) &&
		rh.reflect_member("iy", iy) &&
		rh.reflect_member("iz", iz) &&
		rh.reflect_member("show_wireframe", show_wireframe) &&
		rh.reflect_member("show_sampling_grid", show_sampling_grid) &&
		rh.reflect_member("show_sampling_locations", show_sampling_locations) &&
		rh.reflect_member("show_box", show_box) &&
		rh.reflect_member("show_mini_box", show_mini_box) &&
		rh.reflect_member("show_gradient_normals", show_gradient_normals) &&
		rh.reflect_member("show_mesh_normals", show_mesh_normals) &&
		rh.reflect_member("epsilon", epsilon) &&
		rh.reflect_member("grid_epsilon", grid_epsilon);
//	&& rh.reflect_member("material", material);
}

void gl_implicit_surface_drawable::on_set(void* p)
{
	if (p == &res)
		resolution_change();
	else if (p == &contouring_type || p == &res || p == &normal_threshold || p == &consistency_threshold || 
		 p == &max_nr_iters || p == &normal_computation_type || p == &epsilon ||
		 p == &grid_epsilon || (p >= &box && p < &box+1) )
		   post_rebuild();
	else if (p == &ix || p == &iy || p == &iz || p == &show_vertices || p == &show_wireframe || p == &show_surface || 
		p == &show_sampling_grid || p == &sampling_grid_alpha || 
	    p == &show_sampling_locations || p == &show_box || p == &show_mini_box || 
		 p == &show_gradient_normals || p == &show_mesh_normals || 
		(p >= &material && p < &material+1) || (p >= &srs && p < &srs + 1) || 
		(p >= &crs && p < &crs + 1) || (p >= &brs && p < &brs + 1) || (p >= &ars && p < &ars + 1))
			post_redraw();
	update_member(p);
}

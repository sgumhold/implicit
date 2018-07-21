#include "mls.h"
#include <cgv_gl/gl/gl.h>
#include <cgv/utils/file.h>
#include <cgv/gui/file_dialog.h>
#include <cgv/utils/scan.h>
#include <cgv/media/mesh/obj_loader.h>
#include <cgv/media/axis_aligned_box.h>

using namespace cgv::utils::file;
using namespace cgv::utils;
using namespace cgv::signal;

template <typename T, typename P, typename C>
mls_surface<T,P,C>::mls_surface()
{
	point_size = 3;
	h = 0.01f;
	extent = 1;
	ann = 0;
	skip_eval = false;
	gui_color = 0xFF8888;
}

template <typename T, typename P, typename C>
mls_surface<T,P,C>::~mls_surface()
{
	if (ann)
		delete ann;
}

template <typename T, typename P, typename C>
void mls_surface<T,P,C>::build_ann()
{
	axis_aligned_box<P,3> box;

	if (ann)
		delete ann;
	ANNpointArray pa = new ANNpoint[points.size()];
	for (unsigned i=0; i<points.size(); ++i) {
		pa[i] = &points[i](0);
		box.add_point(points[i]);
	}
	extent = box.get_extent().length();
	update_member(&extent);
	ann = new ANNkd_tree(pa,points.size(),3);
}

template <typename T, typename P, typename C>
void mls_surface<T,P,C>::open()
{
	string _file_name = file_open_dialog("open point cloud", "Obj Files (obj,pobj):*.obj;*.pobj");
	read(_file_name);
}

template <typename T, typename P, typename C>
bool mls_surface<T,P,C>::read(const string& _file_name)
{
	if (!exists(_file_name)) {
		cerr << "mls_surface::read ... could not find file " << _file_name << endl;
		return false;
	}
	string ext = to_lower(cgv::utils::file::get_extension(_file_name));
	if (ext == "obj" || ext == "pobj")
		return read_obj(_file_name);

	std::cerr << "mls_surface::read ... unknown file extension " << ext << endl;
	return false;
}

template <typename T, typename P, typename C>
bool mls_surface<T,P,C>::read_obj(const string& _file_name)
{
	mesh::obj_loader l;
	if (!l.read_obj(_file_name))
		return false;
	points.resize(l.vertices.size());
	for (unsigned i=0; i<l.vertices.size(); ++i) {
		points[i].set((P)l.vertices[i](0),(P)l.vertices[i](1),(P)l.vertices[i](2));
	}
	if (!l.normals.empty()) {
		normals.resize(l.normals.size());
		for (unsigned i=0; i<l.normals.size(); ++i) {
			normals[i].set((P)l.normals[i](0),(P)l.normals[i](1),(P)l.normals[i](2));
		}
	}
	file_name = _file_name;
	build_ann();
	update_scene();
	return true;
}

template <typename T, typename P, typename C>
bool mls_surface<T,P,C>::self_reflect(reflection_handler& rh)
{
	return
		rh.reflect_member("h", h) &&
		rh.reflect_member("show_points", show_points) &&
		rh.reflect_member("skip_eval", skip_eval) &&
		rh.reflect_member("point_size", point_size) &&
		rh.reflect_member("file_name", file_name) &&
		implicit_primitive<T>::self_reflect(rh);
}

template <typename T, typename P, typename C>
void mls_surface<T,P,C>::on_set(void* member_ptr)
{
	if (member_ptr == &file_name)
		read(file_name);
	else 
		if (!(member_ptr == &h || member_ptr == &file_name || member_ptr == &skip_eval)) {
			update_description();
			return;
		}
	implicit_primitive<T>::on_set(member_ptr);
}

template <typename T, typename P, typename C>
T mls_surface<T,P,C>::evaluate(const pnt_type& _p) const
{
	if (points.empty() || skip_eval)
		return 0;
	P3d p((P)_p(0),(P)_p(1),(P)_p(2));
	last_p = p;
	static ANNidx idx_arr[10000];
	static P dist_arr[10000];
	P d = extent*h;
	P dd = d*d;
	P idd = 1.0f/dd;
	int n = ann->annkFRSearch(p,9*dd,10000,idx_arr,dist_arr,0.01);
	if (n==0){
//		last_nml.set(1,0,0);
//		return 3*d;
		ann->annkSearch(p,1,idx_arr,dist_arr,0.01);
		P d1 = sqrt(dist_arr[0]);
		P d2 = dot(p-points[idx_arr[0]],normals[idx_arr[0]]);
		if (10*fabs(d2) > d1) {
			last_nml = normals[idx_arr[0]];
			return d2;
		}
		else {
			last_nml = points[idx_arr[0]]-p;
			last_nml.normalize();
			return d1;
		}
//		n = ann->annkFRSearch(p,sqr(sqrt(dist_arr[0])+3*d),10000,idx_arr,dist_arr,0.01);
	}
	P sum = 0;
	P3d ctr(0,0,0);
	V3d nml(0,0,0);
	for (int i=0; i<n; ++i) {
		P theta = exp(-(dist_arr[i]*dist_arr[i])*idd);
		sum += theta;
		ctr += theta*points[idx_arr[i]];
		nml += theta*normals[idx_arr[i]];
	}
	ctr /= sum;
	nml.normalize();
	last_nml = nml;
	return dot(p-ctr,nml);
}


template <typename T, typename P, typename C>
typename mls_surface<T,P,C>::vec_type mls_surface<T,P,C>::evaluate_gradient(const pnt_type& p) const
{
	if (points.empty() || skip_eval)
		return vec_type(1,0,0);

	if (sqr_length(last_p - P3d((P)p(0),(P)p(1),(P)P(2))) > 1e-10)
		evaluate(p);
	return vec_type(last_nml(0),last_nml(1),last_nml(2));
}


template <typename T, typename P, typename C>
void mls_surface<T,P,C>::draw(context& ctx)
{
	if (points.empty() || !show_points)
		return;

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	glPushAttrib(GL_POINT_BIT|GL_LIGHTING);
	glVertexPointer(3, GL_FLOAT, 0, &points[0]);
	glEnableClientState(GL_VERTEX_ARRAY);
	if (!colors.empty()) {
		glColorPointer(3, GL_UNSIGNED_BYTE, 0, &colors[0]);
		glEnableClientState(GL_COLOR_ARRAY);
	}
	if (!normals.empty()) {
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnable(GL_LIGHTING);
		if (!colors.empty())
			glEnable(GL_COLOR_MATERIAL);
		else
			glDisable(GL_COLOR_MATERIAL);
	}
	else
		glDisable(GL_LIGHTING);
	glColor3f(0.5f,0.5f,0.5f);
	glPointSize(point_size);
	glEnable(GL_POINT_SMOOTH);
	glDrawArrays(GL_POINTS, 0, points.size());
	glPopAttrib();
	glPopClientAttrib();
}

template <typename T, typename P, typename C>
void mls_surface<T,P,C>::create_gui()
{
	add_view("extent", extent);
	add_member_control(this, "show_points", show_points, "check"); 
	add_member_control(this, "skip_eval", skip_eval, "check"); 
	add_member_control(this, "h", h, "value_slider", "min=0.001;max=1;ticks=true;log=true"); 
	add_member_control(this, "point_size", point_size, "value_slider", "min=1;max=30;ticks=true;log=true"); 
	connect_copy(add_button("open")->click, rebind(this, &mls_surface<T,P,C>::open));
}

scene_factory_registration<mls_surface<double,float,unsigned char> > sfr_mls("mls");
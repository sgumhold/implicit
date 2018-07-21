#include "distance_surface.h"

// compute vector from closest point on skeleton edge i to point p
template <typename T>
typename distance_surface<T>::vec_type distance_surface<T>::get_edge_distance_vector(size_t i, const pnt_type &p) const
{
	vec_type v = p - points[edges[i].first];
	vec_type e = points[edges[i].second] - points[edges[i].first];
	T alpha=dot(v, e) / dot(e,e);
	alpha = dot(v, edge_vector_inv_length[i]);
	if (alpha <= 0)
		return v;
	if (alpha >= 1) {
		v= p - points[edges[i].second];
		return v;
	}
	v -= e*alpha;
	return v;
}
/// compute vector v from closest point on skeleton to point p and return value of implicit function
template <typename T>
double distance_surface<T>::put_distance_vector(const pnt_type &p, vec_type& v) const
{
	vec_type vt;

	double m, vn;

	v = get_edge_distance_vector(0,p);
	m = v.sqr_length();
	for (size_t i = 1; i < edges.size(); ++i) {
		vt = get_edge_distance_vector(i,p);
		vn = vt.sqr_length();
		if (vn<m){
			m=vn;
			v=vt;
		}
	}
	return sqrt(m)-r;
}
/// update helper variables for edge i
template <typename T>
void distance_surface<T>::update_edge_precomputations(size_t ei)
{
	edge_vector[ei]  = points[edges[ei].second]-points[edges[ei].first];
	edge_vector_inv_length[ei] = (T(1)/ edge_vector[ei].sqr_length()) * edge_vector[ei];
}

/// construct distance surface
template <typename T>
distance_surface<T>::distance_surface()
{
	r=0.5;
	gui_title_added = false;
}
/// reflect members to expose them to serialization
template <typename T>
bool distance_surface<T>::self_reflect(cgv::reflect::reflection_handler& rh)
{
	return
		skeleton<T>::self_reflect(rh) &&
		rh.reflect_member("r", r);
}

template <typename T>
void distance_surface<T>::append_edge_callback(size_t ei)
{
	edge_vector.push_back(vec_type(0,0,0));
	edge_vector_inv_length.push_back(vec_type(0,0,0));
	update_edge_precomputations(ei);
}
template <typename T>
void distance_surface<T>::edge_changed_callback(size_t ei)
{
	update_edge_precomputations(ei);
}
template <typename T>
void distance_surface<T>::position_changed_callback(size_t pi)
{
	for (unsigned ei = 0; ei < edges.size(); ++ei) 
		if (edges[ei].first == pi || edges[ei].second == pi)
			update_edge_precomputations(ei);
}

/// evaluate the implicit function itself, this must be overloaded
template <typename T>
T distance_surface<T>::evaluate(const pnt_type& p) const
{
	vec_type dummy;
	return put_distance_vector(p,dummy);
}

template <typename T>
typename distance_surface<T>::vec_type distance_surface<T>::evaluate_gradient(const pnt_type& p) const
{
	vec_type g;
	put_distance_vector(p,g);
	return g;
}

template <typename T>
void distance_surface<T>::create_gui()
{
	if (!gui_title_added) {
		add_view("distance surface",name)->set("color",0xFF8888);
		gui_title_added = true;
	}

	add_member_control(this, "radius", r, "value_slider", "min=0;max=5;log=true;ticks=true");

	skeleton<T>::create_gui();
}

scene_factory_registration<distance_surface<double> > sfr_distance_surface("distance_surface;D");

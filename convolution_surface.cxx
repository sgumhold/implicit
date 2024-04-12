#include "distance_surface.h"

template <typename T>
class convolution_surface :  public distance_surface<T>
{
public:
	/// main templated superclass which we want to inherit stuff from
	typedef distance_surface<T> base;
		using typename base::vec_type;
		using typename base::pnt_type;
		using base::r;
		using base::points;
		using base::edges;
		using base::edge_vector;
		using base::edge_vector_inv_length;
		using base::evaluate_gradient;

private:
	/// v= shortest distance from line to p
	T put_line_distance_vector(size_t ei, const pnt_type &p, vec_type& v) const
	{
		v=p-points[edges[ei].first];
		T alpha=dot(v,edge_vector_inv_length[ei]);
		v-= (edge_vector[ei]*alpha);
		return alpha;
	}
	/// updating edge_vector for segment [p[i],p[i+1]]
	void update_edge_precomputations(size_t ei) 
	{
		base::update_edge_precomputations(ei);
		edge_length[ei]=edge_vector[ei].length();
	}
	/// precomputed helper variables
	T a,b;
	std::vector<T> edge_length;
public:
	/// standard constructor
	convolution_surface() {
		base::on_set(&r);
	}
	/// overload to return the type name of this object
	std::string get_type_name() const { return "convolution_surface"; }
	/// 
	void on_set(void* member_ptr) {
		if (member_ptr == &r) {
			a = sqrt(log(2.0)) / r;
			b = log(2.0) / (r*r);
		}
		skeleton<T>::on_set(member_ptr);
	}
	void append_edge_callback(size_t pi)
	{
		edge_length.push_back(0);
		distance_surface<T>::append_edge_callback(pi);
	}
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const
	{
		vec_type v;
		T alpha, f=0;
		for(size_t ei=0; ei<edges.size();++ei) {
			alpha = put_line_distance_vector(ei,p,v);

			T x    = alpha*edge_length[ei];
			T arg1 = x*a;
			T arg2 = (edge_length[ei]-x)*a;

			T t1   = exp(-b*v.sqr_length());
			T t2   = erf(arg1)+erf(arg2);
			f += t1*t2;
		}
		return 1-f;
	}
	vec_type evaluate_gradient(const pnt_type& p) const
	{
		vec_type grad(0,0,0);
		vec_type v(3); 
		T alpha;
		for(size_t ei=0; ei<edges.size();++ei) {
			alpha = put_line_distance_vector(ei,p,v);

			T x    = alpha*edge_length[ei];
			T arg1 = x*a;
			T arg2 = (edge_length[ei]-x)*a;

			T t1   = exp(-b*v.sqr_length());
			T t2   = erf(arg1)+erf(arg2);
			T f    = t1*t2;

			v      = -2*b*f*v;
			T fx   = 2*a*t1*(exp(-arg1*arg1) - exp(-arg2*arg2))*.5641895835;//1.0/sqrt(PI);

			grad -= v;
			grad -= edge_vector[ei]*(fx/edge_length[ei]);
		}
		return grad;
	}
	void create_gui()
	{
		base::add_view("convolution surface", base::name)->set("color",0xFF8888);
		base::gui_title_added = true;
		distance_surface<T>::create_gui();
	}
};

scene_factory_registration<convolution_surface<double> > sfr_convolution_surface("convolution_surface;C");

#pragma once

#include "implicit_primitive.h"
#define ANN_USE_FLOAT
#include <ANN/ANN.h>

using namespace std;
using namespace cgv::math;
using namespace cgv::base;
using namespace cgv::media;
using namespace cgv::reflect;
using namespace cgv::render;
using namespace cgv::gui;
using namespace cgv::type;


template <typename T, typename P = T, typename C = float>
class mls_surface : public implicit_primitive<T>
{
public:
	typedef fvec<P,3> P3d;
	typedef fvec<P,3> V3d;
	typedef cgv::media::color<C, cgv::media::RGB> Rgb;
private:
	mutable P3d last_p;
	mutable V3d last_nml;
protected:
	ANNpointSet* ann;
	P h;
	P extent;
	bool show_points;
	bool skip_eval;
	vector<P3d> points;
	vector<V3d> normals;
	vector<Rgb> colors;
	string file_name;
	float point_size;
	void build_ann();
public:
	mls_surface();
	~mls_surface();
	std::string get_type_name() const { return "mls_surface"; }
	void open();
	bool read(const string& _file_name);
	bool read_obj(const string& _file_name);
	bool self_reflect(reflection_handler&);
	void on_set(void* member_ptr);
	T evaluate(const pnt_type& p) const;
	vec_type evaluate_gradient(const pnt_type& p) const;
	void draw(context& ctx);
	void create_gui();
};

#pragma once

#include "scene.h"
#include <cgv/media/color.h>
#include <cgv/gui/provider.h>
#include <cgv/render/drawable.h>

using namespace cgv::math;
using namespace cgv::render;
using namespace cgv::render::gl;
using namespace cgv::gui;

#define sqr(x) ((x)*(x))

/** base implementation for all group nodes*/
template <typename T>
class implicit_base : 
	public scene_updater,
	public drawable,
	public provider
{
public:
	/// coordinate type
	typedef T crd_type;
	/// type of primitive color
	typedef cgv::media::color<float, cgv::media::RGB, cgv::media::OPACITY> clr_type;
	/// type of 3d vector
	typedef cgv::math::fvec<double, 3> vec_type;
	/// type of 3d point
	typedef cgv::math::fvec<double, 3> pnt_type;
	/// color of implicit primitive
	clr_type color;
	/// gui color
	int gui_color;
	/// constructor sets default gui color
	implicit_base();
	/// returns "implicit_primitive"
	std::string get_type_name() const;
	/// convert to cgv::base::base pointer
	virtual cgv::base::base* get_base() = 0;
	/// interface for evaluation of implicit function
	virtual crd_type evaluate(const pnt_type& p) const = 0;
	/// interface for evaluation of the gradient with central differences based default implementation
	virtual vec_type evaluate_gradient(const pnt_type& p) const;
	/// interface for the evaluation of surface color
	virtual clr_type evaluate_color(const pnt_type& p) const;
};


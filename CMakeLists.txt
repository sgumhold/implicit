cmake_minimum_required(VERSION 3.24)
project("implicit" C CXX)

# pre-set CGV Framework options
option(CGV_BUILD_EXAMPLES off)
option(CGV_BUILD_WITH_AUDIO off)

# Include the CGV Framework
if (DEFINED ENV{CGV_DIR})
	# Use system-installed CGV Framework if present
	set(CGV_DIR $ENV{CGV_DIR})
	message("Using CGV Framework source found at:")
	message("> ${CGV_DIR}")
	add_subdirectory(${CGV_DIR} ./framework)
else()
	message("Including the CGV Framework source from https://github.com/sgumhold/cgv.git")
	message("This could take a while when run the first time...")
	include(FetchContent)
	FetchContent_Declare(
		cgv GIT_REPOSITORY https://github.com/sgumhold/cgv.git
		GIT_TAG 6334805fdcab01a73d3e6ea78e2d10f4e814889d
	)
	FetchContent_MakeAvailable(cgv)
endif()

# Compile a list of source files for each specific source type the CGV CMake build system knows about
set(SOURCES
	blend.cxx
	box.cxx
	convolution_surface.cxx
	csg.cxx
	cylinder.cxx
	distance_surface.cxx
	gl_implicit_surface_drawable.cxx
	heart.cxx
	implicit_base.cxx
	implicit_group.cxx
	implicit_primitive.cxx
	knot_vector.cxx
	level_set.cxx
	marschner_lobb.cxx
	meta_balls.cxx
	mls.cxx
	mul.cxx
	numeric_gradient.cxx
	outline.cxx
	plane.cxx
	quadric.cxx
	scene.cxx
	skeleton.cxx
	spatial_blend.cxx
	sphere.cxx
	super_quadric.cxx
	torus.cxx
	transform.cxx
	tube.cxx
	warping.cxx
)
set(HEADERS
	distance_surface.h
	gl_implicit_surface_drawable.h
	hierarchical_marching_cubes.h
	implicit_base.h
	implicit_group.h
	implicit_primitive.h
	intersect.h
	knot_vector.h
	mls.h
	scene.h
	skeleton.h
	union.h
)

# add our target to the CGV CMake build system
cgv_add_target(implicit
	TYPE plugin
	SOURCES        ${SOURCES}
	HEADERS        ${HEADERS}

	DEPENDENCIES
		cgv_viewer annf cgv_os cgv_utils cgv_type cgv_data cgv_base cgv_reflect cgv_math cgv_signal cgv_media
		cgv_gui cgv_render cgv_gl cg_fltk crg_grid crg_light crg_stereo_view

	ADDITIONAL_CMDLINE_ARGS
		"config:\"${CMAKE_CURRENT_LIST_DIR}/implicit.def\""
)

# Visual Studio fluff
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT implicit)

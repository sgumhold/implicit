#pragma once

#include "implicit_primitive.h"

/** the knot_vector class manages a vector of knot locations and provides callbacks
    in case that points are changed or appended. This is the base class for meta balls
	and skeletons.*/
template <typename T>
class knot_vector : public implicit_primitive<T>
{
protected:
	// vector of knot locations
	std::vector<pnt_type> points;
	/// append a point to the sketelon and call the append callback
	void append_point(const pnt_type& p);
	//! virtual function: can be overwritten by derived classes for data
	/*! updates when points in knot vector are appended or changed. **/
	virtual void append_callback(size_t pi) {}
	virtual void position_changed_callback(size_t pi) {}
	virtual void point_index_selection_callback() {}

	/// index of point that can be edited in user interface
	unsigned int pnt_idx;
private:
	/// point used in user interface for editing of points[pnt_idx]
	pnt_type p;
public:
	/// construct with empty skeleton
	knot_vector();
	/// overload to return the type name of this object
	std::string get_type_name() const { return "knot_vector"; }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh);

	/// implementation of updates needed after members changed
	void on_set(void* member_ptr);
	/// create gui to edit the points in the knot vector and to allow appending a new point
	void create_gui();
};

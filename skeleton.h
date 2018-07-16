#pragma once

#include "knot_vector.h"
#include <cgv/render/drawable.h>
#include <cgv_gl/gl/gl.h>

template <typename T>
class skeleton : 
	public knot_vector<T>, 
	public cgv::render::drawable
{
public:
	// edge is a pair of indices
	typedef std::pair<size_t, size_t> edge_type;
protected:
	// append an edge to the sketelon
	void append_edge(const edge_type& edge);
	// the edges
	std::vector<edge_type> edges;
	/// index of edge that can be edited in user interface
	unsigned int edge_idx;
private:
	/// edge used in user interface for editing of edges[edge_idx]
	edge_type edge;
protected:
	/// virtual function: can be overwritten by child for data
	/** updates when a point is appended. Called by doAppend **/
	virtual void append_edge_callback(size_t ei) {}
	virtual void edge_changed_callback(size_t ei) {}
	virtual void edge_index_selection_callback() {}
	// show skeleton edges
	bool show_edges;
	// line width of skeleton edges
	float edge_width;
public:
	/// construct with empty skeleton
	skeleton();
	/// overload to return the type name of this object
	std::string get_type_name() const { return "skeleton"; }
	/// reflect members to expose them to serialization
	bool self_reflect(cgv::reflect::reflection_handler& rh);
	/// implementation of updates needed after members changed
	void on_set(void* member_ptr);
	/// its a drawable
	void draw(cgv::render::context& ctx);
	/// overload append point callback to configure gui of current edge
	void append_callback(size_t pi);
	/// extend knot vector gui with support for appending and editing edges
	void create_gui();
};

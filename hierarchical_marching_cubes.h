#pragma once

#include <cgv/math/marching_cubes.h>

using namespace cgv::math;

/// class used to perform the marching cubes algorithm
template <typename X, typename T>
class hierarchical_marching_cubes : public marching_cubes<X,T>
{
public:
	/// construct marching cubes object
	hierarchical_marching_cubes(const v3_func<X,T>& _func,
						marching_cubes_callback_handler* _mccbh, 
						const X& _grid_epsilon = 0.01f, 
						const X& _epsilon = 1e-6f) :
		marching_cubes<X,T>(_func,_mccbh, _grid_epsilon, _epsilon)
	{
		idx_off = 0;
	}
	/// extract iso surface and send triangles to marching cubes handler
	void extract_hierarchically(int level, const T& _iso_value,
					 const cgv::media::axis_aligned_box<X>& box,
					 unsigned int resx, unsigned int resy, unsigned int resz,
					 bool show_progress = false)
	{
		int step = pow(2,level);
		resx = (resx/step+1)*step+1;
		resy = (resy/step+1)*step+1;
		resz = (resz/step+1)*step+1;
		// prepare private members
		p = box.minp;
		d = box.get_extent();
		d(0) /= (resx-1); d(1) /= (resy-1); d(2) /= (resz-1);
		d *= step;
		iso_value = _iso_value;

		// prepare progression
		cgv::utils::progression prog;
		if (show_progress) prog.init("extraction", (resz-1)/step, 10);

		// construct two slice infos
		unsigned int s;
		slice_info<T> **slice_infos = new slice_info<T>*[step+1];
		for (s = 0; s <= step; ++s)
			slice_infos[s] = new slice_info<T>(resx,resy);

		// iterate through all slices
		unsigned int nr_vertices[3] = { 0, 0, 0 };
		unsigned int i, j, k, n;
		for (k=0; k<resz; k += step, p(2) += d(2)) {
			n = (int)pnts.size();
			// evaluate function on next slice
			slice_info<T> *info_ptr = slice_infos[step];
			info_ptr->init();
			for (j = 0, p(1) = box.minp(1); j < resy; j += step, p(1) += d(1))
				for (i = 0, p(0) = box.minp(0); i < resx; i += step, p(0)+=d(0))
					info_ptr->set_value(i,j,func.evaluate(p),iso_value);
			// show progression
			if (show_progress) 
				prog.step();
			// if this is the first considered slice, construct the next one
			if (k != 0) {
				// get info of previous slice
				slice_info<T> *prev_info_ptr = slice_infos[0];
				// look for to be subdivided cells
				for (j = 0; j < resy-1; ++j)
					for (i = 0; i < resx-1; ++i) {
						// compute the bit index for the current cube
						int idx = prev_info_ptr->get_bit_code(i,j,step)+
									 16*info_ptr->get_bit_code(i,j,step);
						// skip empty cubes
						if (idx == 0 || idx == 255)
							continue;
						// recursive call to subdivide cells
						tesselate_cube_recursive(i,j,step/2,prev_info_ptr,info_ptr);
						// set edge vertices
						int vis[12] = {
							prev_info_ptr->index(i,j,0),
							prev_info_ptr->index(i+1,j,1),
							prev_info_ptr->index(i,j+1,0),
							prev_info_ptr->index(i,j,1),
							info_ptr->index(i,j,0),
							info_ptr->index(i+1,j,1),
							info_ptr->index(i,j+1,0),
							info_ptr->index(i,j,1),
							prev_info_ptr->index(i,j,2),
							prev_info_ptr->index(i+1,j,2),
							prev_info_ptr->index(i,j+1,2),
							prev_info_ptr->index(i+1,j+1,2) 
						};
						// lookup triangles and construct them
						int n = get_nr_cube_triangles(idx);
						for (int t=0; t<n; ++t) {
							int vi, vj, vk;
							put_cube_triangle(idx, t, vi, vj, vk);
							vi = vis[vi];
							vj = vis[vj];
							vk = vis[vk];
							if ( (vi != vj) && (vi != vk) && (vj != vk) )
								mccbh->triangle_callback(vi,vj,vk);
						}
					}
			}
			n = (int)pnts.size()-n;
			nr_vertices[k%3] = n;
			n = nr_vertices[(k+2)%3];
			if (n > 0) {
				for (i=0; i<n; ++i) {
					pnts.pop_front();
					nmls.pop_front();
				}
				idx_off += n;
			}
		}
	}

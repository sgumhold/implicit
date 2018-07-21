#include "scene.h"
#include <cgv/base/named.h>

#define sqr(x) ((x)*(x))

template <typename T>
struct box : 
	public cgv::math::v3_func<T,T>, 
	public cgv::gui::provider,
	public scene_updater,
	public cgv::base::named
{
	bool use_euclidean_distance;
	box() : use_euclidean_distance(true) {}
	std::string get_type_name() const { return "box"; }
	static int abs_max_idx(const pnt_type& p)
	{
		if (fabs(p(0)) > fabs(p(1))) {
			if (fabs(p(0)) > fabs(p(2)))
				return 0;
			else
				return 2;
		}
		else {
			if (fabs(p(1)) > fabs(p(2)))
				return 1;
			else
				return 2;
		}
	}
	void on_set(void* member_ptr) { update_scene(); }
	/// evaluate the implicit function itself, this must be overloaded
	T evaluate(const pnt_type& p) const {
		if (use_euclidean_distance) {
			int i;
			T distance = 0;
			for (i = 0; i < 3; ++i) {
				if (p(i) < -1)
					distance += sqr(p(i) + 1);
				else if (p(i) > 1)
					distance += sqr(p(i) - 1);
			}
			// if we are outside of box, we are done
			if (distance > 0)
				return sqrt(distance);
			// otherwise compute minimum distance to box faces
			distance = 2;
			for (i = 0; i < 3; ++i) {
				if (p(i) + 1 < distance)
					distance = p(i) + 1;
				if (1 - p(i) < distance)
					distance = 1 - p(i);
			}
			// and return negative distance as we are in the interior of the box
			return -distance;
		}
		else {
			return abs(p(abs_max_idx(p))) - 1;
		}
	}
	vec_type evaluate_gradient(const pnt_type& p) const {
		if (use_euclidean_distance) {
			int i;
			vec_type g(0, 0, 0);
			T distance = 0;
			for (i = 0; i < 3; ++i) {
				if (p(i) < -1) {
					distance += sqr(p(i) + 1);
					g(i) = p(i);
				}
				else if (p(i) > 1) {
					distance += sqr(p(i) - 1);
					g(i) = p(i) - 1;
				}
			}
			// if we are outside of box, we are done
			if (distance > 0)
				return T(1)/distance * g;
			// otherwise compute minimum distance to box faces
			distance = 1;
			for (i = 0; i < 3; ++i) {
				if (p(i) + 1 < distance) {
					distance = p(i) + 1;
					g = T(0);
					g(i) = -1;
				}
				if (1 - p(i) < distance) {
					distance = 1 - p(i);
					g = T(0);
					g(i) = 1;
				}
			}
			// and return negative distance as we are in the interior of the box
			return g;
		}
		else {
			vec_type g(0, 0, 0);
			int i = abs_max_idx(p);
			g(i) = p(i) > 0 ? 1 : -1;
			return g;
		}
	}
	void create_gui()
	{
		add_view("box",name)->set("color",0xFF8888);
		add_member_control(this, "use_euclidean_distance", use_euclidean_distance, "check");
	}
};

scene_factory_registration<box<double> >sfr_box("box;B");
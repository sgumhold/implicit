#include "implicit_group.h"

template <typename T>
class mul_node : public implicit_group<T>
{
public:
	mul_node() { gui_color = 0xffff00; }
	T evaluate(const pnt_type& p) const {
		T prod = 1;
		for (size_t i=0; i<get_nr_children(); ++i)
			prod *= get_implicit_child(i)->evaluate(p);
		return prod;
	}
	std::string get_type_name() const
	{
		return "mul";
	}	
};

scene_factory_registration<mul_node<double> >sfr_blend("multiply");

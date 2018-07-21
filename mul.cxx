#include "implicit_group.h"
#include <cgv/signal/rebind.h>
#include <cgv/type/variant.h>
#include "scene.h"

using namespace cgv::type;
using namespace cgv::signal;

template <typename T>
class mul_node : public implicit_group<T>
{
public:
	T evaluate(const pnt_type& p) const {
		T prod = 1;
		for (unsigned int i=0; i<func_children.size(); ++i)
			prod *= func_children[i]->evaluate(p);
		return prod;
	}
	void create_gui()
	{
		add_view("mul", name)->set("color", 0xffff00);
		implicit_group<T>::create_gui();
	}
	std::string get_type_name() const
	{
		return "mul";
	}	
};

scene_factory_registration<mul_node<double> >sfr_blend("multiply");

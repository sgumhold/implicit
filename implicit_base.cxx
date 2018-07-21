#include "implicit_base.h"

/// constructor sets default gui color
template <typename T>
implicit_base<T>::implicit_base() : color(0.5f, 0.5f, 0.5f, 1.0f)
{
	gui_color = 0x888888;
}

template <typename T>
std::string implicit_base<T>::get_type_name() const
{
	return "implicit_base";
}

/// interface for evaluation of the gradient with central differences based default implementation
template <typename T>
typename implicit_base<T>::vec_type implicit_base<T>::evaluate_gradient(const pnt_type& p) const
{
	static crd_type epsilon = 5 * std::numeric_limits<T>::epsilon();
	static crd_type inv_2_eps = T(2)/epsilon;
	vec_type g;
	pnt_type q(p);
	for (unsigned i = 0; i<3; ++i) {
		q(i) += epsilon;
		g(i) = evaluate(q);
		q(i) = p(i) - epsilon;
		g(i) -= evaluate(q);
		g(i) *= inv_2_eps;
		q(i) = p(i);
	}
	return g;
}

/// return primitive color
template <typename T>
typename implicit_base<T>::clr_type implicit_base<T>::evaluate_color(const pnt_type& p) const
{
	return color;
}

template class implicit_base<double>;
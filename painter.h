#ifndef NATA__PAINTER_H
#define NATA__PAINTER_H

#include "view.h"

namespace nata
{

template<typename T_tokens>
class t_painter
{
	T_tokens& v_tokens;
	size_t v_p;
	std::deque<typename T_tokens::t_span> v_xs;
	size_t v_xs_p;

public:
	t_painter(T_tokens& a_tokens) : v_tokens(a_tokens)
	{
	}
	size_t f_p() const
	{
		return v_p;
	}
	void f_reset()
	{
		v_p = v_xs_p = 0;
		v_xs.clear();
	}
	void f_push(decltype(T_tokens::t_span::v_x) a_x, size_t a_n)
	{
		if (a_n <= 0) return;
		v_p += a_n;
		if (!v_xs.empty()) {
			auto& x = v_xs.back();
			if (a_x == x.v_x) {
				x.v_n += a_n;
				return;
			}
		}
		v_xs.push_back({a_x, a_n});
	}
	void f_flush()
	{
		v_tokens.f_paint(v_xs_p, std::move(v_xs));
		v_xs_p = v_p;
	}
};

}

#endif

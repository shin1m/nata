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
	typename T_tokens::t_iterator v_i;

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
		v_p = 0;
		v_xs.clear();
		v_i = v_tokens.f_begin();
	}
	void f_push(decltype(T_tokens::t_span::v_x) a_x, size_t a_n, size_t a_merge = 0)
	{
		while (a_n > 0) {
			size_t n = std::min(a_n, v_i.f_index().v_i1 + v_i.f_delta().v_i1 - v_p);
			if (v_xs.empty()) {
				if (a_x != v_i->v_x) {
					v_xs.push_back({a_x, n});
					v_xs_p = v_p;
				}
			} else if (a_x == v_i->v_x && n > a_merge) {
				v_i = v_tokens.f_paint(v_xs_p, std::move(v_xs));
			} else if (a_x == v_xs.back().v_x) {
				v_xs.back().v_n += n;
			} else {
				v_xs.push_back({a_x, n});
			}
			v_p += n;
			while (v_i.f_index().v_i1 + v_i.f_delta().v_i1 <= v_p) ++v_i;
			a_n -= n;
		}
	}
	void f_flush()
	{
		if (v_xs.empty()) return;
		v_i = v_tokens.f_paint(v_xs_p, std::move(v_xs));
		while (v_i.f_index().v_i1 + v_i.f_delta().v_i1 <= v_p) ++v_i;
	}
};

}

#endif

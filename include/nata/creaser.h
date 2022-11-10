#ifndef NATA__CREASER_H
#define NATA__CREASER_H

#include <deque>
#include <numeric>
#include <vector>

namespace nata
{

template<typename T_rows>
class t_creaser
{
	T_rows& v_rows;
	size_t v_p;
	std::vector<typename T_rows::t_creases::t_iterator> v_path;

	size_t f_plain(size_t a_n)
	{
		while (a_n > 0) {
			auto& b = v_path.back();
			if (b->v_x) break;
			auto n = b.f_delta().v_i1;
			if (a_n < n) {
				++v_p;
				--a_n;
				if (--n <= 0) v_rows.f_creases().f_next(v_path);
				break;
			}
			v_p += n;
			a_n -= n;
			v_rows.f_creases().f_next(v_path);
		}
		return a_n;
	}
	void f_replace(size_t a_n, std::deque<typename T_rows::t_creases::t_span>&& a_xs)
	{
		size_t dirty_begin = ~0;
		size_t dirty_end = 0;
		for (auto p = v_p; p < v_p + a_n;) {
			while (true) {
				auto& b = v_path.back();
				if (!b->v_x) break;
				if (b->v_x->v_folded) {
					if (p < dirty_begin) dirty_begin = p;
					auto q = p + b.f_delta().v_i1;
					if (q > dirty_end) dirty_end = q;
					break;
				}
				v_path.push_back(b->v_x->v_nested.f_begin());
			}
			p += v_path.back().f_delta().v_i1;
			v_rows.f_creases().f_next(v_path);
		}
		v_path.clear();
		v_path.push_back(v_rows.v_creases.f_replace(v_p, a_n, std::move(a_xs)));
		if (dirty_begin < dirty_end) v_rows.v_tokens_painted(dirty_begin, dirty_end - dirty_begin);
	}

public:
	t_creaser(T_rows& a_rows) : v_rows(a_rows)
	{
		f_reset();
	}
	void f_reset()
	{
		v_p = 0;
		v_path.clear();
		v_path.push_back(v_rows.f_creases().f_begin());
	}
	size_t f_current() const
	{
		return v_p;
	}
	void f_push(size_t a_plain, size_t a_nested)
	{
		assert(a_nested > 0);
		auto enter = [&]
		{
			for (auto p = v_p;;) {
				auto& b = v_path.back();
				while (b.f_index().v_i1 + b.f_delta().v_i1 <= p) ++b;
				if (b.f_index().v_i1 >= p) break;
				p -= b.f_index().v_i1;
				v_path.push_back(b->v_x->v_nested.f_begin());
			}
			v_path.push_back(v_path.back()->v_x->v_nested.f_begin());
		};
		a_plain = f_plain(a_plain);
		if (a_plain > 0) {
			f_replace(a_plain + a_nested, {a_plain, {{{a_nested}}}});
			v_p += a_plain;
			return enter();
		}
		auto& b = v_path.back();
		if (b->v_x && a_nested == b.f_delta().v_i1) {
			v_path.push_back(b->v_x->v_nested.f_begin());
		} else {
			f_replace(a_nested, {{{{a_nested}}}});
			enter();
		}
	}
	void f_end()
	{
		auto plain = f_plain(v_rows.v_tokens.v_text.f_size() + 1 - v_p);
		if (plain <= 0) return;
		f_replace(plain, {plain});
		v_p += plain;
		++v_path.back();
	}
};

}

#endif

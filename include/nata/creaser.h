#ifndef NATA__CREASER_H
#define NATA__CREASER_H

#include <deque>
#include <numeric>
#include <vector>

namespace nata
{

template<typename T_rows, typename T_tag>
class t_creaser
{
	struct t_block
	{
		T_tag v_tag;
		size_t v_n = 0;
		bool v_folded = false;
		std::deque<typename T_rows::t_creases::t_span> v_xs;
		size_t v_dirty_begin = ~0;
		size_t v_dirty_end = 0;

		void f_dirty_begin(size_t a_p)
		{
			if (a_p < v_dirty_begin) v_dirty_begin = a_p;
		}
		void f_dirty_end(size_t a_p)
		{
			if (a_p > v_dirty_end) v_dirty_end = a_p;
		}
	};

	T_rows& v_rows;
	size_t v_p;
	size_t v_n;
	std::deque<t_block> v_nesting;
	size_t v_nesting_p;
	std::vector<typename T_rows::t_creases::t_iterator> v_path;
	size_t v_path_p;
	size_t v_path_folded;

	void f_plain()
	{
		if (v_n <= 0) return;
		auto& b = v_nesting.back();
		b.v_xs.emplace_back(v_n);
		if (v_p < v_path_folded) b.f_dirty_begin(v_p);
		v_p += v_n;
		v_n = 0;
		while (v_path_p < v_p) {
			if (v_path_p <= v_path_folded) b.f_dirty_end(v_path_p);
			while (v_path.back()->v_x) {
				if (v_path.back()->v_x->v_folded && v_path_p >= v_path_folded) v_path_folded = v_path_p + v_path.back().f_delta().v_i1;
				v_path.push_back(v_path.back()->v_x->v_nested.f_begin());
			}
			if (v_path_p < v_path_folded) b.f_dirty_begin(v_path_p);
			v_path_p += v_path.back().f_delta().v_i1;
			v_rows.f_creases().f_next(v_path);
		}
		if (v_p <= v_path_folded) b.f_dirty_end(v_p);
	}
	void f_foldable()
	{
		auto& b = v_nesting.back();
		auto xs = std::move(b.v_xs);
		size_t n = std::accumulate(xs.begin(), xs.end(), 0, [](size_t n, const auto& x)
		{
			return n + x.v_n;
		});
		auto i = v_rows.v_creases.f_replace(v_nesting_p, n, std::move(xs));
		if (b.v_dirty_begin < b.v_dirty_end) v_rows.v_tokens_painted(b.v_dirty_begin, b.v_dirty_end - b.v_dirty_begin);
		v_nesting.clear();
		v_nesting.push_back({{}});
		v_nesting_p = v_p;
		while (i.f_index().v_i1 < v_p) ++i;
		v_path.clear();
		v_path.push_back(i);
		v_path_p = i.f_index().v_i1;
		v_path_folded = v_p;
	}

public:
	t_creaser(T_rows& a_rows) : v_rows(a_rows)
	{
		f_reset();
	}
	void f_reset()
	{
		v_p = v_n = v_nesting_p = v_path_p = v_path_folded = 0;
		v_nesting.clear();
		v_nesting.push_back({{}});
		v_path.clear();
		v_path.push_back(v_rows.f_creases().f_begin());
	}
	size_t f_current() const
	{
		return v_p + v_n;
	}
	void f_push(size_t a_n)
	{
		v_n += a_n;
	}
	const T_tag& f_tag() const
	{
		return v_nesting.back().v_tag;
	}
	void f_open(const T_tag& a_tag)
	{
		f_plain();
		if (v_path_p > v_p || !v_path.back()->v_x)
			v_nesting.push_back({a_tag});
		else
			v_nesting.push_back({a_tag, v_path.back().f_delta().v_i1, v_path.back()->v_x->v_folded});
	}
	void f_close()
	{
		f_plain();
		auto& b0 = v_nesting.back();
		auto xs = std::move(b0.v_xs);
		if (xs.empty()) {
			v_nesting.pop_back();
		} else {
			size_t n = b0.v_n;
			bool folded = b0.v_folded;
			size_t dbegin = b0.v_dirty_begin;
			size_t dend = b0.v_dirty_end;
			v_nesting.pop_back();
			auto& b1 = v_nesting.back();
			b1.v_xs.emplace_back(std::move(xs));
			if (b1.v_xs.back().v_n == n)
				b1.v_xs.back().v_x->v_folded = folded;
			else
				folded = false;
			if (!folded) {
				b1.f_dirty_begin(dbegin);
				b1.f_dirty_end(dend);
			}
		}
		if (v_nesting.size() <= 1) f_foldable();
	}
	void f_flush()
	{
		f_plain();
		while (v_nesting.size() > 1) {
			auto& b0 = v_nesting.back();
			auto xs = std::move(b0.v_xs);
			size_t dbegin = b0.v_dirty_begin;
			size_t dend = b0.v_dirty_end;
			v_nesting.pop_back();
			auto& b1 = v_nesting.back();
			if (!xs.empty() && !xs.front().v_x && !b1.v_xs.empty() && !b1.v_xs.back().v_x) {
				b1.v_xs.back().v_n += xs.front().v_n;
				xs.pop_front();
			}
			b1.v_xs.insert(b1.v_xs.end(), xs.begin(), xs.end());
			b1.f_dirty_begin(dbegin);
			b1.f_dirty_end(dend);
		}
		f_foldable();
	}
};

}

#endif

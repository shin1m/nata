#ifndef NATA__FOLDER_H
#define NATA__FOLDER_H

#include "view.h"

namespace nata
{

template<typename T_rows, typename T_tag>
class t_folder
{
	struct t_block
	{
		T_tag v_tag;
		size_t v_n = 0;
		bool v_folded = false;
		std::deque<typename T_rows::t_foldings::t_span> v_xs;
	};

	T_rows& v_rows;
	size_t v_p;
	size_t v_n;
	std::deque<t_block> v_nesting;
	size_t v_nesting_p;
	std::vector<typename T_rows::t_foldings::t_iterator> v_path;
	size_t v_path_p;

	void f_plain()
	{
		if (v_n <= 0) return;
		v_nesting.back().v_xs.emplace_back(v_n);
		v_n = 0;
	}

public:
	t_folder(T_rows& a_rows) : v_rows(a_rows)
	{
	}
	void f_reset()
	{
		v_p = v_n = v_nesting_p = v_path_p = 0;
		v_nesting.clear();
		v_nesting.push_back({0});
		v_path.clear();
		v_path.push_back(v_rows.f_foldings().f_begin());
	}
	void f_push(size_t a_n)
	{
		v_p += a_n;
		v_n += a_n;
	}
	const T_tag& f_tag() const
	{
		return v_nesting.back().v_tag;
	}
	void f_open(const T_tag& a_tag)
	{
		f_plain();
		while (v_path_p < v_p) {
			while (v_path.back()->v_x) v_path.push_back(v_path.back()->v_x->v_nested.f_begin());
			v_path_p += v_path.back().f_delta().v_i1;
			v_rows.f_foldings().f_next(v_path);
		}
		if (v_path_p > v_p || !v_path.back()->v_x)
			v_nesting.push_back({a_tag});
		else
			v_nesting.push_back({a_tag, v_path.back().f_delta().v_i1, v_path.back()->v_x->v_folded});
	}
	void f_close()
	{
		f_plain();
		auto xs = std::move(v_nesting.back().v_xs);
		if (xs.empty()) {
			v_nesting.pop_back();
		} else {
			size_t n = v_nesting.back().v_n;
			bool folded = v_nesting.back().v_folded;
			v_nesting.pop_back();
			v_nesting.back().v_xs.emplace_back(std::move(xs));
			if (v_nesting.back().v_xs.back().v_n == n) v_nesting.back().v_xs.back().v_x->v_folded = folded;
		}
		if (v_nesting.size() > 1) return;
		auto i = v_rows.f_foldable(v_nesting_p, std::move(v_nesting.back().v_xs));
		v_nesting_p = v_p;
		while (i.f_index().v_i1 < v_p) ++i;
		v_path.clear();
		v_path.push_back(i);
		v_path_p = i.f_index().v_i1;
	}
	void f_flush()
	{
		f_plain();
		while (v_nesting.size() > 1) {
			auto xs = std::move(v_nesting.back().v_xs);
			v_nesting.pop_back();
			if (!xs.empty() && !xs.front().v_x && !v_nesting.back().v_xs.empty() && !v_nesting.back().v_xs.back().v_x) {
				v_nesting.back().v_xs.back().v_n += xs.front().v_n;
				xs.pop_front();
			}
			v_nesting.back().v_xs.insert(v_nesting.back().v_xs.end(), xs.begin(), xs.end());
		}
		v_rows.f_foldable(v_nesting_p, std::move(v_nesting.back().v_xs));
		v_nesting.clear();
		v_path.clear();
	}
};

}

#endif

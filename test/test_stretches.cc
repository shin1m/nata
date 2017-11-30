#include "stretches.h"
#include "test_spans.h"

int main(int argc, char* argv[])
{
	using t_span = nata::t_stretches<int, 5, 5>::t_span;
	{
		nata::t_stretches<int, 5, 5> stretches;
		auto i = stretches.f_replace(0, 0, {
			{0, 10},
			{1, 11},
			{2, 12}
		});
		f_assert_equals(stretches, {
			{0, 10},
			{1, 11},
			{2, 12}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		std::deque<t_span> xs{
			{0, 5},
			{1, 5}
		};
		stretches.f_replace(0, 0, std::deque<t_span>(xs));
		{
			auto i = stretches.f_replace(0, 0, {});
			f_assert_equals(stretches, xs);
			assert(i == stretches.f_begin());
		}
		{
			auto i = stretches.f_replace(5, 0, {});
			f_assert_equals(stretches, xs);
			assert(--i == stretches.f_begin());
		}
		{
			auto i = stretches.f_replace(10, 0, {});
			f_assert_equals(stretches, xs);
			assert(i == stretches.f_end());
		}
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(0, 2, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(1, 2, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(3, 2, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_end());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(0, 5, {});
		f_assert_equals(stretches, {});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5},
			{0, 5}
		});
		auto i = stretches.f_replace(0, 12, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5},
			{0, 5}
		});
		auto i = stretches.f_replace(2, 12, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5},
			{0, 5}
		});
		auto i = stretches.f_replace(3, 12, {});
		f_assert_equals(stretches, {
			{0, 3}
		});
		assert(i == stretches.f_end());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5},
			{0, 5}
		});
		auto i = stretches.f_replace(5, 5, {});
		f_assert_equals(stretches, {
			{0, 10}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5}
		});
		auto i = stretches.f_replace(0, 10, {
			{2, 5},
			{3, 5}
		});
		f_assert_equals(stretches, {
			{2, 5},
			{3, 5}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(0, 0, {
			{0, 5}
		});
		f_assert_equals(stretches, {
			{0, 10}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(2, 0, {
			{0, 5}
		});
		f_assert_equals(stretches, {
			{0, 10}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5}
		});
		auto i = stretches.f_replace(5, 0, {
			{0, 5}
		});
		f_assert_equals(stretches, {
			{0, 10}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5}
		});
		auto i = stretches.f_replace(4, 0, {
			{0, 2},
			{1, 2}
		});
		f_assert_equals(stretches, {
			{0, 6},
			{1, 2},
			{0, 1},
			{1, 5}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5}
		});
		auto i = stretches.f_replace(5, 0, {
			{0, 2},
			{1, 2}
		});
		f_assert_equals(stretches, {
			{0, 7},
			{1, 7}
		});
		assert(i == stretches.f_begin());
	}
	{
		nata::t_stretches<int, 5, 5> stretches;
		stretches.f_replace(0, 0, {
			{0, 5},
			{1, 5}
		});
		auto i = stretches.f_replace(6, 0, {
			{0, 2},
			{1, 2}
		});
		f_assert_equals(stretches, {
			{0, 5},
			{1, 1},
			{0, 2},
			{1, 6}
		});
		assert(----i == stretches.f_begin());
	}
	return 0;
}

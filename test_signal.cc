#include "signal.h"
#include <cassert>

int main(int argc, char* argv[])
{
	{
		nata::t_signal<> signal;
		size_t n = 0;
		{
			nata::t_slot<> slot([&]
			{
				++n;
			});;
			signal >> slot;
			signal();
			assert(n == 1);
		}
		signal();
		assert(n == 1);
	}
	{
		nata::t_signal<int> signal;
		size_t n = 0;
		nata::t_slot<int> slot([&](auto x)
		{
			n += x;
		});;
		signal >> slot;
		signal(1);
		assert(n == 1);
		signal(2);
		assert(n == 3);
	}
	{
		nata::t_signal<> signal;
		size_t n = 0;
		nata::t_slot<> slot([&]
		{
			++n;
		});;
		auto connection = signal >> slot;
		signal();
		assert(n == 1);
		delete connection;
		signal();
		assert(n == 1);
	}
	return 0;
}

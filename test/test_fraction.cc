#include <nata/fraction.h>
#include <cassert>

int main(int argc, char* argv[])
{
	nata::t_fraction zero;
	auto one = nata::t_fraction::c_ONE;
	assert(zero < one);
	auto x01 = nata::f_half(one - zero);
	assert(x01 == nata::t_fraction{{0b0100'0000}});
	assert(x01 + x01 == one);
	auto x011 = x01 + nata::f_half(one - x01);
	assert(x011 == nata::t_fraction{{0b0110'0000}});
	auto x0111 = x011 + nata::f_half(one - x011);
	assert(x0111 == nata::t_fraction{{0b0111'0000}});
	auto x01111 = x0111 + nata::f_half(one - x0111);
	assert(x01111 == nata::t_fraction{{0b0111'1000}});
	auto x011111 = x01111 + nata::f_half(one - x01111);
	assert(x011111 == nata::t_fraction{{0b0111'1100}});
	auto x0111111 = x011111 + nata::f_half(one - x011111);
	assert(x0111111 == nata::t_fraction{{0b0111'1110}});
	auto x01111111 = x0111111 + nata::f_half(one - x0111111);
	assert(x01111111 == nata::t_fraction{{0b0111'1111}});
	auto x011111111 = x01111111 + nata::f_half(one - x01111111);
	assert((x011111111 == nata::t_fraction{{0b0111'1111, 0b1000'0000}}));
	auto x0111111111 = x011111111 + nata::f_half(one - x011111111);
	assert((x0111111111 == nata::t_fraction{{0b0111'1111, 0b1100'0000}}));
	assert(
		(x01 - zero) +
		(x011 - x01) +
		(x0111 - x011) +
		(x01111 - x0111) +
		(x011111 - x01111) +
		(x0111111 - x011111) +
		(x01111111 - x0111111) +
		(x011111111 - x01111111) +
		(x0111111111 - x011111111) +
		(one - x0111111111) ==
		one
	);
	assert(-zero == zero);
	assert(one + -one == zero);
	assert(x0111111 + -x0111111 == zero);
	assert(x01111111 + -x01111111 == zero);
	assert(x011111111 + -x011111111 == zero);
	assert(x0111111111 + -x0111111111 == zero);
	return 0;
}

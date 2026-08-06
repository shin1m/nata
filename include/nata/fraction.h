#ifndef NATA__FRACTION_H
#define NATA__FRACTION_H

#include <vector>
#include <cstdint>

namespace nata
{

class t_fraction : std::vector<uint8_t>
{
	t_fraction& f_add(const t_fraction& a_x, auto a_add, auto a_less)
	{
		auto i = a_x.size();
		if (size() < i) resize(i);
		for (value_type carry = 0; i > 0;) {
			auto x = (*this)[--i];
			auto y = (*this)[i] = a_add(x, a_x[i] + carry);
			carry = a_less(y, x) || carry && y == x ? 1 : 0;
		}
		while (!empty() && !back()) pop_back();
		return *this;
	}

public:
	static const t_fraction c_ONE;

	using std::vector<value_type>::vector;
	bool operator==(const t_fraction&) const = default;
	auto operator<=>(const t_fraction&) const = default;
	t_fraction& operator+=(const t_fraction& a_x)
	{
		return f_add(a_x, std::plus<value_type>(), std::less<value_type>());
	}
	t_fraction& operator-=(const t_fraction& a_x)
	{
		return f_add(a_x, std::minus<value_type>(), std::greater<value_type>());
	}
	t_fraction& f_negate()
	{
		if (empty()) return *this;
		for (auto& x : *this) x = ~x;
		++back();
		return *this;
	}
	t_fraction& f_halve()
	{
		value_type carry = 0;
		for (auto& x : *this) {
			auto c = x << sizeof(value_type) * 8 - 1;
			x = carry + (x >> 1);
			carry = c;
		}
		if (carry) push_back(carry);
		return *this;
	}
};

inline const t_fraction t_fraction::c_ONE{{static_cast<value_type>(~(static_cast<value_type>(~0) >> 1))}};

inline t_fraction operator+(t_fraction&& a_x, const t_fraction& a_y)
{
	a_x += a_y;
	return std::move(a_x);
}

inline t_fraction operator+(const t_fraction& a_x, t_fraction&& a_y)
{
	return std::move(a_y) + a_x;
}

inline t_fraction operator+(t_fraction&& a_x, t_fraction&& a_y)
{
	return std::move(a_x) + a_y;
}

inline t_fraction operator+(const t_fraction& a_x, const t_fraction& a_y)
{
	return t_fraction(a_x) + a_y;
}

inline t_fraction operator-(t_fraction&& a_x, const t_fraction& a_y)
{
	a_x -= a_y;
	return std::move(a_x);
}

inline t_fraction operator-(const t_fraction& a_x, const t_fraction& a_y)
{
	return t_fraction{a_x} - a_y;
}

inline t_fraction operator-(t_fraction&& a_x)
{
	a_x.f_negate();
	return std::move(a_x);
}

inline t_fraction operator-(const t_fraction& a_x)
{
	return -t_fraction{a_x};
}

inline t_fraction f_half(t_fraction&& a_x)
{
	a_x.f_halve();
	return std::move(a_x);
}

inline t_fraction f_half(const t_fraction& a_x)
{
	return f_half(t_fraction(a_x));
}

}

#endif

#include "text.h"

namespace xemmaix::nata
{

void t_text::f_destroy()
{
	::nata::t_text<>::~t_text();
}

}

namespace xemmai
{

void t_type_of<xemmaix::nata::t_text>::f_define(t_library* a_library)
{
	using xemmaix::nata::t_text;
	t_define{a_library}
	(L"size"sv, t_member<size_t(*)(const t_text&), [](auto a_this)
	{
		return a_this.f_size();
	}>())
	(L"in_bytes"sv, t_member<size_t(*)(const t_text&, size_t), [](auto a_this, auto a_p)
	{
		return a_this.f_at(a_p).f_base().f_index().v_byte;
	}>())
	(L"in_text"sv, t_member<size_t(*)(const t_text&, size_t), [](auto a_this, auto a_p)
	{
		return a_this.f_base().f_at(a_p).f_index().v_character;
	}>())
	(L"slice"sv, t_member<t_object*(*)(const t_text&, size_t, size_t), [](auto a_this, auto a_p, auto a_n)
	{
		size_t n = a_this.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		a_n = std::min(a_n, n - a_p);
		return (a_n > 0) ? t_string::f_instantiate(a_n, [&](auto p)
		{
			return a_this.f_slice(a_p, a_n, p);
		}) : f_global()->f_string_empty();
	}>())
	(L"replace"sv, t_member<void(*)(t_text&, size_t, size_t, std::wstring_view), [](auto a_this, auto a_p, auto a_n, auto a_text)
	{
		size_t n = a_this.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		a_this.f_replace(a_p, std::min(a_n, n - a_p), a_text.begin(), a_text.end());
	}>())
	(L"lines"sv, t_member<size_t(t_text::*)() const, &t_text::f_lines>())
	(L"line_at"sv, t_member<t_object*(*)(t_library*, const t_text&, size_t), [](auto a_library, auto a_this, auto a_p)
	{
		if (a_p >= a_this.f_lines()) f_throw(L"out of range."sv);
		return t_text::f_line(a_library, a_this.::nata::t_text<>::f_lines().f_at(a_p));
	}>())
	(L"line_at_in_text"sv, t_member<t_object*(*)(t_library*, const t_text&, size_t), [](auto a_library, auto a_this, auto a_p)
	{
		if ((a_p > a_this.f_size())) f_throw(L"out of range."sv);
		return t_text::f_line(a_library, a_this.::nata::t_text<>::f_lines().f_at_in_text(a_p));
	}>())
	.f_derive<t_text, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_text>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<xemmaix::nata::t_text>::f_do(this, a_stack, a_n);
}

}

#include "text.h"

namespace xemmaix::nata
{

void t_text::f_destroy()
{
	v_marks.~t_marks();
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
		auto n = a_this.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		a_n = std::min(a_n, n - a_p);
		return (a_n > 0) ? t_string::f_instantiate(a_n, [&](auto p)
		{
			return a_this.f_slice(a_p, a_n, p);
		}) : f_global()->f_string_empty();
	}>())
	(L"replace"sv, t_member<void(*)(t_text&, size_t, size_t, std::wstring_view), [](auto a_this, auto a_p, auto a_n, auto a_text)
	{
		auto n = a_this.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		a_this.f_replace(a_p, std::min(a_n, n - a_p), a_text.begin(), a_text.end());
	}>())
	(L"lines"sv, t_member<size_t(t_text::*)() const, &t_text::f_lines>())
	(L"line_at"sv, t_member<t_object*(*)(t_library*, const t_text&, size_t), [](auto a_library, auto a_this, auto a_p)
	{
		if (a_p >= a_this.f_lines()) f_throw(L"out of range."sv);
		return t_text::f_line(a_library, a_this.::nata::t_text<>::f_lines().f_at(a_p));
	}>())
	(L"line_at_text"sv, t_member<t_object*(*)(t_library*, const t_text&, size_t), [](auto a_library, auto a_this, auto a_p)
	{
		if ((a_p > a_this.f_size())) f_throw(L"out of range."sv);
		return t_text::f_line(a_library, a_this.::nata::t_text<>::f_lines().f_at_text(a_p));
	}>())
	(L"marks_in_text_range"sv, t_member<t_object*(*)(const t_text&, size_t, size_t), [](auto a_this, auto a_p, auto a_n)
	{
		auto n = a_this.f_size();
		if ((a_p > n)) f_throw(L"out of range."sv);
		a_n = std::min(a_n, n + 1 - a_p);
		auto i = a_this.v_marks.f_first_at_text(a_p);
		auto j = a_this.v_marks.f_first_at_text(a_p + a_n);
		n = 0;
		for (auto k = i; k != j; ++k) if (!k.f_delta().v_i1) ++n;
		return t_tuple::f_instantiate(n, [&](auto& tuple)
		{
			for (auto p = &tuple[0]; i != j; ++i) if (!i.f_delta().v_i1) new(p++) t_svalue(i->v_x);
		});
	}>())
	(L"mark_of_key"sv, t_member<t_object*(*)(const t_text&, const nata::t_fraction&), [](auto a_this, auto a_key)
	{
		auto i = a_this.v_marks.f_at_key(a_key);
		return f_tuple(i == a_this.v_marks.f_end() ? t_pvalue{} : t_pvalue{i->v_x}, i.f_index().v_i1 - 1);
	}>())
	(L"mark_at_text_with"sv, t_member<nata::t_fraction(*)(t_text&, size_t, const t_pvalue&), [](auto a_this, auto a_p, auto a_mark)
	{
		if ((a_p > a_this.f_size())) f_throw(L"out of range."sv);
		return a_this.v_marks.f_insert(a_p, a_mark).f_index().v_i2;
	}>())
	(L"unmark_by_key"sv, t_member<void(*)(t_text&, const nata::t_fraction&), [](auto a_this, auto a_key)
	{
		a_this.v_marks.f_erase(a_this.v_marks.f_at_key(a_key));
	}>())
	.f_derive<t_text, xemmaix::nata::t_proxy>();
}

t_pvalue t_type_of<xemmaix::nata::t_text>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_object*(*)(t_type*), xemmaix::nata::t_proxy::f_new<xemmaix::nata::t_text>>::f_do(this, a_stack, a_n);
}

}

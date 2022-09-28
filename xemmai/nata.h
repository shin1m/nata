#ifndef XEMMAIX__NATA__NATA_H
#define XEMMAIX__NATA__NATA_H

#ifdef _WIN32
#ifndef XEMMAIX__NATA__EXPORT
#define XEMMAIX__NATA__EXPORT __declspec(dllimport)
#endif
#else
#define XEMMAIX__NATA__EXPORT
#endif

#include <xemmai/convert.h>
#include <xemmai/tuple.h>

namespace nata
{
}

namespace xemmaix::nata
{

using namespace ::nata;
using namespace xemmai;
using xemmai::t_slot;

class t_proxy;
class t_library;
class t_text;
class t_search;

class t_entry
{
protected:
	t_entry* v_previous;
	t_entry* v_next;

	t_entry() : v_previous(this), v_next(this)
	{
	}
	t_entry(t_entry* a_previous) : v_previous(a_previous), v_next(a_previous->v_next)
	{
		v_previous->v_next = v_next->v_previous = this;
	}

public:
	XEMMAIX__NATA__EXPORT virtual void f_dispose();
};

class t_session : public t_entry
{
	friend class t_proxy;

	static XEMMAI__PORTABLE__THREAD t_session* v_instance;

	t_library* v_library;

public:
#ifdef _WIN32
	static XEMMAIX__NATA__EXPORT t_session* f_instance();
#else
	static t_session* f_instance()
	{
		if (!v_instance) f_throw(L"must be inside main."sv);
		return v_instance;
	}
#endif

	t_session(t_library* a_library) : v_library(a_library)
	{
		if (v_instance) f_throw(L"already inside main."sv);
		v_instance = this;
	}
	~t_session()
	{
		while (v_next != this) v_next->f_dispose();
		v_instance = nullptr;
	}
	t_library* f_library() const
	{
		return v_library;
	}
};

class t_library : public xemmai::t_library
{
	t_slot_of<t_type> v_type_proxy;
	t_slot_of<t_type> v_type_line;
	t_slot_of<t_type> v_type_text;
	t_slot_of<t_type> v_type_span;
	t_slot_of<t_type> v_type_search;

public:
	using xemmai::t_library::t_library;
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		using t = t_type_of<typename t_fundamental<T>::t_type>;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
	t_type* f_type_line() const
	{
		return v_type_line;
	}
	t_type* f_type_span() const
	{
		return v_type_span;
	}
};

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_library, proxy)
XEMMAI__LIBRARY__TYPE(t_library, text)
XEMMAI__LIBRARY__TYPE(t_library, search)

inline void f__new(t_svalue* a_p)
{
}

template<typename T_x, typename... T_xs>
inline void f__new(t_svalue* a_p, T_x&& a_x, T_xs&&... a_xs)
{
	new(a_p) t_svalue(std::forward<T_x>(a_x));
	f__new(++a_p, std::forward<T_xs>(a_xs)...);
}

template<typename... T_xs>
inline t_object* f_new(t_type* a_type, T_xs&&... a_xs)
{
	auto p = f_engine()->f_allocate(sizeof(t_svalue) * sizeof...(a_xs));
	f__new(p->f_fields(0), std::forward<T_xs>(a_xs)...);
	p->f_be(a_type);
	return p;
}

}

#endif

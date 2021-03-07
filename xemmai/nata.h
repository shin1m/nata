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
class t_extension;
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

	t_extension* v_extension;

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

	t_session(t_extension* a_extension) : v_extension(a_extension)
	{
		if (v_instance) f_throw(L"already inside main."sv);
		v_instance = this;
	}
	~t_session()
	{
		while (v_next != this) v_next->f_dispose();
		v_instance = nullptr;
	}
	t_extension* f_extension() const
	{
		return v_extension;
	}
};

class t_extension : public xemmai::t_extension
{
	t_slot_of<t_type> v_type_proxy;
	t_slot_of<t_type> v_type_text;
	t_slot_of<t_type> v_type_search;

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_proxy>()
{
	return v_type_proxy;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_text>()
{
	return v_type_text;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_search>()
{
	return v_type_search;
}

}

#endif

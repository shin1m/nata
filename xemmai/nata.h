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

namespace nata
{
}

namespace xemmaix
{

namespace nata
{

using namespace ::nata;
using namespace xemmai;
using xemmai::t_slot;

class t_proxy;
class t_extension;
class t_text;
class t_view;
class t_overlay;
class t_overlay_iterator;
class t_search;

class t_entry
{
protected:
	t_entry* v_previous;
	t_entry* v_next;

	t_entry() : v_previous(this), v_next(this)
	{
	}
	t_entry(t_entry* a_previous)
	{
		v_previous = a_previous;
		v_next = v_previous->v_next;
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
		if (!v_instance) t_throwable::f_throw(L"must be inside main.");
		return v_instance;
	}
#endif

	t_session(t_extension* a_extension) : v_extension(a_extension)
	{
		if (v_instance) t_throwable::f_throw(L"already inside main.");
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
	template<typename T, typename T_super> friend class xemmai::t_define;

	t_slot v_type_proxy;
	t_slot v_type_text;
	t_slot v_type_view;
	t_slot v_type_overlay;
	t_slot v_type_overlay_iterator;
	t_slot v_type_search;

	template<typename T>
	void f_type__(t_scoped&& a_type);

public:
	t_extension(t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(T a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline void t_extension::f_type__<t_proxy>(t_scoped&& a_type)
{
	v_type_proxy = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_text>(t_scoped&& a_type)
{
	v_type_text = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_view>(t_scoped&& a_type)
{
	v_type_view = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_overlay>(t_scoped&& a_type)
{
	v_type_overlay = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_overlay_iterator>(t_scoped&& a_type)
{
	v_type_overlay_iterator = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_search>(t_scoped&& a_type)
{
	v_type_search = std::move(a_type);
}

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_object* t_extension::f_type<t_proxy>() const
{
	return v_type_proxy;
}

template<>
inline t_object* t_extension::f_type<t_text>() const
{
	return v_type_text;
}

template<>
inline t_object* t_extension::f_type<t_view>() const
{
	return v_type_view;
}

template<>
inline t_object* t_extension::f_type<t_overlay>() const
{
	return v_type_overlay;
}

template<>
inline t_object* t_extension::f_type<t_overlay_iterator>() const
{
	return v_type_overlay_iterator;
}

template<>
inline t_object* t_extension::f_type<t_search>() const
{
	return v_type_search;
}

}

}

#endif

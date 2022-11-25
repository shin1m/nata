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
	XEMMAI__LIBRARY__MEMBERS
	t_type* f_type_line() const
	{
		return v_type_line;
	}
	t_type* f_type_span() const
	{
		return v_type_span;
	}
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__TYPE(t_library, proxy)
XEMMAI__LIBRARY__TYPE(t_library, text)
XEMMAI__LIBRARY__TYPE(t_library, search)

}

#endif

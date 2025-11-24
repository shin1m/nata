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
class t_pattern;
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
	~t_entry()
	{
		v_previous->v_next = v_next;
		v_next->v_previous = v_previous;
	}
};

class t_session : public t_entry
{
	static XEMMAI__PORTABLE__THREAD t_session* v_instance;

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

	t_session()
	{
		if (v_instance) f_throw(L"already inside main."sv);
		v_instance = this;
	}
	~t_session();
};

class t_library : public xemmai::t_library
{
#define XEMMAIX__NATA__TYPES(_)\
	_(proxy)\
	_##_JUST(line)\
	_(text)\
	_##_JUST(span)\
	_(pattern)\
	_(search)
	XEMMAIX__NATA__TYPES(XEMMAI__TYPE__DECLARE)

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
#define XEMMAI__TYPE__LIBRARY t_library
XEMMAIX__NATA__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

}

#endif

#ifndef XEMMAIX__NATA__PROXY_H
#define XEMMAIX__NATA__PROXY_H

#include "nata.h"

namespace xemmaix::nata
{

class t_proxy : public t_entry
{
	t_session* v_session;
	t_scoped v_object;
	t_scoped v_owner;
	size_t v_n = 1;

protected:
	t_proxy(t_type* a_class) : t_entry(t_session::f_instance()), v_session(t_session::f_instance()), v_object(t_object::f_allocate(a_class, false)), v_owner(v_object)
	{
		v_object.f_pointer__(this);
	}
	virtual void f_destroy() = 0;

public:
	virtual ~t_proxy() = default;
	virtual void f_dispose();
	void f_acquire()
	{
		++v_n;
	}
	void f_release()
	{
		if (--v_n > 0) return;
		f_destroy();
		v_owner = nullptr;
	}
	bool f_valid() const
	{
		return v_session == t_session::f_instance();
	}
	t_object* f_object() const
	{
		return v_object;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_proxy> : t_underivable<t_bears<xemmaix::nata::t_proxy>>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = static_cast<T0*>(f_object(std::forward<T1>(a_object))->f_pointer());
			if (!p->f_valid()) f_throw(L"accessing from other thread."sv);
			if (!p->f_object()) f_throw(L"already disposed."sv);
			return p;
		}
	};
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *t_cast<typename t_fundamental<T0>::t_type>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			return reinterpret_cast<size_t>(f_object(std::forward<T1>(a_object))) == t_value::e_tag__NULL ? nullptr : t_cast<T0>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		return a_value->f_object();
	}

	typedef xemmaix::nata::t_extension t_extension;

	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	static void f_do_finalize(t_object* a_this);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif

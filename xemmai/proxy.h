#ifndef XEMMAIX__NATA__PROXY_H
#define XEMMAIX__NATA__PROXY_H

#include "nata.h"

namespace xemmaix::nata
{

class t_proxy : public t_entry
{
	t_session* v_session;
	t_root v_object;
	t_root v_owner;
	size_t v_n = 1;

protected:
	t_proxy() : t_entry(t_session::f_instance()), v_session(t_session::f_instance()), v_object(t_object::f_of(this)), v_owner(v_object)
	{
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
	bool f_disposed() const
	{
		return !v_object;
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::nata::t_proxy> : t_bears<xemmaix::nata::t_proxy>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0& f_call(T1&& a_object)
		{
			auto& p = f_object(std::forward<T1>(a_object))->template f_as<T0>();
			if (!p.f_valid()) f_throw(L"accessing from other thread."sv);
			if (p.f_disposed()) f_throw(L"already disposed."sv);
			return p;
		}
	};
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return t_cast<typename t_fundamental<T0>::t_type>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			return f_object(std::forward<T1>(a_object)) ? &t_cast<T0>::f_call(std::forward<T1>(a_object)) : nullptr;
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<uintptr_t>(p)) {
			case e_tag__NULL:
				return true;
			case e_tag__BOOLEAN:
			case e_tag__INTEGER:
			case e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T&& a_value)
	{
		return t_object::f_of(a_value);
	}

	typedef xemmaix::nata::t_library t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	static void f_do_finalize(t_object* a_this);
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

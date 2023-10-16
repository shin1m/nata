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
	template<typename T>
	static T& f_cast(auto&& a_object)
	{
		auto& p = static_cast<t_object*>(a_object)->f_as<T>();
		if (!p.f_valid()) f_throw(L"accessing from other thread."sv);
		if (p.f_disposed()) f_throw(L"already disposed."sv);
		return p;
	}
	template<typename T>
	struct t_cast
	{
		static T f_as(auto&& a_object)
		{
			return f_cast<typename t_fundamental<T>::t_type>(std::forward<decltype(a_object)>(a_object));
		}
		static bool f_is(t_object* a_object)
		{
			return reinterpret_cast<uintptr_t>(a_object) >= e_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<T>::t_type>();
		}
	};
	template<typename T>
	struct t_cast<T*>
	{
		static T* f_as(auto&& a_object)
		{
			return static_cast<t_object*>(a_object) ? &f_cast<T>(std::forward<decltype(a_object)>(a_object)) : nullptr;
		}
		static bool f_is(t_object* a_object)
		{
			return reinterpret_cast<uintptr_t>(a_object) == e_tag__NULL || reinterpret_cast<uintptr_t>(a_object) >= e_tag__OBJECT && a_object->f_type()->f_derives<typename t_fundamental<T>::t_type>();
		}
	};

	static t_pvalue f_transfer(auto* a_library, auto&& a_value)
	{
		return t_object::f_of(a_value);
	}

	using t_library = xemmaix::nata::t_library;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	static void f_do_finalize(t_object* a_this);
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif

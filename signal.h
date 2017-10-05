#ifndef NATA__SIGNAL_H
#define NATA__SIGNAL_H

#include <functional>
#include <list>

namespace nata
{

template<typename T_slot>
struct t_connection
{
	class t_owner
	{
		friend class t_connection<T_slot>;

	protected:
		std::list<t_connection<T_slot>*> v_connections;

		~t_owner()
		{
			while (!v_connections.empty()) delete v_connections.front();
		}
	};
	template<typename T>
	friend t_connection<T>* operator>>(typename t_connection<T>::t_owner&, T&);

	t_owner& v_signal;
	T_slot& v_slot;

	~t_connection()
	{
		v_signal.v_connections.remove(this);
		v_slot.v_connections.remove(this);
	}

private:
	t_connection(t_owner& a_signal, T_slot& a_slot) : v_signal(a_signal), v_slot(a_slot)
	{
		v_signal.v_connections.push_back(this);
		v_slot.v_connections.push_back(this);
	}
};

template<typename... T_an>
struct t_slot : t_connection<t_slot<T_an...>>::t_owner, std::function<void(T_an...)>
{
	using std::function<void(T_an...)>::function;
};

template<typename... T_an>
struct t_signal : t_connection<t_slot<T_an...>>::t_owner
{
	void operator()(T_an... a_an) const
	{
		for (auto x : this->v_connections) x->v_slot(std::forward<T_an>(a_an)...);
	}
};

template<typename T_slot>
inline t_connection<T_slot>* operator>>(typename t_connection<T_slot>::t_owner& a_signal, T_slot& a_slot)
{
	return new t_connection<T_slot>(a_signal, a_slot);
}

}

#endif

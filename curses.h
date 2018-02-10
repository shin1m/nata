#ifndef NATA__CURSES_H
#define NATA__CURSES_H

#include "signal.h"
#include <ncurses.h>

namespace nata
{

namespace curses
{

struct t_session
{
	t_session()
	{
		std::setlocale(LC_ALL, "");
		initscr();
		start_color();
		use_default_colors();
		raw();
		noecho();
		nonl();
	}
	~t_session()
	{
		endwin();
	}
};

class t_window
{
	friend class t_graphics;

protected:
	WINDOW* v_window;

private:
	size_t v_width;
	size_t v_height;

public:
	t_signal<> v_resized;

	t_window(size_t a_x, size_t a_y, size_t a_width, size_t a_height) : v_window(newwin(a_height, a_width, a_y, a_x)), v_width(a_width), v_height(a_height)
	{
		idlok(v_window, TRUE);
		intrflush(v_window, FALSE);
		keypad(v_window, TRUE);
	}
	~t_window()
	{
		delwin(v_window);
	}
	size_t f_width() const
	{
		return v_width;
	}
	size_t f_height() const
	{
		return v_height;
	}
	void f_move(size_t a_x, size_t a_y, size_t a_width, size_t a_height)
	{
		if (a_width != v_width || a_height != v_height) {
			wresize(v_window, a_height, a_width);
			v_width = a_width;
			v_height = a_height;
			v_resized();
		}
		mvwin(v_window, a_y, a_x);
	}
	void f_scroll(size_t a_y, size_t a_height, int a_delta)
	{
		scrollok(v_window, TRUE);
		wsetscrreg(v_window, a_y, a_height - 1);
		wscrl(v_window, a_delta);
		wsetscrreg(v_window, 0, v_height);
		scrollok(v_window, FALSE);
	}
	void f_cursor(size_t a_x, size_t a_y)
	{
		wmove(v_window, a_y, a_x);
	}
	void f_timeout(int a_delay)
	{
		wtimeout(v_window, a_delay);
	}
	int f_get(wint_t& a_c)
	{
		return wget_wch(v_window, &a_c);
	}
};

struct t_target : t_window
{
	typedef attr_t t_attribute;

	attr_t v_attribute_control = A_NORMAL;
	attr_t v_attribute_folded = A_NORMAL;
	wchar_t v_prefix = L'\0';

	using t_window::t_window;
	size_t f_width() const
	{
		return t_window::f_width() - 1;
	}
	std::tuple<size_t, size_t, size_t> f_size(wchar_t a_c, attr_t a_a) const
	{
		return {wcwidth(a_c), 1, 0};
	}
	std::tuple<size_t, size_t, size_t> f_tab(size_t a_x, attr_t a_a) const
	{
		return {std::max(std::min((a_x + 8) / 8 * 8, f_width()) - a_x, size_t(1)), 1, 0};
	}
	std::tuple<size_t, size_t, size_t> f_eol(attr_t a_a) const
	{
		return {1, 1, 0};
	}
	std::tuple<size_t, size_t, size_t> f_eof() const
	{
		return {1, 1, 0};
	}
	std::tuple<size_t, size_t, size_t> f_folded() const
	{
		return {3, 1, 0};
	}
	void f_cursor(size_t a_x, size_t a_y)
	{
		t_window::f_cursor(a_x + 1, a_y);
	}
	void f_attributes(attr_t a_control, attr_t a_folded)
	{
		v_attribute_control = a_control;
		v_attribute_folded = a_folded;
	}
};

struct t_graphics
{
	t_target& v_target;
	const wchar_t v_prefix;
	size_t v_x;
	cchar_t v_c{A_NORMAL, L" "};
	attr_t v_overlay;

	t_graphics(t_target& a_target) : v_target(a_target), v_prefix(L'0' + v_target.v_prefix)
	{
		v_target.v_prefix = (v_target.v_prefix + 1) % 10;
	}
	~t_graphics()
	{
		wnoutrefresh(v_target.v_window);
	}
	void f_move(size_t a_y)
	{
		v_x = 0;
		wmove(v_target.v_window, a_y, 0);
		cchar_t cc{v_target.v_attribute_control, v_prefix};
		wadd_wch(v_target.v_window, &cc);
	}
	void f_attribute(attr_t a_value)
	{
		v_c.attr = a_value;
		v_overlay = A_NORMAL;
	}
	void f_overlay(attr_t a_value)
	{
		v_c.attr = v_overlay = a_value;
	}
	void f_put(wchar_t a_c)
	{
		v_c.chars[0] = a_c;
		wadd_wch(v_target.v_window, &v_c);
		v_x += std::get<0>(v_target.f_size(a_c, v_c.attr));
	}
	void f_tab()
	{
		cchar_t cc{v_overlay == A_NORMAL ? v_target.v_attribute_control : v_overlay, L"|"};
		wadd_wch(v_target.v_window, &cc);
		size_t w = std::get<0>(v_target.f_tab(v_x++, v_c.attr));
		if (--w <= 0) return;
		cc.chars[0] = L' ';
		do {
			wadd_wch(v_target.v_window, &cc);
			++v_x;
		} while (--w > 0);
	}
	void f_wrap()
	{
		if (v_x < v_target.f_width()) wclrtoeol(v_target.v_window);
	}
	void f_eol()
	{
		cchar_t cc{v_overlay == A_NORMAL ? v_target.v_attribute_control : v_overlay, L"/"};
		wadd_wch(v_target.v_window, &cc);
		v_x += std::get<0>(v_target.f_eol(v_c.attr));
		f_wrap();
	}
	void f_eof()
	{
		cchar_t cc{v_target.v_attribute_control, L"<"};
		wadd_wch(v_target.v_window, &cc);
		v_x += std::get<0>(v_target.f_eof());
		f_wrap();
	}
	void f_folded()
	{
		cchar_t cc{v_overlay == A_NORMAL ? v_target.v_attribute_folded : v_overlay, L"."};
		for (size_t i = 0; i < 3; ++i) wadd_wch(v_target.v_window, &cc);
	}
	void f_empty(size_t a_y)
	{
		wmove(v_target.v_window, a_y, 0);
		cchar_t cc{v_target.v_attribute_control, v_prefix};
		wadd_wch(v_target.v_window, &cc);
		wclrtoeol(v_target.v_window);
	}
};

}

}

#endif

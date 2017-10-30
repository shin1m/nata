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
		idlok(stdscr, TRUE);
		scrollok(stdscr, TRUE);
		intrflush(stdscr, FALSE);
		keypad(stdscr, TRUE);
	}
	~t_session()
	{
		endwin();
	}
};

struct t_target
{
	t_signal<> v_resized;

	size_t f_width() const
	{
		return COLS;
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
	void f_scroll(size_t a_y, size_t a_height, int a_delta)
	{
		setscrreg(a_y, a_height - 1);
		scrl(a_delta);
		setscrreg(0, LINES - 1);
	}
};

struct t_graphics
{
	const t_target& v_target;
	const attr_t v_attribute_control;
	size_t v_to;
	size_t v_x = 0;
	cchar_t v_c{A_NORMAL, L" "};

	void f_move(size_t a_y)
	{
		v_x = 0;
		move(v_to + a_y, 0);
	}
	void f_attribute(attr_t a_a)
	{
		v_c.attr = a_a;
	}
	void f_put(wchar_t a_c)
	{
		v_c.chars[0] = a_c;
		add_wch(&v_c);
		v_x += std::get<0>(v_target.f_size(a_c, v_c.attr));
	}
	void f_tab()
	{
		size_t w = std::get<0>(v_target.f_tab(v_x, v_c.attr));
		if (v_x % 8 == 0) {
			cchar_t cc{v_attribute_control, L"|"};
			add_wch(&cc);
			++v_x;
			--w;
		}
		v_c.chars[0] = L' ';
		while (w > 0) {
			add_wch(&v_c);
			++v_x;
			--w;
		}
	}
	void f_wrap()
	{
		clrtoeol();
	}
	void f_eol()
	{
		cchar_t cc{v_attribute_control, L"/"};
		add_wch(&cc);
		clrtoeol();
	}
	void f_eof()
	{
		cchar_t cc{v_attribute_control, L"<"};
		add_wch(&cc);
		clrtoeol();
	}
	void f_empty(size_t a_y)
	{
		move(v_to + a_y, 0);
		clrtoeol();
	}
};

}

}

#endif

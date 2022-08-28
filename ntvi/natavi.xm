nata = Module("nata"

Session = Object + @
	$logs
	$undos
	$redos
	$__initialize = @
		$undos = [
		$redos = [
	$begin = @ $logs = [
	$log = @(x) $logs.push(x
	$commit = @(message)
		$redos.clear(
		$undos.push('(message, $logs
		$logs = null
	$replay = @(message, logs)
		$logs = [
		while logs.size() > 0
			logs.pop()(
		e = '(message, $logs
		$logs = null
		e
	$cancel = @ $replay(null, $logs
	$undo = @ $redos.push($replay(*$undos.pop(
	$redo = @ $undos.push($replay(*$redos.pop(

with = @(x, f)
	try
		f(x
	finally
		x.dispose(

TextSession = Session + @
	Delta = Object + @
		$x
		$xs
		$__initialize = @(x, xs)
			$x = x
			$xs = xs
		$__call = @ $x.replace(*$xs

	$text
	$__initialize = @(text)
		Session.__initialize[$](
		$text = text
	$replace = @(p, n, s)
		t = $text.slice(p, n
		$log(Delta($, '(p, s.size(), t
		$text.replace(p, n, s
		t
	$merge = @(p, n, s)
		m = $logs.size()
		m <= 0 && return $replace(p, n, s
		e = $logs[m - 1]
		e.@ !== Delta && return $replace(p, n, s
		p0 = e.xs[0]
		q0 = p0 + e.xs[1]
		q = p + n
		q < p0 || p > q0 && return $replace(p, n, s
		if q > q0
			x = s.size()
			y = e.xs[2] + $text.slice(q0, q - q0
		else
			x = s.size() + q0 - q
			y = e.xs[2]
		e.xs = p < p0 ? '(p, x, $text.slice(p, p0 - p) + y) : '(p0, p - p0 + x, y)
		$text.replace(p, n, s

letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")
merge = @(x, y)
	x.each(@(k, v) y[k] = v
	y

$new = @(host, path)
	text = host.text(
	path !== null && host.read(text, path
	view = host.main_view(text
	selection = host.selection(view
	status = host.text(
	strip = host.strip_view(status
	session = TextSession(text
	move_to = @(p)
		view.folded(p, false
		view.position__(p, false
	log_position = @(p) session.log(@
		move_to(p
		session.log(@ log_position(p
	begin = @(p)
		session.begin(
		log_position(p
	commit = @ if session.logs !== null
		p = view.position().text
		session.log(@ log_position(p
		session.commit("edit"
	count = 0
	times = @(f) for n = count > 0 ? count : 1; n > 0; n = n - 1: f(
	forward_n = @(x, limit, n, f)
		y = x + n
		y > limit && (y = limit)
		y > x && f(y
	forward = @(x, limit, f) forward_n(x, limit, count > 0 ? count : 1, f
	backward = @(x, limit, f)
		n = count > 0 ? count : 1
		y = x > limit + n ? x - n : limit
		y < x && f(y
	current = view
	input = [
	message = ""
	mode = mode_insert = mode_visual = mode_prompt = null
	show_prompt = @(prompt, doing, done)
		status.replace(0, -1, prompt
		:current = strip
		mode_prompt.caller = mode
		:mode = mode_prompt
		mode.prompt = prompt
		mode.doing = doing
		mode.done = done
	with_search = @(pattern, f) with(nata.Search(text), @(search)
		try
			search.pattern(pattern, nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
			f(search
		catch Throwable t
			'(
	search_forward = @(pattern, p) with_search(pattern, @(search)
		f = @(p)
			search.reset(p, -1
			search.next(
		match = p < text.size() ? f(p + 1) : '()
		match.size() > 0 ? match : f(0)
	search_backward = @(pattern, p) with_search(pattern, @(search)
		f = @(p)
			search.reset(0, -1
			last = '(
			while true
				m = search.next(
				(m.size() <= 0 || m[0].from > p) && return last
				last = m
		match = p > 0 ? f(p - 1) : '()
		match.size() > 0 ? match : f(text.size())
	last_pattern = null
	last_reverse = false
	search_message = @(reverse, p, match)
		if match.size() <= 0
			:message = "not found: " + last_pattern
		else if reverse
			:message = (match[0].from < p ? "?" : "continuing at BOTTOM: ") + last_pattern
		else
			:message = (match[0].from > p ? "/" : "continuing at TOP: ") + last_pattern
	search_next = @(reverse)
		if last_pattern === null
			:message = "no last pattern"
			$reset(
			return
		reverse = last_reverse ^ reverse
		p = view.position().text
		match = reverse ? search_backward(last_pattern, p) : search_forward(last_pattern, p)
		search_message(reverse, p, match
		if match.size() > 0
			$finish(@ move_to(match[0].from
		else
			$reset(
	search = @(reverse)
		p = view.position().text
		pattern = ""
		match = '(
		paint = @(selected) if match.size() > 0
			m = match[0]
			selection.paint(m.from, m.count, selected
		show_prompt(reverse ? "?" : "/"
			@(text)
				:pattern = text
				paint(false
				:match = text.size() > 0 ? reverse ? search_backward(text, p) : search_forward(text, p) : '()
				paint(true
				if match.size() > 0
					move_to(match[0].from
				else
					view.position__(p, false
			@(ok)
				if ok
					::last_reverse = reverse
					if pattern.size() > 0
						paint(false
						::last_pattern = pattern
						search_message(reverse, p, match
						if match.size() > 0
							:$finish(@
						else
							:$reset(
					else
						search_next[:$](false
				else
					:$reset(
	map_motion = {
		control("H"): @ $finish(@ backward(view.position().text, 0, @(p) view.position__(p, false
		letter(" "): @ $finish(@ forward(view.position().text, text.size(), @(p) view.position__(p, true
		letter("$"): @ $finish(@
			view.target__(-1
			count > 1 && forward_n(view.row().line, view.size().line, count - 1, view.line__
		letter("/"): @ search[$](false
		letter("0"): @
			if count > 0
				:count = count * 10
			else
				view.position__(text.line_at_in_text(view.position().text).from, false
		letter("1"): @ :count = count * 10 + 1
		letter("2"): @ :count = count * 10 + 2
		letter("3"): @ :count = count * 10 + 3
		letter("4"): @ :count = count * 10 + 4
		letter("5"): @ :count = count * 10 + 5
		letter("6"): @ :count = count * 10 + 6
		letter("7"): @ :count = count * 10 + 7
		letter("8"): @ :count = count * 10 + 8
		letter("9"): @ :count = count * 10 + 9
		letter("?"): @ search[$](true
		letter("N"): @ search_next[$](true
		letter("h"): @ $finish(@
			p = view.position().text
			backward(p, text.line_at_in_text(p).from, @(p) view.position__(p, false
		letter("j"): @ $finish(@ forward(view.row().line, view.size().line - 1, view.line__
		letter("k"): @ $finish(@ backward(view.row().line, 0, view.line__
		letter("l"): @ $finish(@
			p = view.position().text
			l = text.line_at_in_text(p
			forward(p, l.from + l.count - 1, @(p) view.position__(p, true
		letter("n"): @ search_next[$](false
	repeat = @(n, start, end, f)
		for ; n > 1; n = n - 1
			for i = start; i < end; i = i + 1
				f(input[i]
	render_status = @
		if message != ""
			status.replace(0, -1, message
		else
			position = view.position(
			line = text.line_at_in_text(position.text
			n = view.range(
			i = ""
			input.each(@(x) :i = i + String.from_code(x)
			status.replace(0, -1, mode.name + " " +
				line.index + "," +
				(position.text - line.from) + "-" +
				(position.x - view.row().x) + " " +
				(n > 0 ? view.top() * 100 / n : 100) + "% <" +
				session.undos.size() +
				(session.logs === null ? "" : "?" + session.logs.size()) +
				(session.redos.size() > 0 ? "|" + session.redos.size() : "") + "> " + i
	Mode = Object + @
		$post
		$map
		$__initialize = @
			$post = $reset
			$map = $map_default
		$render = @ render_status(
		$reset = @
			::count = 0
			::input = [
			$post = $reset
			$map = $map_default
		$finish = @(f)
			try
				f[$](
			catch Throwable t
				::message = t.__string(
			::count = 0
			$post[$](
		$__call = @(c)
			try
				$map[c][$](
			catch Throwable t
				$reset(
	insert = @
		:mode = mode_insert
		mode.start = input.size(
	register = '(false, ""
	yank = @(p, q, line = false) $finish(@ ::register = '(line, text.slice(p, q - p
	delete_and_edit = @(p, q, line) :register = '(line, session.replace(p, q - p, ""
	replace = @(p, q, line = false)
		delete_and_edit(p, q, line
		insert(
	delete = @(p, q, line = false) $commit(@
		delete_and_edit(p, q, line
		commit(
	mode = mode_normal = (Mode + @
		map_Z = {
			letter("Z"): @ host.quit(
		$for_lines = @(f)
			$post = $reset
			l0 = text.line_at_in_text(view.position().text
			l = l0.index + (count > 0 ? count : count0 > 0 ? count0 : 1) - 1
			l >= text.lines() && (l = text.lines() - 1)
			l1 = l > l0.index ? text.line_at(l) : l0
			f(l0.from, l1.from + l1.count
		map_c = merge(map_motion, {
			letter("c"): @ $for_lines(@(p, q)
				begin(p
				replace(p, q - 1, true
		map_d = merge(map_motion, {
			letter("d"): @ $for_lines(@(p, q)
				begin(p
				delete[:$](p, q, true
		map_y = merge(map_motion, {
			letter("y"): @ $for_lines(@(p, q) yank[:$](p, q, true
		$for_motion = @(begin, f)
			:count0 = ::count
			start = input.size(
			::count = 0
			p0 = view.position().text
			$post = @
				$post = @
				repeat(count0, start, input.size(), mode
				$post = $reset
				p1 = view.position().text
				begin(p0
				if p1 < p0
					f(p1, p0
				else
					f(p0, p1
		$map_default = merge(map_motion, {
			control("R"): @ $finish(@ times(@ session.redo(
			letter("."): @
				input.pop(
				last = $last_input
				c = last.shift(
				if count > 0: while c >= 0x30 && c <= 0x39: c = last.shift(
				for ;; c = last.shift()
					input.push(c
					mode(c
					last.size() > 0 || break
			letter("Z"): @ $map = map_Z
			letter("c"): @
				$for_motion(begin, replace
				$map = map_c
			letter("d"): @
				$for_motion(begin, delete[$]
				$map = map_d
			letter("i"): insert
			letter("p"): @ $commit(@
				p = view.position().text
				begin(p
				register[0] && (p = text.line_at_in_text(p).from)
				times(@ session.merge(p, 0, register[1]
				commit(
			letter("u"): @ $finish(@ times(@ session.undo(
			letter("v"): @
				::count = 0
				::input = [
				::mode = mode_visual
				mode.base = view.position().text
			letter("y"): @
				$for_motion(@(p) view.position__(p, false), yank[$]
				$map = map_y
		$name = "NORMAL"
		$last_input
		$__initialize = @
			Mode.__initialize[$](
			$last_input = [
		$commit = @(f)
			$last_input = input
			$finish(f
	)(
	mode_insert = (Object + @
		backspace = @
			p = view.position().text
			if p > 0
				view.folded(p - 1, false
				session.logs === null && begin(p
				session.merge(p - 1, 1, ""
		map = {
			control("H"): backspace
			control("["): @
				repeat(count, $start, input.size() - 1, $
				::mode = mode_normal
				mode.commit(commit
			host.KEY_BACKSPACE: backspace
		$name = "INSERT"
		$start
		$render = @ render_status(
		$__call = @(c)
			try
				map[c][$](
			catch Throwable t
				(c == host.KEY_ENTER || c == 0xd) && (c = 0xa)
				p = view.position().text
				session.logs === null && begin(p
				session.merge(p, 0, String.from_code(c
	)(
	mode_visual = (Mode + @
		each = @(overlay, f)
			with(host.overlay_iterator(overlay), @(i)
				while (x = i.next()) !== null
					x.value && f(x.from, x.count
		clear = @
			xs = [
			each(selection, @(p, n) xs.push('(p, n
			xs.each(@(x) selection.paint(x[0], x[1], false
			xs
		for_selection = @(c, f)
			xs = clear(
			x = xs.size() > 0 ? xs[0] : '(0, 0
			s = "v" + x[1] + " " + c
			::input = [
			for i = 0; i < s.size(); i = i + 1: input.push(s.code_at(i
			::count = 0
			f(x[0], x[0] + x[1]
		$map_default = merge(map_motion, {
			control("["): @
				clear(
				::mode = mode_normal
				mode.finish(@
			letter("c"): @ for_selection("c", @(p, q)
				begin(:$base
				replace(p, q
			letter("d"): @ for_selection("d", @(p, q)
				begin(:$base
				:::mode = mode_normal
				delete[mode](p, q
			letter("y"): @ for_selection("y", @(p, q)
				view.position__(:$base, false
				:::mode = mode_normal
				yank[mode](p, q
		$name = "VISUAL"
		$base
		$reset = @
			clear(
			position = view.position().text
			if position < $base
				selection.paint(position, $base - position, true
			else
				selection.paint($base, position - $base, true
			::count = 0
			::input = [
			$map = $map_default
	)(
	mode_prompt = (Object + @
		backspace = @
			n = status.size(
			if n > $prompt.size()
				status.replace(n - 1, 1, ""
				$doing(status.slice($prompt.size(), -1
			else
				done[$](false
		done = @(ok)
			::current = view
			::mode = $caller
			$done(ok
		map = {
			control("H"): backspace
			control("M"): @ done[$](true
			control("["): @
				$doing(""
				done[$](false
			host.KEY_BACKSPACE: backspace
			host.KEY_ENTER: @ done[$](true
		$caller
		$prompt
		$doing
		$done
		$render = @
		$__call = @(c)
			try
				map[c][$](
			catch Throwable t
				status.replace(status.size(), 0, String.from_code(c
				$doing(status.slice($prompt.size(), -1
	)(
	(Object + @
		$render = @
			view.into_view(view.position().text
			mode.render(
		$main = @ view
		$strip = @ strip
		$current = @ current
		$__call = @(c)
			input.push(c
			mode(c
		$message = @(x) ::message = x
	)(

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

$Buffer = Buffer = Object + @
	$path
	$session
	$view
	$selection
	$commands
	$__initialize = @(path, text, view, selection)
		$path = path
		$session = TextSession(text
		$view = view
		$selection = selection
		$commands = {
	$dispose = @
		$selection.dispose(
		$view.dispose(
		$session.text.dispose(

KeyMap = Object + @
	$action
	$base
	$map
	$__initialize = @(action, base = null, map = null)
		$action = action
		$base = base
		$map = map === null ? {} : map
	$__get_at = @(c)
		try
			$map[c]
		catch Throwable t
			$base[c]
	$get = @ $action !== null ? $action : $base !== null ? $base.get() : null
	$more = @ $map.size() > 0 || $base !== null && $base.more()
	$list = @
		list = [
		f = @(map, lhs)
			entries = [
			map.map.each(@(k, v) entries.push('(k, v
			entries.sort(@(x, y) x[0] - y[0]
			entries.each(@(x)
				s = lhs + String.from_code(x[0])
				x[1].action === null || list.push('(s, x[1].action
				f(x[1], s
		f($, ""
		list
	$put = @(cs, action)
		map = $
		n = cs.size(
		for i = 0; i < n; i = i + 1
			c = cs.code_at(i
			try
				map = map.map[c]
			catch Throwable t
				try
					base = map.base[c]
				catch Throwable t
					base = null
				map = map.map[c] = KeyMap(null, base
		map.action = action
	$remove = @(cs)
		map = $
		path = [
		n = cs.size(
		for i = 0; i < n; i = i + 1
			c = cs.code_at(i
			try
				path.push('(map, c
				map = map.map[c]
			catch Throwable t
				return
		map.action = null
		while map.map.size() <= 0 && path.size() > 0
			parent = path.pop(
			map = parent[0]
			map.map.remove(parent[1]
			map.action === null || break

KeyMapAction = Object + @
	$name
	$action
	$__initialize = @(name, action)
		$name = name
		$action = action
	$__string = @ $name
	$__get_at = @(mode) $action[mode]

letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")
to_codes = @(s)
	cs = [
	n = s.size(
	for i = 0; i < n; i = i + 1: cs.push(s.code_at(i
	cs

with_search = @(text, pattern, f) with(nata.Search(text), @(search)
	try
		search.pattern(pattern, nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		f(search
	catch Throwable t
		'(

$new = @(host, status, strip, path)
	buffers = [
	add_buffer = @(path)
		buffer = host.buffer(path
		buffers.push(buffer
		buffer
	remove_buffer = @(i) buffers.remove(i).dispose(
	switch_buffer = @(buffer)
		:buffer = buffer
		:session = buffer.session
		:text = session.text
		:current = :view = buffer.view
		:selection = buffer.selection
		buffer
	switch_buffer(add_buffer(path
	open_buffer = @(path)
		n = buffers.size(
		for i = 0;; i = i + 1
			i < n || return switch_buffer(add_buffer(path
			x = buffers[i]
			x.path == path && break switch_buffer(x
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
	message = progress = ""
	show_prompt = @(prompt, doing, done)
		status.replace(0, -1, prompt
		:current = strip
		mode_prompt.caller = mode
		:mode = mode_prompt
		mode.prompt = prompt
		mode.doing = doing
		mode.done = done
	search_forward = @(pattern, p) with_search(text, pattern, @(search)
		f = @(p)
			search.reset(p, -1
			search.next(
		match = p < text.size() ? f(p + 1) : '()
		match.size() > 0 ? match : f(0)
	search_backward = @(pattern, p) with_search(text, pattern, @(search)
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
		match.size() > 0 || throw Throwable("not found: " + last_pattern
		:message = (reverse ?
			(match[0].from < p ? "?" : "continuing at BOTTOM: ") :
			(match[0].from > p ? "/" : "continuing at TOP: ")
		) + last_pattern
	search_next = @(reverse)
		last_pattern === null && throw Throwable("no last pattern"
		reverse = last_reverse ^ reverse
		p = view.position().text
		match = reverse ? search_backward(last_pattern, p) : search_forward(last_pattern, p)
		search_message(reverse, p, match
		move_to(match[0].from
	search = @(reverse)
		p = view.position().text
		pattern = ""
		match = '(
		paint = @(selected) if match.size() > 0
			m = match[0]
			selection.paint(m.from, m.count, selected
		show_prompt(reverse ? "?" : "/"
			@
				:pattern = status.slice(1, -1
				paint(false
				:match = pattern.size() > 0 ? reverse ? search_backward(pattern, p) : search_forward(pattern, p) : '()
				paint(true
				if match.size() > 0
					move_to(match[0].from
				else
					view.position__(p, false
			@(ok) !ok ? :$reset() : :$finish(@
				:::last_reverse = reverse
				pattern.size() > 0 || return search_next[::$](false
				paint(false
				:::last_pattern = pattern
				search_message(reverse, p, match
	map_motion = {
		control("H"): KeyMap(@ $finish(@ backward(view.position().text, 0, @(p) view.position__(p, false
		letter(" "): KeyMap(@ $finish(@ forward(view.position().text, text.size(), @(p) view.position__(p, true
		letter("$"): KeyMap(@ $finish(@
			view.target__(-1
			count > 1 && forward_n(view.row().line, view.size().line, count - 1, view.line__
		letter("/"): KeyMap(@ search[$](false
		letter("0"): KeyMap(@
			if count > 0
				:count = count * 10
			else
				$finish(@ view.position__(text.line_at_in_text(view.position().text).from, false
		letter("1"): KeyMap(@ :count = count * 10 + 1
		letter("2"): KeyMap(@ :count = count * 10 + 2
		letter("3"): KeyMap(@ :count = count * 10 + 3
		letter("4"): KeyMap(@ :count = count * 10 + 4
		letter("5"): KeyMap(@ :count = count * 10 + 5
		letter("6"): KeyMap(@ :count = count * 10 + 6
		letter("7"): KeyMap(@ :count = count * 10 + 7
		letter("8"): KeyMap(@ :count = count * 10 + 8
		letter("9"): KeyMap(@ :count = count * 10 + 9
		letter("?"): KeyMap(@ search[$](true
		letter("N"): KeyMap(@ $finish(@ search_next[:$](true
		letter("h"): KeyMap(@ $finish(@
			p = view.position().text
			backward(p, text.line_at_in_text(p).from, @(p) view.position__(p, false
		letter("j"): KeyMap(@ $finish(@ forward(view.row().line, view.size().line - 1, view.line__
		letter("k"): KeyMap(@ $finish(@ backward(view.row().line, 0, view.line__
		letter("l"): KeyMap(@ $finish(@
			p = view.position().text
			l = text.line_at_in_text(p
			forward(p, l.from + l.count - 1, @(p) view.position__(p, true
		letter("n"): KeyMap(@ $finish(@ search_next[:$](false
	repeat = @(n, start, end, f)
		for ; n > 1; n = n - 1
			for i = start; i < end; i = i + 1
				f(input[i]
	pending = null
	Mode = Object + @
		$post
		$map
		$__initialize = @
			$post = $reset
			$map = $map_default
		$render = @
			if progress != ""
				status.replace(0, -1, progress
				::progress = ""
			else if message != ""
				status.replace(0, -1, message
			else
				position = view.position(
				line = text.line_at_in_text(position.text
				n = view.range(
				i = ""
				input.each(@(x) :i = i + String.from_code(x)
				status.replace(0, -1, $name + " " +
					line.index + "," +
					(position.text - line.from) + "-" +
					(position.x - view.row().x) + " " +
					(n > 0 ? view.top() * 100 / n : 100) + "% <" +
					session.undos.size() +
					(session.logs === null ? "" : "?" + session.logs.size()) +
					(session.redos.size() > 0 ? "|" + session.redos.size() : "") + "> " + i
		$reset = @
			::count = 0
			::input = [
			$post = $reset
		$finish = @(f)
			try
				f[$](
			catch Throwable t
				::message = t.__string(
				return $reset(
			::count = 0
			$post(
		$unknown = @(c) $reset(
		$__call = @(c)
			try
				try
					map = $map[c]
				catch Throwable t
					if pending === null
						$map = $map_default
						$unknown(c
					else
						pending[1](
						pending[0](
					return
				action = map.get(
				if map.more()
					$map = map
					action === null && return
					n = input.size(
					commit = @
						cs = [
						while input.size() > n: cs.unshift(input.pop(
						:$map = :$map_default
						:::pending = null
						action[:$](
						cs.each(push
					::pending = '(commit, host.timeout(1000, commit
				else
					$map = $map_default
					if pending !== null
						pending[1](
						::pending = null
					action !== null && action[$](
			catch Throwable t
				$map = $map_default
				$reset(
	match_status = @(pattern, i) with_search(status, pattern, @(search)
		search.reset(i, -1
		search.next(
	map = @(map, lhs, rhs)
		cs = to_codes(rhs
		map.put(lhs, KeyMapAction(rhs, @
			n = lhs.size(
			for i = 0; i < n; i = i + 1: input.pop(
			cs.each(push
	for_ncdyv = @(f)
		f(mode_normal.map_default
		f(map_change
		f(map_delete
		f(map_yank
		f(mode_visual.map_default
	builtin_commands = {
		"buffers": @(i)
			:message = "buffers"
			n = buffers.size(
			for i = 0; i < n; i = i + 1
				x = buffers[i]
				:message = message + "\n" + (i + 1) + (x === buffer ? " %" : "  ") + (x.path === null ? " no name" : " \"" + x.path + "\"")
		"edit": @(i)
			match = match_status("^\\s*(?:(#\\d*)|(\\S*))", i
			if match[1].count > 0
				switch_buffer(buffers[Integer(status.slice(match[1].from + 1, match[1].count - 1)) - 1]
			else if match[2].count > 0
				open_buffer(status.slice(match[2].from, match[2].count
		"map": @(i)
			match = match_status("^\\s*(\\S+)\\s+(.+)", i
			if match.size() < 2
				:message = "maps"
				mode_normal.map_default.list().each(@(x) ::message = message + "\n" + x[0] + " " + x[1]
			else
				lhs = status.slice(match[1].from, match[1].count
				rhs = status.slice(match[2].from, match[2].count
				for_ncdyv(@(x) map(x, lhs, rhs
		"quit": @(i)
			n = buffers.size(
			n > 1 || return host.quit(
			for i = 0; i < n; i = i + 1: buffers[i] === buffer && break
			switch_buffer(buffers[i > 0 ? i - 1 : i + 1]
			remove_buffer(i
		"unmap": @(i)
			match = match_status("^\\s*(\\S+)", i
			match.size() > 1 || throw Throwable("invalid arguments"
			lhs = status.slice(match[1].from, match[1].count
			for_ncdyv(@(x) x.remove(lhs
	commands = {
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
	push = @(c)
		input.push(c
		mode(c
	map_change = KeyMap(null, KeyMap(null, map_motion, {
		letter("c"): KeyMap(@ $for_lines(@(p, q)
			begin(p
			replace(p, q - 1, true
	map_delete = KeyMap(null, KeyMap(null, map_motion, {
		letter("d"): KeyMap(@ $for_lines(@(p, q)
			begin(p
			delete[:$](p, q, true
	map_yank = KeyMap(null, KeyMap(null, map_motion, {
		letter("y"): KeyMap(@ $for_lines(@(p, q) yank[:$](p, q, true
	mode = mode_normal = (Mode + @
		$for_motion = @(begin, f)
			:count0 = count
			::count = 0
			start = input.size(
			p0 = view.position().text
			$post = @
				:$post = @
				repeat(count0, start, input.size(), mode
				:$post = :$reset
				p1 = view.position().text
				begin(p0
				if p1 < p0
					f(p1, p0
				else
					f(p0, p1
		$for_lines = @(f)
			$post = $reset
			l0 = text.line_at_in_text(view.position().text
			l = l0.index + (count > 0 ? count : count0 > 0 ? count0 : 1) - 1
			l >= text.lines() && (l = text.lines() - 1)
			l1 = l > l0.index ? text.line_at(l) : l0
			f(l0.from, l1.from + l1.count
		$map_default = KeyMap(null, KeyMap(null, map_motion, {
			control("R"): KeyMap(@ $finish(@ times(@ session.redo(
			letter("."): KeyMap(@
				input.pop(
				last = $last_input
				c = last.shift(
				if count > 0: while c >= 0x30 && c <= 0x39: c = last.shift(
				for ;; c = last.shift()
					push(c
					last.size() > 0 || break
			letter(":"): KeyMap(@ show_prompt(":"
				@
				@(ok) !ok ? :$reset() : :$finish(@
					match = match_status("^\\w+", 1
					match.size() > 0 || throw Throwable("invalid command"
					m = match[0]
					name = status.slice(m.from, m.count
					if buffer.commands.has(name)
						command = buffer.commands[name]
					else if commands.has(name)
						command = commands[name]
					else if builtin_commands.has(name)
						command = builtin_commands[name]
					else
						throw Throwable("no command: " + name
					command[::$](m.from + m.count
			letter("Z"): KeyMap(null, null, {
				letter("Z"): KeyMap(@ host.quit(
			letter("c"): KeyMap(@
				$for_motion(begin, replace
				$map = map_change
			letter("d"): KeyMap(@
				$for_motion(begin, delete[$]
				$map = map_delete
			letter("i"): KeyMap(insert
			letter("p"): KeyMap(@ $commit(@
				p = view.position().text
				begin(p
				register[0] && (p = text.line_at_in_text(p).from)
				times(@ session.merge(p, 0, register[1]
				commit(
			letter("u"): KeyMap(@ $finish(@ times(@ session.undo(
			letter("v"): KeyMap(@
				::count = 0
				::input = [
				::mode = mode_visual
				mode.base = view.position().text
			letter("y"): KeyMap(@
				$for_motion(@(p) view.position__(p, false), yank[$]
				$map = map_yank
			letter("z"): KeyMap(null, null, {
				letter("c"): KeyMap(@ $finish(@ view.position__(view.folded(view.position().text, true), false
				letter("o"): KeyMap(@ $finish(@ view.folded(view.position().text, false
		$name = "NORMAL"
		$last_input
		$__initialize = @
			Mode.__initialize[$](
			$last_input = [
		$commit = @(f) $finish(@
			f(
			$last_input = input
	)(
	mode_insert = (Mode + @
		backspace = KeyMap(@
			p = view.position().text
			if p > 0
				view.folded(p - 1, false
				session.logs === null && begin(p
				session.merge(p - 1, 1, ""
		$map_default = KeyMap(null, {
			control("H"): backspace
			control("["): KeyMap(@
				repeat(count, $start, input.size() - 1, $
				::mode = mode_normal
				mode.commit(commit
			host.KEY_BACKSPACE: backspace
		$name = "INSERT"
		$start
		$unknown = @(c)
			(c == host.KEY_ENTER || c == 0xd) && (c = 0xa)
			p = view.position().text
			session.logs === null && begin(p
			session.merge(p, 0, String.from_code(c
	)(
	mode_visual = (Mode + @
		clear = @
			xs = [
			with(host.overlay_iterator(selection), @(i)
				while (x = i.next()) !== null
					x.value && xs.push('(x.from, x.count
			xs.each(@(x) selection.paint(x[0], x[1], false
			xs
		for_selection = @(c, f)
			xs = clear(
			x = xs.size() > 0 ? xs[0] : '(0, 0
			::input = to_codes("v" + x[1] + " " + c
			::count = 0
			f(x[0], x[0] + x[1]
		$map_default = KeyMap(null, map_motion, {
			control("["): KeyMap(@
				clear(
				::mode = mode_normal
				mode.reset(
			letter("c"): KeyMap(@ for_selection("c", @(p, q)
				begin(:$base
				replace(p, q
			letter("d"): KeyMap(@ for_selection("d", @(p, q)
				begin(:$base
				:::mode = mode_normal
				delete[mode](p, q
			letter("y"): KeyMap(@ for_selection("y", @(p, q)
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
	)(
	mode_prompt = (Object + @
		done = @(ok)
			::current = view
			::mode = $caller
			$done(ok
		backspace = @
			n = status.size(
			if n > $prompt.size()
				status.replace(n - 1, 1, ""
				$doing(
			else
				done[$](false
		map = {
			control("H"): backspace
			control("M"): @ done[$](true
			control("["): @
				status.replace($prompt.size(), -1, ""
				$doing(
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
				$doing(
	)(
	map(mode_normal.map_default.base, "C", "c$"
	map(mode_normal.map_default.base, "D", "d$"
	map(mode_normal.map_default.base, "S", "cc"
	map(mode_normal.map_default.base, "X", "dh"
	map(mode_normal.map_default.base, "s", "cl"
	map(mode_normal.map_default.base, "x", "dl"
	(Object + @
		$render = @ mode.render(
		$current = @ current
		$__call = @(c)
			::message = ""
			push(c
		$message = @(x) ::message = x
		$progress = @(x) ::progress = x
		$buffers = @ buffers
		$buffer = @ buffer
		$add_buffer = add_buffer
		$switch_buffer = switch_buffer
		$open_buffer = open_buffer
		$match_status = match_status
		$commands = @ commands
	)(

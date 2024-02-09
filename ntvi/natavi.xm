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

do = @(f) f(
with = @(x, f)
	try
		f(x
	finally
		x.dispose(
letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")
$each_code = each_code = @(s, f)
	n = s.size(
	for i = 0; i < n; i = i + 1: f(s.code_at(i
join = @(each)
	s = ""
	each(@(x) :s = s + x
	s

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
			entries.sort(@(x, y) x[0] < y[0]
			entries.each(@(x)
				s = lhs + String.from_code(x[0])
				x[1].action === null || list.push('(s, x[1].action
				f(x[1], s
		f($, ""
		list
	$put = @(s, action)
		map = $
		each_code(s, @(c)
			try
				:map = map.map[c]
			catch Throwable t
				try
					base = map.base[c]
				catch Throwable t
					base = null
				:map = map.map[c] = KeyMap(null, base
		map.action = action
	$remove = @(s)
		map = $
		path = [
		each_code(s, @(c)
			try
				path.push('(map, c
				:map = map.map[c]
			catch Throwable t
				return
		map.action = null
		while map.map.size() <= 0 && path.size() > 0
			parent = path.pop(
			map = parent[0]
			map.map.remove(parent[1]
			map.action === null || break
	$merge = @(s)
		map = $
		base = $base
		try
			each_code(s, @(c)
				:map = map.map[c]
				:base = map.base = try
					base[c]
				catch Throwable t
		catch Throwable t

KeyMapAction = Object + @
	$name
	$action
	$__initialize = @(name, action)
		$name = name
		$action = action
	$__string = @ $name
	$__get_at = @(mode) $action[mode]

Maps = Object + @
	$NORMAL
	$VISUAL
	$operator
	$INSERT
	$__initialize = @(normal, visual, operator, insert)
		$NORMAL = normal
		$VISUAL = visual
		$operator = operator
		$INSERT = insert

$Buffer = Buffer = Session + @
	Delta = Object + @
		$x
		$xs
		$__initialize = @(x, xs)
			$x = x
			$xs = xs
		$__call = @ $x.replace(*$xs

	$path
	$text
	$view
	$selection
	$maps
	$commands
	$__initialize = @(path, maps, text, view, selection)
		Session.__initialize[$](
		$path = path
		$text = text
		$view = view
		$selection = selection
		$maps = Maps(
			KeyMap(null, maps.NORMAL
			KeyMap(null, maps.VISUAL
			KeyMap(null, maps.operator
			KeyMap(null, maps.INSERT
		$commands = {
	$dispose = @
		$selection.dispose(
		$view.dispose(
		$text.dispose(
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

with_search = @(text, pattern, f) with(nata.Search(text), @(search)
	try
		search.pattern(pattern, nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		f(search
	catch Throwable t
		'(

$new = @(host, status, strip, path)
	maps = Maps(
		KeyMap(null
		KeyMap(null
		KeyMap(null
		KeyMap(null
	buffers = [
	add_buffer = @(path)
		buffer = host.buffer(path, maps
		buffers.push(buffer
		buffer
	remove_buffer = @(i) buffers.remove(i).dispose(
	switch_buffer = @(buffer)
		:buffer = buffer
		:text = buffer.text
		:current = :view = buffer.view
		:selection = buffer.selection
		Mode.rewind(
		buffer
	open_buffer = @(path)
		n = buffers.size(
		for i = 0;; i = i + 1
			i < n || return switch_buffer(add_buffer(path
			x = buffers[i]
			x.path == path && break switch_buffer(x
	move_to = @(p)
		view.folded(p, false
		view.position__(p, false
	log_position = @(p) buffer.log(@
		move_to(p
		buffer.log(@ log_position(p
	begin = @(p)
		buffer.begin(
		log_position(p
	commit = @ if buffer.logs !== null
		p = view.position().text
		buffer.log(@ log_position(p
		buffer.commit("edit"
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
	last_input = [
	message = progress = ""
	show_prompt = @(prompt, doing, done)
		status.replace(0, -1, prompt
		strip.position__(prompt.size(), false
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
	escape = @(x) x < 0x20 ? "^" + String.from_code(x + 0x40) : String.from_code(x)
	Mode = Object + @
		$echo = @(f) input.each(@(x) f(escape(x
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
				status.replace(0, -1, "" + $name + " " +
					(line.index + 1) + "," +
					(position.text - line.from + 1) + "-" +
					(position.x - view.row().x + 1) + " " +
					(n > 0 ? view.top() * 100 / n : 100) + "% <" +
					buffer.undos.size() +
					(buffer.logs === null ? "" : "?" + buffer.logs.size()) +
					(buffer.redos.size() > 0 ? "|" + buffer.redos.size() : "") + "> " + join($echo
		rewind_builtin = @
			:map = mode.map
			:fallback = @(c)
				mode.unknown(c
				rewind(
			:partial = @
				action = map.get(
				action === null && return
				::pending = '(@
					:::pending = null
					action[mode](
					rewind(
				, @
		rewind_mapping = @
			:map = buffer.maps.(mode.name)
			n = 0
			commit = @
				::pending = null
				cs = [
				for ; n > 0; (:n = n - 1): cs.unshift(input.pop(
				if action === null
					rewind_builtin(
				else
					action[mode](
					rewind(
				cs.each(push
			:fallback = @(c)
				rewind_builtin(
				mode(c
			:partial = @
				action = map.get(
				if action === null
					:n = n + 1
				else
					:n = 0
					:action = action
				::pending = '(commit, host.timeout(1000, commit
		rewind = rewind_mapping
		$rewind = @ rewind(
		$nomap = @(f)
			last = rewind
			:rewind = rewind_builtin
			try
				rewind(
				f(
			finally
				:rewind = last
				rewind(
		$reset = @
			::count = 0
			::input = [
		$finish = @(f)
			f[$](
			$reset(
		$commit = @(f) $finish(@
			f(
			:::last_input = input
		$unknown = @(c) $reset(
		$__call = @(c)
			try
				try
					map = :map[c]
				catch Throwable t
					pending === null && return fallback(c
					pending[1](
					input.pop(
					pending[0](
					return push(c
				if pending !== null
					pending[1](
					:pending = null
				if map.more()
					:map = map
					partial(
				else
					action = map.get(
					action !== null && action[$](
					rewind(
			catch Throwable t
				::message = t.__string(
				mode.reset(
				rewind(
	match_status = @(pattern, i) with_search(status, pattern, @(search)
		search.reset(i, -1
		search.next(
	map = @(for_, lhs, rhs, noremap = false)
		name = join(@(f) each_code(rhs, @(c) f(escape(c
		action = @
			each_code(lhs, @(c) input.pop(
			each_code(rhs, push
		kma = noremap ?
			KeyMapAction("*" + name, @ Mode.nomap(action)) :
			KeyMapAction(name, action
		for_(@(x) x.put(lhs, kma
	for_nvo = @(maps) @(f)
		f(maps.NORMAL
		f(maps.VISUAL
		f(maps.operator
	for_ip = @(maps) @(f)
		f(maps.INSERT
	command_map = @(for_, noremap) @(i)
		match = match_status("^\\s*(<buffer>)?\\s*(\\S+)\\s+(.+)", i
		if match.size() > 0
			lhs = status.slice(match[2].from, match[2].count
			rhs = status.slice(match[3].from, match[3].count
			if match[1].count > 0
				map(for_(buffer.maps), lhs, rhs, noremap
			else
				map(for_(maps), lhs, rhs, noremap
				buffers.each(@(x) for_(x.maps)(@(x) x.merge(lhs
		else
			::message = "maps"
			# TODO: merge modes.
			buffer.maps.NORMAL.list().each(@(x) :::message = message + "\n" + x[0] + "\t@" + x[1]
			maps.NORMAL.list().each(@(x) :::message = message + "\n" + x[0] + "\t" + x[1]
	command_unmap = @(for_) @(i)
		match = match_status("^\\s*(<buffer>)?\\s*(\\S+)", i
		match.size() > 0 || throw Throwable("invalid arguments"
		lhs = status.slice(match[2].from, match[2].count
		if match[1].count > 0
			for_(buffer.maps)(@(x) x.remove(lhs
		else
			for_(maps)(@(x) x.remove(lhs
			buffers.each(@(x) for_(x.maps)(@(x) x.merge(lhs
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
		"map": command_map(for_nvo, false
		"map!": command_map(for_ip, false
		"noremap": command_map(for_nvo, true
		"noremap!": command_map(for_ip, true
		"quit": @(i)
			n = buffers.size(
			n > 1 || return host.quit(
			for i = 0; i < n; i = i + 1: buffers[i] === buffer && break
			switch_buffer(buffers[i > 0 ? i - 1 : i + 1]
			remove_buffer(i
		"unmap": command_unmap(for_nvo
		"unmap!": command_unmap(for_ip
	commands = {
	insert = @
		:mode = mode_insert
		mode.start = mode.from = input.size(
	register = '(false, ""
	yank = @(p, q, line = false) mode.finish(@ ::register = '(line, text.slice(p, q - p
	delete_and_edit = @(p, q, line) :register = '(line, buffer.replace(p, q - p, ""
	replace = @(p, q, line = false)
		delete_and_edit(p, q, line
		insert(
	delete = @(p, q, line = false) mode.commit(@
		delete_and_edit(p, q, line
		commit(
	push = @(c)
		input.push(c
		mode(c
	repeat = @(n, start) Mode.nomap(@ for ; n > 1; (:n = n - 1)
		cs = [
		while input.size() > start: cs.unshift(input.pop(
		cs.each(push
	literal = do(@
		add = @(base, i) @ ::code = code * base + i
		digit = @(map, depth, code) @
			::depth = depth
			::code = code
			map
		@(commit)
			Digit = Object + @
				$map
				$__initialize = @(f)
					$map = {
					f(@(base, c, i, n) for ; i < n; i = i + 1: :$map[c + i] = add(base, i
				$__get_at = @(c)
					$map[c](
					:::depth = depth - 1
					$
				$get = @ @ commit[$](code
				$more = @ depth > 0
			decimal = Digit(@(put) put(10, 0x30, 0, 10
			octal = Digit(@(put) put(8, 0x30, 0, 8
			hexadecimal = Digit(@(put)
				put(16, 0x30, 0, 10
				put(16, 0x37, 10, 16
				put(16, 0x57, 10, 16
			map = {
				letter("O"): digit(octal, 3, 0
				letter("X"): digit(hexadecimal, 2, 0
				letter("o"): digit(octal, 3, 0
				letter("x"): digit(hexadecimal, 2, 0
			for i = 0; i < 10; i = i + 1: map[0x30 + i] = digit(decimal, 2, i
			map_commit = KeyMap(@ commit[$](code
			do(Object + @
				$__get_at = @(c)
					try
						map[c](
					catch Throwable t
						:::code = c
						map_commit
				$get = @ null
				$more = @ true
	mode = mode_normal = do(Mode + @
		$map = KeyMap(null, map_motion, {
			control("R"): KeyMap(@ $finish(@ times(@ buffer.redo(
			letter("."): KeyMap(@ Mode.nomap(@
				input.pop(
				last = last_input
				c = last.shift(
				if count > 0: while c >= 0x30 && c <= 0x39: c = last.shift(
				for ;; c = last.shift()
					push(c
					last.size() > 0 || break
			letter(":"): KeyMap(@ show_prompt(":"
				@
				@(ok) !ok ? :$reset() : :$finish(@
					match = match_status("^(\\w|!)+", 1
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
			letter("c"): KeyMap(@ ::mode = ModeOperator(begin, replace, {
				letter("c"): KeyMap(@ $for_lines(@(p, q)
					begin(p
					replace(p, q - 1, true
			letter("d"): KeyMap(@ ::mode = ModeOperator(begin, delete, {
				letter("d"): KeyMap(@ $for_lines(@(p, q)
					begin(p
					delete(p, q, true
			letter("i"): KeyMap(insert
			letter("p"): KeyMap(@ $commit(@
				p = view.position().text
				begin(p
				register[0] && (p = text.line_at_in_text(p).from)
				times(@ buffer.merge(p, 0, register[1]
				commit(
			letter("u"): KeyMap(@ $finish(@ times(@ buffer.undo(
			letter("v"): KeyMap(@
				::count = 0
				::input = [
				::mode = mode_visual
				mode.base = view.position().text
			letter("y"): KeyMap(@ ::mode = ModeOperator(@(p) view.position__(p, false), yank, {
				letter("y"): KeyMap(@ $for_lines(@(p, q) yank(p, q, true
			letter("z"): KeyMap(null, null, {
				letter("c"): KeyMap(@ $finish(@ view.position__(view.folded(view.position().text, true), false
				letter("o"): KeyMap(@ $finish(@ view.folded(view.position().text, false
		$name = 'NORMAL
	mode_visual = do(Mode + @
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
			::input = [
			each_code("v" + x[1] + " " + c, input.push
			::count = 0
			f(x[0], x[0] + x[1]
		$map = KeyMap(null, map_motion, {
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
				delete(p, q
			letter("y"): KeyMap(@ for_selection("y", @(p, q)
				view.position__(:$base, false
				:::mode = mode_normal
				yank(p, q
		$name = 'VISUAL
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
	ModeOperator = Mode + @
		$for_lines = @(f)
			$post = $reset
			l0 = text.line_at_in_text(view.position().text
			l = l0.index + (count > 0 ? count : $count > 0 ? $count : 1) - 1
			l >= text.lines() && (l = text.lines() - 1)
			l1 = l > l0.index ? text.line_at(l) : l0
			f(l0.from, l1.from + l1.count
		$map
		$name = 'operator
		$count
		$post
		$__initialize = @(begin, f, map)
			Mode.__initialize[$](
			$count = count
			::count = 0
			start = input.size(
			p0 = view.position().text
			$post = @
				:::count = 0
				:$post = @ ::::count = 0
				repeat(:$count, start
				:$post = :$reset
				p1 = view.position().text
				begin(p0
				if p1 < p0
					f(p1, p0
				else
					f(p0, p1
			$map = KeyMap(null, map_motion, map
		$render = @ mode_normal.render(
		$reset = @
			::mode = mode_normal
			mode.reset(
		$finish = @(f)
			f[$](
			$post(
	mode_insert = do(Mode + @
		backspace = KeyMap(@
			p = view.position().text
			if p > 0
				view.folded(p - 1, false
				buffer.logs === null && begin(p
				buffer.merge(p - 1, 1, ""
			$from = input.size(
		$map = KeyMap(null, {
			control("H"): backspace
			control("V"): literal(@(c) $unknown(c
			control("["): KeyMap(@
				c = input.pop(
				repeat(count, $start
				input.push(c
				::mode = mode_normal
				mode.commit(commit
			host.KEY_BACKSPACE: backspace
		$name = 'INSERT
		$start
		$from
		$echo = @(f)
			n = input.size(
			for i = $from; i < n; i = i + 1: f(escape(input[i]
		$unknown = @(c)
			(c == host.KEY_ENTER || c == 0xd) && (c = 0xa)
			p = view.position().text
			buffer.logs === null && begin(p
			buffer.merge(p, 0, String.from_code(c
			$from = input.size(
	# TODO: inherit Mode
	mode_prompt = do(Object + @
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
		$name = 'NORMAL
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
	(@(for_n)
		map(for_n, "C", "c$", true
		map(for_n, "D", "d$", true
		map(for_n, "S", "cc", true
		map(for_n, "X", "dh", true
		map(for_n, "s", "cl", true
		map(for_n, "x", "dl", true
	)(@(f) f(mode_normal.map
	switch_buffer(add_buffer(path
	do(Object + @
		$render = @ mode.render(
		$current = @ current
		$__call = @(c)
			::message = ""
			push(c
		$message = @(x) ::message = x
		$progress = @(x) ::progress = x
		$buffers = buffers
		$buffer = @ buffer
		$add_buffer = add_buffer
		$switch_buffer = switch_buffer
		$open_buffer = open_buffer
		$match_status = match_status
		$map = map
		$nomap = Mode.nomap
		$map_action = @(for_, lhs, name, action)
			kma = KeyMapAction(name, @
				each_code(lhs, @(c) input.pop(
				action[$](
			for_(@(x) x.put(lhs, kma
		$commands = commands

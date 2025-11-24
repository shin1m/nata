nata = Module("nata"
utilities = Module("utilities"
do = utilities.do
with = utilities.with
letter = utilities.letter
control = utilities.control
each_code = utilities.each_code

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
		$map = map || {}
	$__get_at = @(c) try
		$map[c]
	catch Throwable t
		$base[c]
	$get = @ $action || $base && $base.get(
	$more = @ $map.size() > 0 || $base && $base.more(
	$each = @(do)
		f = @(map, lhs) map.map.each(@(k, v)
			s = lhs + String.from_code(k)
			v.action && do(s, v.action
			f(v, s
		f($, ""
	$put = @(s, action)
		map = $
		each_code(s, @(c) :map = try
			map.map[c]
		catch Throwable t
			map.map[c] = KeyMap(null, try
				map.base[c]
			catch Throwable t
		map.action = action
	$remove = @(s)
		map = $
		path = [
		each_code(s, @(c) try
			path.push('(map, c
			:map = map.map[c]
		catch Throwable t
			return
		map.action = null
		while map.map.size() <= 0 && path.size() > 0
			parent = path.pop(
			map = parent[0]
			map.map.remove(parent[1]
			map.action && break
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
	$prompt
	$__initialize = @(normal, visual, operator, insert, prompt)
		$NORMAL = normal
		$VISUAL = visual
		$operator = operator
		$INSERT = insert
		$prompt = prompt

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
			KeyMap(null, maps.prompt
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

new_pattern = @(pattern) nata.Pattern(pattern, nata.Pattern.ECMASCRIPT | nata.Pattern.OPTIMIZE

$new = @(host, status, strip, path)
	maps = Maps(
		KeyMap(null
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
	commit = @ if buffer.logs
		p = view.position().text
		buffer.log(@ log_position(p
		buffer.commit("edit"
	count = 0
	times = @(f) for n = count > 0 ? count : 1; n > 0; n = n - 1; f(
	forward_n = @(x, limit, n, f)
		y = x + n
		y > limit && (y = limit
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
		current = :current
		:current = strip
		mode = :mode
		:mode = mode_prompt
		:mode.prompt = prompt
		:mode.doing = doing
		:mode.done = @(ok)
			::current = current
			::mode = mode
			done(ok
	search_loop = @(p, step, wrap)
		wrapped = false
		for n = count > 0 ? count : 1
			match = step(p
			if match.size() <= 0
				match = wrap(
				match.size() > 0 || return
				wrapped = true
			n = n - 1
			n > 0 || return '(match[0], wrapped
			p = match[0].from
	search_forward = @(pattern, p) with(new_pattern(pattern), @(pattern)
		f = @(p) pattern.search(text, p, -1
		search_loop(p, @(p) p < text.size() ? f(p + 1) : '(), @ f(0
	search_backward = @(pattern, p) with(new_pattern(pattern), @(pattern) with(nata.Search(text, pattern), @(search)
		f = @(p)
			last = '(
			for m = search.first(0, -1); m.size() > 0 && m[0].from <= p; m = search.next(); last = m
			last
		search_loop(p, @(p) p > 0 ? f(p - 1) : '(), @ f(text.size(
	search_pattern = null
	search_reverse = false
	search_message = @(reverse, match)
		match || throw Throwable("not found: " + search_pattern
		:message = (reverse ?
			(match[1] ? "continuing at BOTTOM: " : "?") :
			(match[1] ? "continuing at TOP: " : "/")
		) + search_pattern
	search_next = @(reverse) $finish(@
		search_pattern || throw Throwable("no last pattern"
		reverse = search_reverse ^ :reverse
		p = view.position().text
		match = reverse ? search_backward(search_pattern, p) : search_forward(search_pattern, p)
		search_message(reverse, match
		move_to(match[0].from
	search = @(reverse)
		p = view.position().text
		pattern = ""
		paint = @(selected) if match
			m = match[0
			selection.paint(m.from, m.count, selected
		show_prompt(reverse ? "?" : "/"
			@
				:pattern = status.slice(1, -1
				paint(false
				try
					:match = pattern.size() > 0 && (reverse ? search_backward(pattern, p) : search_forward(pattern, p)
					:error = null
				catch Throwable t
					:match = null
					:error = t
				paint(true
				if match
					move_to(match[0].from
				else
					view.position__(p, false
			@(ok) !ok ? :$reset() : (
				::search_reverse = reverse
				pattern.size() > 0 || return search_next[:$](false
				paint(false
				::search_pattern = pattern
				error && throw error
				:$finish(@ search_message(reverse, match
	find_letter = null
	find_reverse = false
	find_next = @(reverse)
		find_letter || return $reset(
		reverse = find_reverse ^ reverse
		p = view.position().text
		l = text.line_at_in_text(p
		limit = reverse ? l.from : l.from + l.count - 1
		d = reverse ? -1 : 1
		for n = count > 0 ? count : 1; p != limit
			p = p + d
			text.slice(p, 1) == find_letter || continue
			n = n - 1
			n > 0 || return $finish(@ view.position__(p, !reverse
		$reset(
	find = @(reverse)
		map_commit = KeyMap(@ find_next[$](false
		do(Object + @
			$__get_at = @(c)
				:::find_letter = String.from_code(c
				:::find_reverse = reverse
				map_commit
			$get = @
			$more = @ true
	hat_pattern = new_pattern("\\S"
	hat = @(line)
		l = text.line_at(line
		m = hat_pattern.search(text, l.from, l.count - 1
		view.position__(m.size() > 0 ? m[0].from : l.from + l.count - 1, false
	skip_loop = @(f)
		p = q = view.position().text
		for n = count > 0 ? count : 1; q = f(q)
			n = n - 1
			n > 0 || break
		q != p && view.position__(q, q > p
	skip_forward = @(pattern) skip_loop(@(p)
		m = pattern.search(text, p, -1
		m.size() > 0 && m[0].from + m[0].count
	skip_backward = @(pattern) skip_loop(@(p)
		l = text.line_at_in_text(p).index
		for q = l > 0 ? text.line_at(l - 1).from : 0
			m = pattern.search(text, q, p - q
			m.size() > 0 || return
			q = m[0].from
			r = q + m[0].count
			r < p || break q
			q = r
	do(@
		start = @(word) "(?:" + word + ")(?:(?!\\n)\\s)*(?:\\n(?:(?!\\n)\\s)*)?"
		end = @(word) "\\s*(?:" + word + ")"
		word = "\\w+|[^\\s\\w]*"
		WORD = "\\S*"
		:skip_word_start = new_pattern(start(word
		:skip_word_end = new_pattern(end(word
		:skip_WORD_start = new_pattern(start(WORD
		:skip_WORD_end = new_pattern(end(WORD
	map_motion = {
		control("H"): KeyMap(@ $finish(@ backward(view.position().text, 0, @(p) view.position__(p, false
		letter(" "): KeyMap(@ $finish(@ forward(view.position().text, text.size(), @(p) view.position__(p, true
		letter("$"): KeyMap(@ $finish(@
			view.target__(-1
			count > 1 && forward_n(view.row().line, view.size().line - 1, count - 1, view.line__
		letter("%"): KeyMap(@ $finish(@ if count > 0 && count <= 100
			n = view.size().line
			forward_n(-1, n - 1, (count * n + 99) / 100, hat
		letter("+"): KeyMap(@ $finish(@ forward(view.row().line, view.size().line - 1, hat
		letter(","): KeyMap(@ find_next[$](true
		letter("-"): KeyMap(@ $finish(@ backward(view.row().line, 0, hat
		letter("/"): KeyMap(@ search[$](false
		letter("0"): KeyMap(@
			if count > 0
				:count = count * 10
			else
				$finish(@ view.position__(text.line_at(view.row().line).from, false
		letter("1"): KeyMap(@ :count = count * 10 + 1
		letter("2"): KeyMap(@ :count = count * 10 + 2
		letter("3"): KeyMap(@ :count = count * 10 + 3
		letter("4"): KeyMap(@ :count = count * 10 + 4
		letter("5"): KeyMap(@ :count = count * 10 + 5
		letter("6"): KeyMap(@ :count = count * 10 + 6
		letter("7"): KeyMap(@ :count = count * 10 + 7
		letter("8"): KeyMap(@ :count = count * 10 + 8
		letter("9"): KeyMap(@ :count = count * 10 + 9
		letter(";"): KeyMap(@ find_next[$](false
		letter("?"): KeyMap(@ search[$](true
		letter("B"): KeyMap(@ $finish(@ skip_backward(skip_WORD_start
		letter("E"): KeyMap(@ $finish(@ skip_forward(skip_WORD_end
		letter("F"): find(true
		letter("G"): KeyMap(@ $finish(@
			n = view.size().line
			forward_n(-1, n - 1, count > 0 ? count : n, hat
		letter("N"): KeyMap(@ search_next[$](true
		letter("W"): KeyMap(@ $finish(@ skip_forward(skip_WORD_start
		letter("^"): KeyMap(@ $finish(@ hat(view.row().line
		letter("b"): KeyMap(@ $finish(@ skip_backward(skip_word_start
		letter("e"): KeyMap(@ $finish(@ skip_forward(skip_word_end
		letter("f"): find(false
		letter("g"): KeyMap(null, null, {
			letter("g"): KeyMap(@ $finish(@ forward_n(-1, view.size().line - 1, count > 0 ? count : 1, hat
		letter("h"): KeyMap(@ $finish(@
			p = view.position().text
			backward(p, text.line_at_in_text(p).from, @(p) view.position__(p, false
		letter("j"): KeyMap(@ $finish(@ forward(view.row().line, view.size().line - 1, view.line__
		letter("k"): KeyMap(@ $finish(@ backward(view.row().line, 0, view.line__
		letter("l"): KeyMap(@ $finish(@
			p = view.position().text
			l = text.line_at_in_text(p
			forward(p, l.from + l.count - 1, @(p) view.position__(p, true
		letter("n"): KeyMap(@ search_next[$](false
		letter("w"): KeyMap(@ $finish(@ skip_forward(skip_word_start
		letter("|"): KeyMap(@ $finish(@ view.target__(count > 0 ? count - 1 : 0
	map_motion[control("J")] = map_motion[letter("j"
	map_motion[control("M")] = map_motion[letter("+"
	map_motion[control("N")] = map_motion[letter("j"
	map_motion[control("P")] = map_motion[letter("k"
	escape = @(x) x < 0x20 ? "^" + String.from_code(x + 0x40) : String.from_code(x)
	escape_string = @(s) join(@(f) each_code(s, @(c) f(escape(c
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
					(buffer.logs ? "?" + buffer.logs.size() : "") +
					(buffer.redos.size() > 0 ? "|" + buffer.redos.size() : "") + "> " + join($echo
		rewind_builtin = @
			:map = mode.map
			:fallback = @(c)
				mode.unknown(c
				rewind(
			:partial = @
				action = map.get(
				action || return
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
				for ; n > 0; :n = n - 1; cs.unshift(input.pop(
				if action
					action[mode](
					rewind(
				else
					rewind_builtin(
				cs.each(push
			:fallback = @(c)
				rewind_builtin(
				mode(c
			:partial = @
				action = map.get(
				if action
					:n = 0
					:action = action
				else
					:n = n + 1
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
		$__call = @(c) try
			try
				map = :map[c]
			catch Throwable t
				pending || return fallback(c
				pending[1](
				input.pop(
				pending[0](
				return push(c
			if pending
				pending[1](
				:pending = null
			if map.more()
				:map = map
				partial(
			else
				action = map.get(
				action && action[$](
				rewind(
		catch Throwable t
			::message = t.__string(
			mode.reset(
			rewind(
	map = @(for_, lhs, rhs, noremap = false)
		# TODO: detect recursing.
		action = @
			each_code(lhs, @(c) input.pop(
			each_code(rhs, push
		kma = KeyMapAction(
			(noremap ? "*" : " ") + escape_string(rhs
			noremap ? @ Mode.nomap(action) : action
		for_(@(x) x.put(lhs, kma
	for_nvo = @(maps) @(f)
		f(maps.NORMAL
		f(maps.VISUAL
		f(maps.operator
	for_ip = @(maps) @(f)
		f(maps.INSERT
		f(maps.prompt
	command_map_pattern = new_pattern("^\\s*(<buffer>)?\\s*(\\S+)\\s+(.+)"
	command_map = @(for_, noremap) @(i)
		match = command_map_pattern.search(status, i, -1
		if match.size() > 0
			lhs = status.slice(match[2].from, match[2].count
			rhs = status.slice(match[3].from, match[3].count
			if match[1].count > 0
				map(for_(buffer.maps), lhs, rhs, noremap
			else
				map(for_(maps), lhs, rhs, noremap
				buffers.each(@(x) for_(x.maps)(@(x) x.merge(lhs
		else
			list = @(maps) join(@(push)
				modes = {
					maps.NORMAL: '(1 << 0, "n"
					maps.VISUAL: '(1 << 1, "v"
					maps.operator: '(1 << 2, "o"
					maps.INSERT: '(1 << 3, "i"
					maps.prompt: '(1 << 4, "p"
				lhs2a2ms = {
				f = @(map, mode) map.each(@(lhs, action)
					a2ms = try
						lhs2a2ms[lhs]
					catch Throwable t
						lhs2a2ms[lhs] = {
					a2ms[action] = mode + (try
						a2ms[action]
					catch Throwable t
						0
					)
				for_maps = for_(maps
				for_maps(@(x) f(x, modes[x][0]
				xs = [
				lhs2a2ms.each(@(lhs, a2ms) a2ms.each(@(a, ms) xs.push('(lhs, a, ms
				xs.sort(@(x, y) x[0] != y[0] ? x[0] < y[0] : x[2] < y[2]
				xs.each(@(x)
					ms = join(@(f) for_maps(@(y)
						m = modes[y]
						f((x[2] & m[0]) == 0 ? " " : m[1]
					push("\n " + ms + "\t" + escape_string(x[0]) + "\t" + x[1]
			::message = "maps <buffer>" + list(buffer.maps) + "\nmaps" + list(maps
	command_unmap_pattern = new_pattern("^\\s*(<buffer>)?\\s*(\\S+)"
	command_unmap = @(for_) @(i)
		match = command_unmap_pattern.search(status, i, -1
		match.size() > 0 || throw Throwable("invalid arguments"
		lhs = status.slice(match[2].from, match[2].count
		if match[1].count > 0
			for_(buffer.maps)(@(x) x.remove(lhs
		else
			for_(maps)(@(x) x.remove(lhs
			buffers.each(@(x) for_(x.maps)(@(x) x.merge(lhs
	command_edit_pattern = new_pattern("^\\s*(?:(#\\d*)|(\\S*))"
	builtin_commands = {
		"buffers": @(i)
			:message = "buffers" + join(@(f)
				n = buffers.size(
				for i = 0; i < n; i = i + 1
					x = buffers[i
					f("\n" + (i + 1) + (x === buffer ? " %" : "  ") + (x.path ? " \"" + x.path + "\"" : " no name"
		"edit": @(i)
			match = command_edit_pattern.search(status, i, -1
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
			for i = 0; i < n; i = i + 1; buffers[i] === buffer && break
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
		if p < q
			delete_and_edit(p, q, line
			commit(
		else
			buffer.cancel(
	push = @(c)
		input.push(c
		mode(c
	repeat = @(n, start) Mode.nomap(@ for ; n > 1; (:n = n - 1)
		cs = [
		while input.size() > start; cs.unshift(input.pop(
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
					f(@(base, c, i, n) for ; i < n; i = i + 1; :$map[c + i] = add(base, i
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
			for i = 0; i < 10; i = i + 1; map[0x30 + i] = digit(decimal, 2, i
			map_commit = KeyMap(@ commit[$](code
			do(Object + @
				$__get_at = @(c) try
					map[c](
				catch Throwable t
					:::code = c
					map_commit
				$get = @
				$more = @ true
	mode = mode_normal = do(Mode + @
		command_pattern = new_pattern("^(\\w|!)+"
		$map = KeyMap(null, map_motion, {
			control("R"): KeyMap(@ $finish(@ times(@ buffer.redo(
			letter("."): KeyMap(@ Mode.nomap(@
				input.pop(
				last = last_input
				c = last.shift(
				if count > 0; while c >= 0x30 && c <= 0x39; c = last.shift(
				for ;; c = last.shift()
					push(c
					last.size() > 0 || break
			letter(":"): KeyMap(@ show_prompt(":"
				@
				@(ok) !ok ? :$reset() : :$finish(@
					match = command_pattern.search(status, 1, -1
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
				while x = i.next()
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
			l0 = text.line_at(view.row().line
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
		$render = mode_normal.render
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
				buffer.logs || begin(p
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
			for i = $from; i < n; i = i + 1; f(escape(input[i]
		$unknown = @(c)
			(c == host.KEY_ENTER || c == 0xd) && (c = 0xa)
			p = view.position().text
			buffer.logs || begin(p
			buffer.merge(p, 0, String.from_code(c
			$from = input.size(
	mode_prompt = do(Mode + @
		backspace = KeyMap(@
			n = status.size(
			if n > $prompt.size()
				status.replace(n - 1, 1, ""
				$doing(
			else
				$done(false
		$map = KeyMap(null, {
			control("H"): backspace
			control("M"): KeyMap(@ $done(true
			control("["): KeyMap(@
				status.replace($prompt.size(), -1, ""
				$doing(
				$done(false
			host.KEY_BACKSPACE: backspace
			host.KEY_ENTER: KeyMap(@ $done(true
		$name = 'prompt
		$prompt
		$doing
		$done
		$render = @
		$unknown = @(c)
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
		$map = map
		$nomap = Mode.nomap
		$map_action = @(for_, lhs, name, action)
			kma = KeyMapAction("&" + name, @
				each_code(lhs, @(c) input.pop(
				action[$](
			for_(@(x) x.put(lhs, kma
		$commands = commands

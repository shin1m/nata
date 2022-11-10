system = Module("system"
io = Module("io"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_tree_sitter = Module("nata-tree-sitter"

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

read = @(text, path)
	reader = io.Reader(io.File(path, "r"), "utf-8"
	try
		while true
			s = reader.read(256
			s == "" && break
			text.replace(text.size(), 0, s
	finally
		reader.close(

with = @(x, f)
	try
		f(x
	finally
		x.dispose(

each = @(overlay, f)
	with(nata_curses.OverlayIterator(overlay), @(i)
		while (x = i.next()) !== null
			x.value && f(x.from, x.count

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
		$log(Delta($, '(p, s.size(), $text.slice(p, n
		$text.replace(p, n, s
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

nata.main(@ nata_curses.main(@
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	nata_curses.define_pair(4, nata_curses.COLOR_RED, -1
	nata_curses.define_pair(5, nata_curses.COLOR_GREEN, -1
	nata_curses.define_pair(6, nata_curses.COLOR_YELLOW, -1
	nata_curses.define_pair(7, nata_curses.COLOR_BLUE, -1
	nata_curses.define_pair(8, nata_curses.COLOR_MAGENTA, -1
	nata_curses.define_pair(9, nata_curses.COLOR_CYAN, -1
	capture2token = {
		"string": nata_curses.Token(nata_curses.color_pair(4
		"number": nata_curses.Token(nata_curses.color_pair(4
		"literal": nata_curses.Token(nata_curses.color_pair(4
		"key": nata_curses.Token(nata_curses.color_pair(6
		"keyword": nata_curses.Token(nata_curses.color_pair(6
		"escape": nata_curses.Token(nata_curses.color_pair(8
		"comment": nata_curses.Token(nata_curses.color_pair(7
		"bracket": nata_curses.Token(nata_curses.color_pair(8
		"object": nata_curses.Token(nata_curses.color_pair(5
		"array": nata_curses.Token(nata_curses.color_pair(9
	text = nata.Text(
	if system.arguments.size() > 0
		read(text, system.arguments[0]
	else
		text.replace(0, -1, "{
  \"hello\": [\"Hello\", \"World!\"],
  \"by\": [\"This\", \"is\", \"shin.\"],
  \"bye\": [\"Good\", \"bye.\"]
}
"
	size = nata_curses.size(
	view = nata_curses.View(text, 0, 0, size[0], size[1] - 1
	view.attributes(
		nata_curses.A_DIM | nata_curses.color_pair(1
		nata_curses.A_DIM | nata_curses.color_pair(2
	highlight = nata_curses.Overlay(view, nata_curses.color_pair(3
	selection = nata_curses.Overlay(view, nata_curses.A_REVERSE
	status = nata.Text(
	strip = nata_curses.View(status, 0, size[1] - 1, size[0], 1
	session = TextSession(text
	log_position = @(p) session.log(@
		view.position__(p, false
		session.log(@ log_position(p
	last_position = 0
	edit = @(f)
		if session.logs === null
			session.begin(
			log_position(view.position().text
		f(
		:last_position = view.position().text
	commit = @ if session.logs !== null
		p = last_position
		session.log(@ log_position(p
		session.commit("edit"
	message = ""
	syntax = (@
		UNIT = 4
		query = nata_tree_sitter.Query(
			Module("nata-tree-sitter-json").language
			"
(number) @number
(string) @string
(escape_sequence) @escape
[(null) (true) (false)] @literal
(pair key: (_) @key)
[\"{\" \"}\" \"[\" \"]\"] @bracket
(comment) @comment
(object) @object
(array) @array
[(object) (array)] @crease
"
		parser = nata_tree_sitter.Parser(text, query
		painter = nata_curses.Painter(view
		creaser = nata_curses.Creaser(view
		paint = @(p) while true
			token = tokens[0]
			to = p < token[1] ? p : token[1]
			q = painter.current(
			q < to && painter.push(token[0], to - q, 64
			to < token[1] && break
			tokens.shift(
		captures = [
		query.captures().each(@(x)
			if x == "crease"
				captures.push(@(p, n) creaser.push(p - creaser.current(), n
			else
				token = capture2token.has(x) ? capture2token[x] : null
				captures.push(@(p, n)
					paint(p
					tokens.unshift('(token, p + n
		step = @
			parser.parsed() || (:tokens = ['(null, text.size() + 1)])
			for i = 0; i < UNIT; i = i + 1
				(match = parser.next()) === null && break
				captures[match[2]](match[0], match[1]
			if match === null
				paint(text.size(
				painter.flush(
				creaser.end(
				:message = ""
			else
				painter.flush(
				:message = "running: " + painter.current() * 100 / text.size() + "%"
		(Object + @
			$more = @ !parser.parsed() || painter.current() < text.size()
			$step = step
		)(
	)(
	done = false
	backspace = @
		position = view.position().text
		if position > 0
			view.folded(position - 1, false
			edit(@ session.merge(position - 1, 1, ""
	actions = {
		0x8: backspace
		0x19: @
			commit(
			try
				session.redo(
			catch Throwable t
				:message = t.__string(
		0x1a: @
			commit(
			try
				session.undo(
			catch Throwable t
				:message = t.__string(
		0x1b: @
			:done = true
		nata_curses.KEY_RESIZE: @
			size = nata_curses.size(
			view.move(0, 0, size[0], size[1] - 1
			strip.move(0, size[1] - 1, size[0], 1
		nata_curses.KEY_DOWN: @
			commit(
			line = view.row().line
			line + 1 < view.size().line && view.line__(line + 1
		nata_curses.KEY_UP: @
			commit(
			line = view.row().line
			line > 0 && view.line__(line - 1
		nata_curses.KEY_LEFT: @
			position = view.position().text
			position > 0 && view.position__(position - 1, false
		nata_curses.KEY_RIGHT: @
			position = view.position().text
			position < text.size() && view.position__(position + 1, true
		nata_curses.KEY_BACKSPACE: backspace
		nata_curses.KEY_F1: @
			view.position__(view.folded(view.position().text, true), false
		nata_curses.KEY_F2: @
			view.folded(view.position().text, false
		nata_curses.KEY_F3: @
			pattern = ""
			each(selection, @(p, n) :pattern = pattern + text.slice(p, n
			selection.paint(0, -1, false
			pattern == "" && return
			with(nata.Search(text), @(search)
				search.pattern(pattern, nata.Search.ECMASCRIPT
				search.reset(0, -1
				while (match = search.next()).size() > 0
					highlight.paint(match[0].from, match[0].count, true
		nata_curses.KEY_F4: @
			highlight.paint(0, -1, false
		nata_curses.KEY_F5: @
			position = view.position().text
			if position < text.size()
				selection.paint(position, 1, true
				view.position__(position + 1, true
		nata_curses.KEY_F6: @
			selection.paint(0, -1, false
	update_status = @
		position = view.position(
		line = text.line_at_in_text(position.text
		n = view.range(
		status.replace(0, -1, "" +
			line.index + "," +
			(position.text - line.from) + "-" +
			(position.x - view.row().x) + " " +
			(n > 0 ? view.top() * 100 / n : 100) + "% <" +
			session.undos.size() +
			(session.logs === null ? "" : "?" + session.logs.size()) +
			(session.redos.size() > 0 ? "|" + session.redos.size() : "") + ">"
	update_status(
	while !done
		view.render(
		strip.render(
		view.timeout(syntax.more() ? 0 : -1
		c = null
		try
			c = view.get(
		catch Throwable t
		if c !== null
			action = null
			try
				action = actions[c]
			catch Throwable t
				(c == nata_curses.KEY_ENTER || c == 0xd) && (c = 0xa)
				edit(@ session.merge(view.position().text, 0, String.from_code(c
			action !== null && action(
			view.into_view(view.position().text
		syntax.step(
		if message != ""
			status.replace(0, -1, message
		else if c !== null || !syntax.more()
			update_status(

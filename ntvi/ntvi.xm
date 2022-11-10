system = Module("system"
io = Module("io"
time = Module("time"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_tree_sitter = Module("nata-tree-sitter"
natavi = Module("natavi"

View = nata_curses.View + @
	$disposing
	$__initialize = @(text, x, y, width, height)
		nata_curses.View.__initialize[$](text, x, y, width, height
		$attributes(
			nata_curses.A_DIM | nata_curses.color_pair(1
			nata_curses.A_DIM | nata_curses.color_pair(2
		$disposing = [
	$dispose = @
		$disposing.each(@(x) x(
		nata_curses.View.dispose[$](

remove = @(xs, x)
	n = xs.size(
	for i = 0; i < n; i = i + 1
		xs[i] === x && break xs.remove(i

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
	syntax = @(text, view, message)
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
		view.disposing.unshift(@
			remove(tasks, step
			parser.dispose(
			query.dispose(
			painter.dispose(
			creaser.dispose(
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
			else
				painter.flush(
				message("running: " + painter.current() * 100 / text.size() + "%"
				timers.push('(time.now(), @
		tasks.push(step
	done = false
	timers = [
	tasks = [
	size = nata_curses.size(
	vi = natavi.new((Object + @
		$quit = @ ::done = true
		$text = nata.Text
		$read = @(text, path)
			reader = io.Reader(io.File(path, "r"), "utf-8"
			try
				while true
					s = reader.read(256
					s == "" && break
					text.replace(text.size(), 0, s
			finally
				reader.close(
		$main_view = @(text)
			main = View(text, 0, 0, size[0], size[1] - 1
			syntax(text, main, @(x) vi.message(x
			main
		$strip_view = @(text) View(text, 0, size[1] - 1, size[0], 1
		$timeout = @(timeout, action)
			x = '(time.now() + timeout / 1000.0, action
			timers.push(x
			@ remove(timers, x
		$selection = @(view) nata_curses.Overlay(view, nata_curses.A_REVERSE
		$overlay_iterator = nata_curses.OverlayIterator
		$KEY_BACKSPACE = nata_curses.KEY_BACKSPACE
		$KEY_ENTER = nata_curses.KEY_ENTER
	)(), system.arguments.size() > 0 ? system.arguments[0] : null
	while !done
		now = time.now(
		ts = timers
		timers = [
		ts.each(@(x) x[0] > now ? timers.push(x) : x[1](
		tasks.each(@(x) x(
		vi.render(
		strip = vi.strip(
		n = strip.size().index
		n > size[1] && (n = size[1])
		main = vi.main(
		main.move(0, 0, size[0], size[1] - n
		main.into_view(main.position().text
		strip.move(0, size[1] - n, size[0], n
		strip.into_view(strip.position().text
		main.render(
		strip.render(
		current = vi.current(
		timeout = -1
		timers.each(@(x)
			t = Integer((x[0] - now) * 1000.0
			(timeout < 0 || t < timeout) && (:timeout = t)
		current.timeout(timeout
		try
			c = current.get(
		catch Throwable t
			continue
		if c == nata_curses.KEY_RESIZE
			size = nata_curses.size(
		else
			vi(c

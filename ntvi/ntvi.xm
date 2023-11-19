system = Module("system"
io = Module("io"
suisha = Module("suisha"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_syntax = Module("syntax"
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

open = @(path, action)
	reader = io.Reader(io.File("" + path, "r"), "utf-8"
	try
		action(reader
	finally
		reader.close(

suisha.main(@(loop) nata.main(@ nata_curses.main(@
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	nata_syntax.initialize(4
	tasks = [
	size = nata_curses.size(
	vi = natavi.new(Object + @
		$quit = @ loop.exit(
		$text = nata.Text
		$read = @(text, path) open(path, @(reader) while true
			s = reader.read(256
			s == "" && break
			text.replace(text.size(), 0, s
		$main_view = @(text, path)
			main = View(text, 0, 0, size[0], size[1] - 1
			syntax = nata_syntax.new(text, path, main, @(size, current)
				vi.message("running: " + current * 100 / size + "%"
				invalidate(
			if syntax !== null
				main.disposing.unshift(@
					remove(tasks, syntax.step
					syntax.dispose(
				tasks.push(syntax.step
			main
		$strip_view = @(text) View(text, 0, size[1] - 1, size[0], 1
		$timeout = @(timeout, action) loop.timer(@
			action(
			invalidate(
		, timeout, true).stop
		$selection = @(view) nata_curses.Overlay(view, nata_curses.A_REVERSE
		$overlay_iterator = nata_curses.OverlayIterator
		$KEY_BACKSPACE = nata_curses.KEY_BACKSPACE
		$KEY_ENTER = nata_curses.KEY_ENTER
	, system.arguments.size() > 0 ? system.arguments[0] : null
	invalid = false
	invalidate = @ if !invalid
		:invalid = true
		loop.post(@
			::invalid = false
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
			vi.current().focus(
			nata_curses.flush(
	loop.poll(0, true, false, @(readable, writable) if readable
		c = vi.current().get(
		if c == nata_curses.KEY_RESIZE
			size = nata_curses.size(
		else
			vi(c
		invalidate(
	invalidate(
	loop.run(

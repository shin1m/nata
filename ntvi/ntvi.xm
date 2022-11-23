system = Module("system"
io = Module("io"
time = Module("time"
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

nata.main(@ nata_curses.main(@
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	nata_syntax.initialize(4
	done = false
	timers = [
	tasks = [
	size = nata_curses.size(
	vi = natavi.new(Object + @
		$quit = @ ::done = true
		$text = nata.Text
		$read = @(text, path) open(path, @(reader) while true
			s = reader.read(256
			s == "" && break
			text.replace(text.size(), 0, s
		$main_view = @(text, path)
			main = View(text, 0, 0, size[0], size[1] - 1
			syntax = nata_syntax.new(text, path, main, @(size, current)
				vi.message("running: " + current * 100 / size + "%"
				timers.push('(time.now(), @
			if syntax !== null
				main.disposing.unshift(@
					remove(tasks, syntax.step
					syntax.dispose(
				tasks.push(syntax.step
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
	, system.arguments.size() > 0 ? system.arguments[0] : null
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

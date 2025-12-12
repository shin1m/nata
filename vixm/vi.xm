system = Module("system"
suisha = Module("suisha"
nata = Module("nata"
nata_curses = Module("nata-curses"
utilities = Module("utilities"
remove = utilities.remove
letter = utilities.letter
control = utilities.control
open_reader = utilities.open_reader
open_writer = utilities.open_writer
core = Module("core"
syntax = Module("syntax"
assist = Module("assist"

Text = nata.Text + @
	$replaced
	$version
	$__initialize = @
		nata.Text.__initialize[$](
		$replaced = [
		$version = 0
	$to_location = @(x)
		l = $line_at_text(x
		'(l.index, $in_bytes(x) - $in_bytes(l.from)
	$from_location = @(x) $in_text($in_bytes($line_at(x["line"]).from) + x["character"]
	$replace = @(p, n, s)
		l0 = $to_location(p
		l1 = $to_location(p + n
		nata.Text.replace[$](p, n, s
		$version = $version + 1
		$replaced.each(@(x) x(l0[0], l0[1], l1[0], l1[1], s

View = nata_curses.View + @
	$__initialize = @(text, x, y, width, height)
		nata_curses.View.__initialize[$](text, x, y, width, height
		$attributes(
			nata_curses.A_DIM | nata_curses.color_pair(1
			nata_curses.A_DIM | nata_curses.color_pair(2

Buffer = core.Buffer + @
	$disposing
	$syntax
	$hooks
	$__initialize = @(path, maps, text, view, syntax)
		core.Buffer.__initialize[$](path, maps, text, view, nata_curses.Overlay(view, nata_curses.A_REVERSE
		$disposing = [
		$syntax = syntax
		$hooks = {
	$dispose = @
		$hooks.each(@(k, v) v(
		$disposing.each(@(x) x(
		core.Buffer.dispose[$](

read = @(text, path) open_reader(path, @(reader) while
	s = reader.read(256
	s == "" && break
	text.replace(text.size(), 0, s

write = @(text, path) open_writer(path, @(writer)
	n = text.size(
	for i = 0; i < n; i = i + 256; writer.write(text.slice(i, 256

Popup = Object + @
	$text
	$view
	$__initialize = @(x, y, width, height)
		$text = nata.Text(
		$view = View($text, x, y, width, height
	$dispose = @
		$view.dispose(
		$text.dispose(

Chooser = Popup + @
	$selection
	$done
	$__initialize = @(x, y, width, height, done)
		Popup.__initialize[$](x, y, width, height
		$selection = nata_curses.Overlay($view, nata_curses.A_REVERSE
		$done = done
	$dispose = @
		$selection.dispose(
		Popup.dispose[$](
	$at = @ $view.head().line
	$at__ = @(i)
		line = $text.line_at($view.head().line
		$selection.paint(line.from, line.count, false
		$view.line__(i
		line = $text.line_at($view.head().line
		$selection.paint(line.from, line.count, true
	map = {
		control("M"): @ $done(true
		control("["): @ $done(false
		letter("j"): @
			line = $view.head().line
			line < $view.size().line - 1 && $at__(line + 1
		letter("k"): @
			line = $view.head().line
			line > 0 && $at__(line - 1
		nata_curses.KEY_ENTER: @ $done(true
	$__call = @(c) try
		map[c][$](
	catch Throwable t

suisha.main(@ nata.main(@ nata_curses.main_with_resized(@(resized)
	loop = suisha.loop(
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	syntax.initialize(4
	tasks = [
	hooks = [
	size = nata_curses.size(
	status = nata.Text(
	strip = View(status, 0, size[1] - 1, size[0], 1
	mode = vi = core.new(Object + @
		$quit = loop.exit
		$buffer = @(path, maps)
			text = Text(
			path && read(text, path
			view = View(text, 0, 0, size[0], size[1] - 1
			syntax = :syntax.new(text, path, view
			buffer = Buffer(path, maps, text, view, syntax
			if syntax
				tasks.push(step = @
					current = syntax.step(
					current || return
					vi.progress("running: " + current * 100 / text.size() + "%"
					invalidate(
				buffer.disposing.unshift(@
					remove(tasks, step
					syntax.dispose(
			hooks.each(@(x) x(buffer
			buffer
		$write = write
		$timeout = @(timeout, action) loop.timer(@
			action(
			invalidate(
		, timeout, true).stop
		$overlay_iterator = nata_curses.OverlayIterator
		$KEY_BACKSPACE = nata_curses.KEY_BACKSPACE
		$KEY_ENTER = nata_curses.KEY_ENTER
	, status, strip, system.arguments.size() > 0 ? system.arguments[0] : null
	invalid = false
	invalidate = @ if !invalid
		:invalid = true
		loop.post(@
			::invalid = false
			tasks.each(@(x) x(
			main = vi.buffer().view
			main.move(0, 0, size[0], size[1] - 1
			main.into_view(main.position().text
			vi.prepare(
			sh = strip.size().y
			limit = size[1] - 2
			sh > limit && (sh = limit
			strip.move(0, size[1] - sh, size[0], sh
			strip.into_view(strip.position().text
			mh = size[1] - sh
			if popup
				popup.move(0, 0, size[0], 0
				limit = size[1] - sh - 1
				ph = popup.size().y
				ph > limit && (ph = limit
				hy = main.head().y
				bh = main.size().y
				bh = (mh > bh ? mh : bh) - hy
				if bh < ph + 1
					y = size[1] - sh - ph
					main.move(0, 0, size[0], y + bh - 1
					main.into_view(hy, bh
				else
					main.move(0, 0, size[0], mh
					main.into_view(hy, ph + 1
					y = hy - main.top() + 1
				popup.move(0, y, size[0], ph
				popup.into_view(popup.position().text
			else
				main.move(0, 0, size[0], mh
			main.render(
			strip.render(
			popup && popup.render(
			vi.current().focus(
			nata_curses.flush(
	assist = :assist.new(Object + @
		$loop = loop
		$hooks = hooks
		$status = status
		$vi = vi
		$mode__ = @(x) ::mode = x
		$popup__ = @(x) ::popup = x
		$invalidate = invalidate
		$popup = @ Popup(0, 0, size[0], 0
		$chooser = @(done) Chooser(0, 0, size[0], 0, done
	assist("clangd", "clangd", '("--log=verbose"), '(), @(buffer) buffer.syntax && buffer.syntax.type == "cpp"
	loop.poll(0, suisha.POLLIN, @(events) if (events & suisha.POLLIN) != 0
		mode(vi.current().get(
		invalidate(
	loop.poll(resized, suisha.POLLIN, @(events) if (events & suisha.POLLIN) != 0
		nata_curses.read_resized(
		:size = nata_curses.size(
		invalidate(
	invalidate(
	loop.run(

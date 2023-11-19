system = Module("system"
io = Module("io"
time = Module("time"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_syntax = Module("../ntvi/syntax"

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

open = @(path, action)
	reader = io.Reader(io.File("" + path, "r"), "utf-8"
	try
		action(reader
	finally
		reader.close(

read = @(text, path) open(path, @(reader) while true
	s = reader.read(256
	s == "" && break
	text.replace(text.size(), 0, s

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
	nata_syntax.initialize(4
	text = nata.Text(
	path = system.arguments.size() > 0 ? system.arguments[0] : null
	if path === null
		text.replace(0, -1, "{
  \"hello\": [\"Hello\", \"World!\"],
  \"by\": [\"This\", \"is\", \"shin.\"],
  \"bye\": [\"Good\", \"bye.\"]
}
"
	else
		read(text, path
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
	done = false
	timers = [
	tasks = [
	message = ""
	syntax = nata_syntax.new(text, path, view
	syntax !== null && tasks.push(@
		current = syntax.step(
		current === null && return
		:message = "running: " + current * 100 / text.size() + "%"
		timers.push('(time.now(), @
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
	while !done
		now = time.now(
		ts = timers
		timers = [
		ts.each(@(x) x[0] > now ? timers.push(x) : x[1](
		tasks.each(@(x) x(
		if message != ""
			status.replace(0, -1, message
			message = ""
		else
			update_status(
		view.into_view(view.position().text
		view.render(
		strip.render(
		view.focus(
		nata_curses.flush(
		timeout = -1
		timers.each(@(x)
			t = Integer((x[0] - now) * 1000.0
			(timeout < 0 || t < timeout) && (:timeout = t)
		view.timeout(timeout
		try
			c = view.get(
		catch Throwable t
			continue
		try
			action = actions[c]
		catch Throwable t
			(c == nata_curses.KEY_ENTER || c == 0xd) && (c = 0xa)
			edit(@ session.merge(view.position().text, 0, String.from_code(c
			continue
		action(

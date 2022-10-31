system = Module("system"
io = Module("io"
nata = Module("nata"
natacurses = Module("natacurses"

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
	with(natacurses.OverlayIterator(overlay), @(i)
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

nata.main(@ natacurses.main(@
	natacurses.define_pair(1, natacurses.COLOR_WHITE, -1
	natacurses.define_pair(2, natacurses.COLOR_BLACK, natacurses.COLOR_WHITE
	natacurses.define_pair(3, -1, natacurses.COLOR_YELLOW
	natacurses.define_pair(4, natacurses.COLOR_BLUE, -1
	attribute_comment = natacurses.color_pair(4
	natacurses.define_pair(5, natacurses.COLOR_YELLOW, -1
	attribute_keyword = natacurses.color_pair(5
	token_comment = natacurses.Token(attribute_comment
	token_keyword = natacurses.Token(attribute_keyword
	text = nata.Text(
	if system.arguments.size() > 0
		read(text, system.arguments[0]
	else
		text.replace(0, -1, "Hello, World!\nThis is shin.\nGood bye.\n"
	size = natacurses.size(
	view = natacurses.View(text, 0, 0, size[0], size[1] - 1
	view.attributes(
		natacurses.A_DIM | natacurses.color_pair(1
		natacurses.A_DIM | natacurses.color_pair(2
	highlight = natacurses.Overlay(view, natacurses.color_pair(3
	selection = natacurses.Overlay(view, natacurses.A_REVERSE
	status = nata.Text(
	strip = natacurses.View(status, 0, size[1] - 1, size[0], 1
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
		search = nata.Search(text
		search.pattern(
			"(#.*(?:\\n|$))|(?:\\b(?:(if|for|in|break|continue|return)|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done))\\b)"
			nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		painter = natacurses.Painter(view
		creaser = natacurses.Creaser(view
		more = @ painter.current() < text.size()
		step = @ if more()
			for i = 0; i < UNIT; i = i + 1
				(match = search.next()).size() > 0 || break
				type = 1
				while type < match.size() && match[type].count <= 0
					type = type + 1
				a = match[0].from - painter.current()
				b = match[0].count
				painter.push(null, a, 64
				painter.push(type == 1 ? token_comment : token_keyword, b, 64
				close = @
					creaser.push(a
					creaser.close(
					creaser.push(b
				if type == 3
					if creaser.tag() == 4
						close(
						creaser.open(4
					else
						creaser.push(a + b
				else if type == 4 || type == 5 || type == 6
					creaser.push(a + b
					creaser.open(type
				else if type == 7 || type == 8 || type == 9
					if creaser.tag() == type - 3
						close(
					else
						creaser.push(a + b
				else
					creaser.push(a + b
			if match.size() > 0
				painter.flush(
				::message = "running: " + painter.current() * 100 / text.size() + "%"
			else
				painter.push(null, -1, 64
				painter.flush(
				creaser.push(-1
				creaser.flush(
				::message = ""
		search.reset(0, -1
		exports = (Object + @
			$more
			$step
		)(
		exports.more = more
		exports.step = step
		exports
	)(
	done = false
	actions = {
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
		natacurses.KEY_RESIZE: @
			size = natacurses.size(
			view.move(0, 0, size[0], size[1] - 1
			strip.move(0, size[1] - 1, size[0], 1
		natacurses.KEY_DOWN: @
			commit(
			line = view.row().line
			line + 1 < view.size().line && view.line__(line + 1
		natacurses.KEY_UP: @
			commit(
			line = view.row().line
			line > 0 && view.line__(line - 1
		natacurses.KEY_LEFT: @
			position = view.position().text
			position > 0 && view.position__(position - 1, false
		natacurses.KEY_RIGHT: @
			position = view.position().text
			position < text.size() && view.position__(position + 1, true
		natacurses.KEY_BACKSPACE: @
			position = view.position().text
			if position > 0
				view.folded(position - 1, false
				edit(@ session.merge(position - 1, 1, ""
		natacurses.KEY_F1: @
			view.position__(view.folded(view.position().text, true), false
		natacurses.KEY_F2: @
			view.folded(view.position().text, false
		natacurses.KEY_F3: @
			pattern = ""
			each(selection, @(p, n) :pattern = pattern + text.slice(p, n
			selection.paint(0, -1, false
			pattern == "" && return
			with(nata.Search(text), @(search)
				search.pattern(pattern, nata.Search.ECMASCRIPT
				search.reset(0, -1
				while (match = search.next()).size() > 0
					highlight.paint(match[0].from, match[0].count, true
		natacurses.KEY_F4: @
			highlight.paint(0, -1, false
		natacurses.KEY_F5: @
			position = view.position().text
			if position < text.size()
				selection.paint(position, 1, true
				view.position__(position + 1, true
		natacurses.KEY_F6: @
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
				(c == natacurses.KEY_ENTER || c == 0xd) && (c = 0xa)
				edit(@ session.merge(view.position().text, 0, String.from_code(c
			action !== null && action(
			view.into_view(view.position().text
		syntax.step(
		if message != ""
			status.replace(0, -1, message
		else if c !== null || !syntax.more()
			update_status(

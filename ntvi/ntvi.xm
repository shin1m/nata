system = Module("system"
io = Module("io"
time = Module("time"
nata = Module("nata"
natacurses = Module("natacurses"
natavi = Module("natavi"

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
	syntax = @(text, view, message)
		UNIT = 4
		search = nata.Search(text
		search.pattern(
			"(#.*(?:\\n|$))|(?:\\b(?:(if|for|in|break|continue|return)|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done))\\b)"
			nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		painter = natacurses.Painter(view
		creaser = natacurses.Creaser(view
		search.reset(0, -1
		painter.reset(
		creaser.reset(
		more = @ painter.current() < text.size()
		@
			more() || return
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
				message("running: " + painter.current() * 100 / text.size() + "%"
			else
				painter.push(null, -1, 64
				painter.flush(
				creaser.push(-1
				creaser.flush(
			more() && timers.push('(time.now(), @
	done = false
	timers = [
	tasks = [
	view = @(text, x, y, width, height)
		view = natacurses.View(text, x, y, width, height
		view.attributes(
			natacurses.A_DIM | natacurses.color_pair(1
			natacurses.A_DIM | natacurses.color_pair(2
		view
	size = natacurses.size(
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
			main = view(text, 0, 0, size[0], size[1] - 1
			tasks.push(syntax(text, main, @(x) vi.message(x
			main
		$strip_view = @(text) view(text, 0, size[1] - 1, size[0], 1
		$timeout = @(timeout, action)
			x = '(time.now() + timeout / 1000.0, action
			timers.push(x
			@
				for i = 0;; i = i + 1
					i < timers.size() || return
					timers[i] === x && break
				timers.remove(i
		$selection = @(view) natacurses.Overlay(view, natacurses.A_REVERSE
		$overlay_iterator = natacurses.OverlayIterator
		$KEY_BACKSPACE = natacurses.KEY_BACKSPACE
		$KEY_ENTER = natacurses.KEY_ENTER
	)(), system.arguments.size() > 0 ? system.arguments[0] : null
	main = vi.main(
	main__ = @(x) if x !== main
		main = x
		x.move(0, 0, size[0], size[1] - 1
	strip = vi.strip(
	strip__ = @(x) if x !== strip
		strip = x
		x.move(0, size[1] - 1, size[0], 1
	while !done
		now = time.now(
		ts = timers
		timers = [
		ts.each(@(x) x[0] > now ? timers.push(x) : x[1](
		tasks.each(@(x) x(
		vi.render(
		main.render(
		strip.render(
		current = vi.current(
		timeout = -1
		timers.each(@(x)
			t = Integer((x[0] - now) * 1000.0
			(timeout < 0 || t < timeout) && (:timeout = t)
		current.timeout(timeout
		vi.message(""
		try
			c = current.get(
		catch Throwable t
			continue
		if c == natacurses.KEY_RESIZE
			size = natacurses.size(
			main.move(0, 0, size[0], size[1] - 1
			strip.move(0, size[1] - 1, size[0], 1
		else
			vi(c
			main__(vi.main(
			strip__(vi.strip(

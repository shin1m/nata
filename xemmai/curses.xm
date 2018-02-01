system = Module("system"
io = Module("io"
nata = Module("nata"

read = @(text, path)
	reader = io.Reader(io.File(path, "r"), "utf-8"
	try
		while true
			s = reader.read(256
			if s == "": break
			text.replace(text.size(), 0, s
	finally
		reader.close(

with = @(x, f)
	try
		f(x
	finally
		x.dispose(

each = @(overlay, f)
	with(nata.OverlayIterator(overlay), @(i)
		while (x = i.next()) !== null: if x[0]: f(x[1], x[2]

nata.main(@() nata.curses(@
	nata.define_pair(1, nata.COLOR_WHITE, -1
	nata.define_pair(2, nata.COLOR_BLACK, nata.COLOR_WHITE
	nata.define_pair(3, -1, nata.COLOR_YELLOW
	nata.define_pair(4, nata.COLOR_BLUE, -1
	attribute_comment = nata.color_pair(4
	nata.define_pair(5, nata.COLOR_YELLOW, -1
	attribute_keyword = nata.color_pair(5
	text = nata.Text(
	if system.arguments.size() > 0
		read(text, system.arguments[0]
	else
		text.replace(0, -1, "Hello, World!\nThis is shin.\nGood bye.\n"
	view = nata.View(text
	view.attributes(
		nata.A_DIM | nata.color_pair(1
		nata.A_DIM | nata.color_pair(2
	highlight = nata.Overlay(view, nata.color_pair(3
	selection = nata.Overlay(view, nata.A_REVERSE
	syntax = (@
		UNIT = 4
		search = nata.Search(text
		search.pattern(
			"(#.*(?:\\n|$))|(?:\\b(?:(if|for|in|break|continue|return)|(else)|(then)|(case)|(do)|(elif|fi)|(esac)|(done))\\b)"
			nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		painter = nata.Painter(view
		folder = nata.Folder(view
		exports = Object(
		exports.message = ""
		exports.more = @() painter.current() < text.size()
		exports.step = @
			if !exports.more(): return
			for i = 0; i < UNIT; i = i + 1
				if (match = search.next()).size() <= 0: break
				type = 1
				while type < match.size() && match[type][1] <= 0
					type = type + 1
				a = match[0][0] - painter.current()
				b = match[0][1]
				painter.push(nata.A_NORMAL, a, 64
				painter.push(type == 1 ? attribute_comment : attribute_keyword, b, 64
				close = @
					folder.push(a
					folder.close(
					folder.push(b
				if type == 3
					if folder.tag() == 4
						close(
						folder.open(4
					else
						folder.push(a + b
				else if type == 4 || type == 5 || type == 6
					folder.push(a + b
					folder.open(type
				else if type == 7 || type == 8 || type == 9
					if folder.tag() == type - 3
						close(
					else
						folder.push(a + b
				else
					folder.push(a + b
			if match.size() > 0
				painter.flush(
				exports.message = "running: " + painter.current() * 100 / text.size() + "%"
			else
				painter.push(nata.A_NORMAL, -1, 64
				painter.flush(
				folder.push(-1
				folder.flush(
				exports.message = ""
		search.reset(
		painter.reset(
		folder.reset(
		exports
	)(
	while true
		view.render(
		nata.timeout(syntax.more() ? 0 : -1
		try
			c = nata.get(
			if c == 0x1b
				break
			else if c == nata.KEY_RESIZE
				view.resize(
			else if c == nata.KEY_DOWN
				line = view.line() + 1
				if line < text.lines(): view.line__(line
			else if c == nata.KEY_UP
				line = view.line(
				if line > 0: view.line__(line - 1
			else if c == nata.KEY_LEFT
				position = view.position(
				if position > 0: view.position__(position - 1, false
			else if c == nata.KEY_RIGHT
				position = view.position(
				if position < text.size(): view.position__(position + 1, true
			else if c == nata.KEY_BACKSPACE
				position = view.position(
				if position > 0
					view.folded(position - 1, false
					text.replace(position - 1, 1, ""
			else if c == nata.KEY_F1
				view.position__(view.folded(view.position(), true), false
			else if c == nata.KEY_F2
				view.folded(view.position(), false
			else if c == nata.KEY_F3
				pattern = ""
				each(selection, @(p, n) :pattern = pattern + text.slice(p, n
				selection.paint(0, text.size(), false
				if pattern != ""
					with(nata.Search(text), @(search)
						search.pattern(pattern, nata.Search.ECMASCRIPT
						search.reset(
						while (match = search.next()).size() > 0
							highlight.paint(match[0][0], match[0][1], true
			else if c == nata.KEY_F4
				highlight.paint(0, text.size(), false
			else if c == nata.KEY_F5
				position = view.position(
				if position < text.size()
					selection.paint(position, 1, true
					view.position__(position + 1, true
			else if c == nata.KEY_F6
				selection.paint(0, text.size(), false
			else
				if c == nata.KEY_ENTER || c == 0xd: c = 0xa
				text.replace(view.position(), 0, String.from_code(c
			view.into_view(view.row(
		catch Throwable e
		syntax.step(

system = Module("system"
nata = Module("nata"

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
	nata.define_pair(4, nata.COLOR_RED, -1
	text = nata.Text(
	text.replace(0, -1, "Hello, World!\nThis is shin.\nGood bye.\n"
	view = nata.View(text
	view.attributes(
		nata.A_DIM | nata.color_pair(1
		nata.A_DIM | nata.color_pair(2
	highlight = nata.Overlay(view, nata.color_pair(3
	selection = nata.Overlay(view, nata.A_REVERSE
	while true
		view.render(
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
		else if c == nata.KEY_F7
			each(selection, @(p, n) view.paint(p, n, nata.A_BOLD | nata.color_pair(4
			selection.paint(0, text.size(), false
		else if c == nata.KEY_F8
			view.paint(1, nata.A_NORMAL
			selection.paint(0, text.size(), false
		else if c == nata.KEY_F9
			each(selection, @(p, n) view.foldable(p, n, true
			selection.paint(0, text.size(), false
		else if c == nata.KEY_F10
			each(selection, @(p, n) view.foldable(p, n, false
			selection.paint(0, text.size(), false
		else
			if c == nata.KEY_ENTER || c == 0xd: c = 0xa
			text.replace(view.position(), 0, String.from_code(c
		view.into_view(view.row(

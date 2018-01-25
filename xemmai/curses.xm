system = Module("system"
nata = Module("nata"

nata.curses(@() nata.main(@
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
	view.add_overlay(nata.color_pair(3
	view.add_overlay(nata.A_REVERSE
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
			search = nata.Search(text
			try
				search.pattern("all", nata.Search.ECMASCRIPT
				while (match = search.next()).size() > 0
					view.overlay(0, match[0][0], match[0][1], true
			finally
				search.dispose(
		else if c == nata.KEY_F4
			view.overlay(0, 0, text.size(), false
		else if c == nata.KEY_F5
			position = view.position(
			if position < text.size()
				view.overlay(1, position, 1, true
				view.position__(position + 1, true
		else if c == nata.KEY_F6
			view.overlay(1, 0, text.size(), false
		else if c == nata.KEY_F7
			view.paint(1, nata.A_BOLD | nata.color_pair(4
			view.overlay(1, 0, text.size(), false
		else if c == nata.KEY_F8
			view.paint(1, nata.A_NORMAL
			view.overlay(1, 0, text.size(), false
		else if c == nata.KEY_F9
			view.foldable(1, true
			view.overlay(1, 0, text.size(), false
		else if c == nata.KEY_F10
			view.foldable(1, false
			view.overlay(1, 0, text.size(), false
		else
			if c == nata.KEY_ENTER || c == 0xd: c = 0xa
			text.replace(view.position(), 0, String.from_code(c
		view.into_view(view.row(

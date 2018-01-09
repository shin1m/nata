system = Module("system"
nata = Module("nata"

nata.curses(@() nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello, World!\nThis is shin.\nGood bye.\n"
	view = nata.View(text
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
			if position > 0: text.replace(position - 1, 1, ""
		else
			if c == nata.KEY_ENTER || c == 0xd: c = 0xa
			text.replace(view.position(), 0, String.from_code(c
		view.into_view(view.row(

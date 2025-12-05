system = Module("system"
print = system.out.write_line
nata = Module("nata"
utilities = Module("utilities"
remove = utilities.remove
core = Module("core"

Text = nata.Text + @
	$replaced
	$__initialize = @
		nata.Text.__initialize[$](
		$replaced = [
	$replace = @(p, n, s)
		size = $size(
		nata.Text.replace[$](p, n, s
		(n < 0 || n > size) && (n = size)
		$replaced.each(@(x) x(p, n, s.size(

Position = Object + @
	$text
	$x
	$width
	$__initialize = @(text, x, width)
		$text = text
		$x = x
		$width = width

Row = Object + @
	$index
	$line
	$text
	$x
	$y
	$__initialize = @(index, line, text, x, y)
		$index = index
		$line = line
		$text = text
		$x = x
		$y = y

View = Object + @
	$text
	$_height
	$_position
	$_target
	$_top
	$__initialize = @(text)
		$text = text
		$text.replaced.push($
		$_height = 3
		$_position = $_target = $_top = 0
	$dispose = @ remove($text.replaced, $
	$__call = @(p, n0, n1) $_position < p || $position__(($_position < p + n0 ? p : $_position - n0) + n1, false
	$size = @
		lines = $text.lines(
		size = $text.size(
		Row(lines, lines, size, size, lines
	$height = @ $_height
	$range = @ $text.lines() - 1
	$top = @ $_top
	$position = @ Position($_position, $_position, 1
	$position__ = @(p, forward)
		$_position = p
		$_target = p - $text.line_at_text(p).from
	to_row = @(line) Row(line.index, line.index, line.from, line.from, line.index
	$row = @ to_row($text.line_at_text($_position
	$row_at = @(x)
		n = $text.lines(
		to_row($text.line_at(x < n ? x : n - 1
	$row_at_line = @(x) $row_at(x
	$row_at_y = @(x) $row_at(x
	$from_line = @(line)
		n = line.count - 1
		x = $_target
		$_position = line.from + (x < 0 || x > n ? n : x
	$line__ = @(x) $from_line($text.line_at(x
	$target__ = @(x)
		$_target = x
		$from_line($text.line_at_text($_position
	$into_view = @(p, h = null) h && ($_top = p
	$folded = @(p, on)

Span = Object + @
	$from
	$count
	$value
	$__initialize = @(from, count, value)
		$from = from
		$count = count
		$value = value

Overlay = Object + @
	$text
	$values
	$__initialize = @(view)
		$text = view.text
		$text.replaced.push($
		$values = [
		$replace(0, 0, $text.size(), false
	$dispose = @ remove($text.replaced, $
	$replace = @(p, n0, n1, on)
		for ; n0 > 0; n0 = n0 - 1; $values.remove(p
		for ; n1 > 0; n1 = n1 - 1; $values.insert(p, on
	$__call = @(p, n0, n1) $replace(p, n0, n1, false
	$paint = @(p, n, on) $replace(p, n, n, on

OverlayIterator = Object + @
	$values
	$from
	$__initialize = @(overlay)
		$values = overlay.values
		$from = 0
	$dispose = @
	$next = @
		n = $values.size(
		i = $from
		i < n || return
		x = $values[i]
		while
			i = i + 1
			i < n && $values[i] == x || break
		span = Span($from, i - $from, x
		$from = i
		span

$test = @(name, text, f)
	print(name
	done = false
	timers = [
	status = Text(
	vi = core.new(Object + @
		$quit = @ ::done = true
		$buffer = @(path, maps)
			text = Text(
			path && text.replace(text.size(), 0, path
			view = View(text
			core.Buffer(path, maps, text, view, Overlay(view
		$timeout = @(timeout, action)
			x = '(timeout, action
			timers.push(x
			@ remove(timers, x
		$overlay_iterator = OverlayIterator
		$KEY_BACKSPACE = 0x1008
		$KEY_ENTER = 0x100a
	, status, View(status), text
	f(vi
		@(s)
			n = s.size(
			for i = 0; i < n; i = i + 1
				c = s.code_at(i
				if c == 0
					ts = timers
					:timers = [
					ts.each(@(x) x[1](
					continue
				if c == 0x5e
					i = i + 1
					c = s.code_at(i
					c == 0x5e || (c = c - 0x40)
				vi(c
		@ if !done
			vi.prepare(
			status = :status.slice(0, -1
			timers.each(@(x) :status = status + " t" + x[0]
			print("\t" + status
			status

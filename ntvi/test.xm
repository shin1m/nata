system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"
natavi = Module("natavi"

Text = nata.Text + @
	$replaced
	$__initialize = @
		nata.Text.__initialize[$](
		$replaced = [
	$replace = @(p, n, s)
		nata.Text.replace[$](p, n, s
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

remove = @(xs, x)
	n = xs.size(
	for i = 0; i < n; i = i + 1
		if xs[i] == x
			xs.remove(i
			break

View = Object + @
	$text
	$_position
	$_top
	$__initialize = @(text)
		$text = text
		$text.replaced.push($
		$_position = $_top = 0
	$dispose = @ remove($text.replaced, $
	$__call = @(p, n0, n1) $_position < p || ($_position = ($_position < p + n0 ? p : $_position - n0) + n1)
	$position = @ Position($_position, $_position, 1
	$position__ = @(p, forward) $_position = p
	$row = @
		line = $text.line_at_in_text($_position
		Row(line.index, line.index, line.from, line.from, line.index
	$range = @
		n = $text.lines(
		n < 24 ? 0 : n - 24
	$top = @ $_top
	$into_view = @(p)
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
		for ; n0 > 0; n0 = n0 - 1: $values.remove(p
		for ; n1 > 0; n1 = n1 - 1: $values.insert(p, on
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
		while true
			i = i + 1
			i < n && $values[i] == x || break
		span = Span($from, i - $from, x
		$from = i
		span

letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")

test = @(name, text, f)
	print(name
	done = false
	vi = natavi.new((Object + @
		$quit = @ ::done = true
		$text = Text
		$read = @(text, path) text.replace(text.size(), 0, path
		$main_view = View
		$strip_view = View
		$selection = Overlay
		$overlay_iterator = OverlayIterator
		$KEY_BACKSPACE = 0x1008
		$KEY_ENTER = 0x100a
	)(), text
	f(vi, @
		done && return
		vi.render(
		status = vi.strip().text.slice(0, -1
		print("\t" + status
		status

nata.main(@ test("quit", null, @(vi, update)
	assert(update() == "NORMAL 0,0-0 100% <0> "
	vi(letter("Z"
	assert(update() == "NORMAL 0,0-0 100% <0> Z"
	vi(letter("Z"
	assert(update() === null

nata.main(@ test("undo & redo", "abc", @(vi, update)
	vi(letter("d"
	vi(letter("l"
	vi(letter("l"
	vi(letter("l"
	vi(letter("p"
	vi(letter("h"
	vi(letter("d"
	vi(letter("h"
	vi(letter("h"
	vi(letter("p"
	assert(update() == "NORMAL 0,1-1 100% <4> "
	assert(vi.main().text.slice(0, -1) == "cba"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <3|1> "
	assert(vi.main().text.slice(0, -1) == "ba"
	vi(letter("u"
	assert(update() == "NORMAL 0,2-2 100% <2|2> "
	assert(vi.main().text.slice(0, -1) == "bca"
	vi(letter("u"
	assert(update() == "NORMAL 0,2-2 100% <1|3> "
	assert(vi.main().text.slice(0, -1) == "bc"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|4> "
	assert(vi.main().text.slice(0, -1) == "abc"
	vi(control("R"
	assert(update() == "NORMAL 0,0-0 100% <1|3> "
	assert(vi.main().text.slice(0, -1) == "bc"
	vi(control("R"
	assert(update() == "NORMAL 0,3-3 100% <2|2> "
	assert(vi.main().text.slice(0, -1) == "bca"
	vi(control("R"
	assert(update() == "NORMAL 0,1-1 100% <3|1> "
	assert(vi.main().text.slice(0, -1) == "ba"
	vi(control("R"
	assert(update() == "NORMAL 0,1-1 100% <4> "
	assert(vi.main().text.slice(0, -1) == "cba"
	vi(letter("3"
	vi(letter("u"
	assert(update() == "NORMAL 0,2-2 100% <1|3> "
	assert(vi.main().text.slice(0, -1) == "bc"
	vi(letter("2"
	vi(control("R"
	assert(update() == "NORMAL 0,1-1 100% <3|1> "
	assert(vi.main().text.slice(0, -1) == "ba"

nata.main(@ test("insert", null, @(vi, update)
	vi(letter("2"
	assert(update() == "NORMAL 0,0-0 100% <0> 2"
	vi(letter("i"
	assert(update() == "INSERT 0,0-0 100% <0> 2i"
	vi(letter("h"
	assert(update() == "INSERT 0,1-1 100% <0?2> 2ih"
	assert(vi.main().text.slice(0, -1) == "h"
	vi(letter("e"
	assert(update() == "INSERT 0,2-2 100% <0?2> 2ihe"
	assert(vi.main().text.slice(0, -1) == "he"
	vi(control("H"
	assert(update() == "INSERT 0,1-1 100% <0?2> 2ihe\b"
	assert(vi.main().text.slice(0, -1) == "h"
	vi(letter("i"
	assert(update() == "INSERT 0,2-2 100% <0?2> 2ihe\bi"
	assert(vi.main().text.slice(0, -1) == "hi"
	vi(control("["
	assert(update() == "NORMAL 0,4-4 100% <1> "
	assert(vi.main().text.slice(0, -1) == "hihi"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == ""
	vi(control("R"
	assert(update() == "NORMAL 0,4-4 100% <1> "
	assert(vi.main().text.slice(0, -1) == "hihi"
	vi(letter("."
	assert(update() == "NORMAL 0,8-8 100% <2> "
	assert(vi.main().text.slice(0, -1) == "hihihihi"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,14-14 100% <3> "
	assert(vi.main().text.slice(0, -1) == "hihihihihihihi"

nata.main(@ test("delete motion", "abcdefghijklmnopqr", @(vi, update)
	vi(letter("2"
	vi(letter("d"
	vi(letter("3"
	assert(update() == "NORMAL 0,0-0 100% <0> 2d3"
	vi(letter("l"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ghijklmnopqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghijklmnopqr"
	vi(control("R"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ghijklmnopqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,0-0 100% <2> "
	assert(vi.main().text.slice(0, -1) == "pqr"

nata.main(@ test("change motion", "abcdefghijklmnopqr", @(vi, update)
	vi(letter("2"
	vi(letter("c"
	vi(letter("3"
	vi(letter("l"
	assert(update() == "INSERT 0,0-0 100% <0?2> 2c3l"
	assert(vi.main().text.slice(0, -1) == "ghijklmnopqr"
	vi(letter("A"
	vi(letter("B"
	vi(letter("C"
	assert(update() == "INSERT 0,3-3 100% <0?2> 2c3lABC"
	assert(vi.main().text.slice(0, -1) == "ABCghijklmnopqr"
	vi(control("["
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABCghijklmnopqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghijklmnopqr"
	vi(control("R"
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABCghijklmnopqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,6-6 100% <2> "
	assert(vi.main().text.slice(0, -1) == "ABCABCpqr"

nata.main(@ test("yank motion & paste", "abcdefghijklmnopqr", @(vi, update)
	vi(letter("3"
	vi(letter("y"
	vi(letter("2"
	vi(letter("l"
	assert(update() == "NORMAL 0,0-0 100% <0> "
	vi(letter("2"
	vi(letter("p"
	assert(update() == "NORMAL 0,12-12 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abcdefabcdefabcdefghijklmnopqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghijklmnopqr"
	vi(control("R"
	assert(update() == "NORMAL 0,12-12 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abcdefabcdefabcdefghijklmnopqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,30-30 100% <2> "
	assert(vi.main().text.slice(0, -1) == "abcdefabcdefabcdefabcdefabcdefabcdefghijklmnopqr"

nata.main(@ test("delete lines", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, update)
	vi(letter("2"
	vi(letter("d"
	vi(letter("d"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ghi\njkl\nmno\npqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	vi(control("R"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ghi\njkl\nmno\npqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,0-0 100% <2> "
	assert(vi.main().text.slice(0, -1) == "pqr"

nata.main(@ test("change lines", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, update)
	vi(letter("2"
	vi(letter("c"
	vi(letter("c"
	assert(update() == "INSERT 0,0-0 100% <0?2> 2cc"
	assert(vi.main().text.slice(0, -1) == "\nghi\njkl\nmno\npqr"
	vi(letter("A"
	vi(letter("B"
	vi(letter("C"
	assert(update() == "INSERT 0,3-3 100% <0?2> 2ccABC"
	assert(vi.main().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	vi(control("["
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	vi(control("R"
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,3-3 100% <2> "
	assert(vi.main().text.slice(0, -1) == "ABC\nmno\npqr"

nata.main(@ test("yank lines & paste", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, update)
	vi(letter("2"
	vi(letter("y"
	vi(letter("y"
	assert(update() == "NORMAL 0,0-0 100% <0> "
	vi(letter("2"
	vi(letter("p"
	assert(update() == "NORMAL 4,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	vi(control("R"
	assert(update() == "NORMAL 4,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 10,0-0 100% <2> "
	assert(vi.main().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"

nata.main(@ test("delete selection", "abcdefghi", @(vi, update)
	vi(letter("v"
	vi(letter("3"
	vi(letter("l"
	assert(update() == "VISUAL 0,3-3 100% <0> "
	vi(letter("d"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "defghi"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghi"
	vi(control("R"
	assert(update() == "NORMAL 0,0-0 100% <1> "
	assert(vi.main().text.slice(0, -1) == "defghi"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,0-0 100% <2> "
	assert(vi.main().text.slice(0, -1) == "ghi"

nata.main(@ test("change selection", "abcdefghi", @(vi, update)
	vi(letter("v"
	vi(letter("3"
	vi(letter("l"
	assert(update() == "VISUAL 0,3-3 100% <0> "
	vi(letter("c"
	assert(update() == "INSERT 0,0-0 100% <0?2> v3 c"
	assert(vi.main().text.slice(0, -1) == "defghi"
	vi(letter("A"
	vi(letter("B"
	vi(letter("C"
	assert(update() == "INSERT 0,3-3 100% <0?2> v3 cABC"
	assert(vi.main().text.slice(0, -1) == "ABCdefghi"
	vi(control("["
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABCdefghi"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghi"
	vi(control("R"
	assert(update() == "NORMAL 0,3-3 100% <1> "
	assert(vi.main().text.slice(0, -1) == "ABCdefghi"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,6-6 100% <2> "
	assert(vi.main().text.slice(0, -1) == "ABCABCghi"

nata.main(@ test("yank selection & paste", "abcdefghi", @(vi, update)
	vi(letter("v"
	vi(letter("3"
	vi(letter("l"
	assert(update() == "VISUAL 0,3-3 100% <0> "
	vi(letter("y"
	assert(update() == "NORMAL 0,0-0 100% <0> "
	vi(letter("2"
	vi(letter("p"
	assert(update() == "NORMAL 0,6-6 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abcabcabcdefghi"
	vi(letter("u"
	assert(update() == "NORMAL 0,0-0 100% <0|1> "
	assert(vi.main().text.slice(0, -1) == "abcdefghi"
	vi(control("R"
	assert(update() == "NORMAL 0,6-6 100% <1> "
	assert(vi.main().text.slice(0, -1) == "abcabcabcdefghi"
	vi(letter("3"
	vi(letter("."
	assert(update() == "NORMAL 0,15-15 100% <2> "
	assert(vi.main().text.slice(0, -1) == "abcabcabcabcabcabcdefghi"

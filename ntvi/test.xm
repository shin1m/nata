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

remove = @(xs, x)
	n = xs.size(
	for i = 0; i < n; i = i + 1
		xs[i] === x && break xs.remove(i

View = Object + @
	$text
	$_position
	$_target
	$_top
	$__initialize = @(text)
		$text = text
		$text.replaced.push($
		$_position = $_target = $_top = 0
	$dispose = @ remove($text.replaced, $
	$__call = @(p, n0, n1) $_position < p || $position__(($_position < p + n0 ? p : $_position - n0) + n1, false
	$position = @ Position($_position, $_position, 1
	$position__ = @(p, forward)
		$_position = p
		$_target = p - $text.line_at_in_text(p).from
	$target__ = @(x)
		$_target = x
		line = $text.line_at_in_text($_position
		n = line.count - 1
		$_position = line.from + (x < 0 || x > n ? n : x)
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

test = @(name, text, f)
	print(name
	done = false
	timers = [
	status = Text(
	vi = natavi.new(Object + @
		$quit = @ ::done = true
		$buffer = @(path, maps)
			text = Text(
			path !== null && text.replace(text.size(), 0, path
			view = View(text
			natavi.Buffer(path, maps, text, view, Overlay(view
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
					return
				if c == 0x5e
					i = i + 1
					c = s.code_at(i
					c == 0x5e || (c = c - 0x40)
				vi(c
		@
			done && return
			vi.render(
			status = :status.slice(0, -1
			timers.each(@(x) :status = status + " t" + x[0]
			print("\t" + status
			status

nata.main(@ test("quit", null, @(vi, type, update)
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("Z"
	assert(update() == "NORMAL 1,1-1 100% <0> Z"
	type("Z"
	assert(update() === null

nata.main(@ test("just 0 moves to the head", "abc", @(vi, type, update)
	type("lll"
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("0"
	assert(update() == "NORMAL 1,1-1 100% <0> "

nata.main(@ test("undo & redo", "abc", @(vi, type, update)
	type("dlllphdhhp"
	assert(update() == "NORMAL 1,2-2 100% <4> "
	assert(vi.buffer().text.slice(0, -1) == "cba"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <3|1> "
	assert(vi.buffer().text.slice(0, -1) == "ba"
	type("u"
	assert(update() == "NORMAL 1,3-3 100% <2|2> "
	assert(vi.buffer().text.slice(0, -1) == "bca"
	type("u"
	assert(update() == "NORMAL 1,3-3 100% <1|3> "
	assert(vi.buffer().text.slice(0, -1) == "bc"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|4> "
	assert(vi.buffer().text.slice(0, -1) == "abc"
	type("^R"
	assert(update() == "NORMAL 1,1-1 100% <1|3> "
	assert(vi.buffer().text.slice(0, -1) == "bc"
	type("^R"
	assert(update() == "NORMAL 1,4-4 100% <2|2> "
	assert(vi.buffer().text.slice(0, -1) == "bca"
	type("^R"
	assert(update() == "NORMAL 1,2-2 100% <3|1> "
	assert(vi.buffer().text.slice(0, -1) == "ba"
	type("^R"
	assert(update() == "NORMAL 1,2-2 100% <4> "
	assert(vi.buffer().text.slice(0, -1) == "cba"
	type("3u"
	assert(update() == "NORMAL 1,3-3 100% <1|3> "
	assert(vi.buffer().text.slice(0, -1) == "bc"
	type("2^R"
	assert(update() == "NORMAL 1,2-2 100% <3|1> "
	assert(vi.buffer().text.slice(0, -1) == "ba"

nata.main(@ test("insert", null, @(vi, type, update)
	type("2"
	assert(update() == "NORMAL 1,1-1 100% <0> 2"
	type("i"
	assert(update() == "INSERT 1,1-1 100% <0> "
	type("h"
	assert(update() == "INSERT 1,2-2 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "h"
	type("e"
	assert(update() == "INSERT 1,3-3 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "he"
	type("^H"
	assert(update() == "INSERT 1,2-2 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "h"
	type("i"
	assert(update() == "INSERT 1,3-3 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "hi"
	type("^["
	assert(update() == "NORMAL 1,5-5 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "hihi"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == ""
	type("^R"
	assert(update() == "NORMAL 1,5-5 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "hihi"
	type("."
	assert(update() == "NORMAL 1,9-9 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "hihihihi"
	type("3."
	assert(update() == "NORMAL 1,15-15 100% <3> "
	assert(vi.buffer().text.slice(0, -1) == "hihihihihihihi"

nata.main(@ test("delete motion", "abcdefghijklmnopqr", @(vi, type, update)
	type("2d3"
	assert(update() == "NORMAL 1,1-1 100% <0> 2d3"
	type("l"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ghijklmnopqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghijklmnopqr"
	type("^R"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ghijklmnopqr"
	type("3."
	assert(update() == "NORMAL 1,1-1 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "pqr"

nata.main(@ test("change motion", "abcdefghijklmnopqr", @(vi, type, update)
	type("2c3l"
	assert(update() == "INSERT 1,1-1 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "ghijklmnopqr"
	type("ABC"
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "ABCghijklmnopqr"
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABCghijklmnopqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghijklmnopqr"
	type("^R"
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABCghijklmnopqr"
	type("3."
	assert(update() == "NORMAL 1,7-7 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "ABCABCpqr"

nata.main(@ test("yank motion & paste", "abcdefghijklmnopqr", @(vi, type, update)
	type("3y2l"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("2p"
	assert(update() == "NORMAL 1,13-13 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefabcdefabcdefghijklmnopqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghijklmnopqr"
	type("^R"
	assert(update() == "NORMAL 1,13-13 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefabcdefabcdefghijklmnopqr"
	type("3."
	assert(update() == "NORMAL 1,31-31 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefabcdefabcdefabcdefabcdefabcdefghijklmnopqr"

nata.main(@ test("delete lines", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, type, update)
	type("2dd"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ghi\njkl\nmno\npqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	type("^R"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ghi\njkl\nmno\npqr"
	type("3."
	assert(update() == "NORMAL 1,1-1 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "pqr"

nata.main(@ test("change lines", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, type, update)
	type("2cc"
	assert(update() == "INSERT 1,1-1 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "\nghi\njkl\nmno\npqr"
	type("ABC"
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	type("^R"
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABC\nghi\njkl\nmno\npqr"
	type("3."
	assert(update() == "NORMAL 1,4-4 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "ABC\nmno\npqr"

nata.main(@ test("yank lines & paste", "abc\ndef\nghi\njkl\nmno\npqr", @(vi, type, update)
	type("2yy"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("2p"
	assert(update() == "NORMAL 5,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nghi\njkl\nmno\npqr"
	type("^R"
	assert(update() == "NORMAL 5,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"
	type("3."
	assert(update() == "NORMAL 11,1-1 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "abc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nabc\ndef\nghi\njkl\nmno\npqr"

nata.main(@ test("delete selection", "abcdefghi", @(vi, type, update)
	type("v3l"
	assert(update() == "VISUAL 1,4-4 100% <0> "
	type("d"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "defghi"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghi"
	type("^R"
	assert(update() == "NORMAL 1,1-1 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "defghi"
	type("3."
	assert(update() == "NORMAL 1,1-1 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "ghi"

nata.main(@ test("change selection", "abcdefghi", @(vi, type, update)
	type("v3l"
	assert(update() == "VISUAL 1,4-4 100% <0> "
	type("c"
	assert(update() == "INSERT 1,1-1 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "defghi"
	type("ABC"
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "ABCdefghi"
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABCdefghi"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghi"
	type("^R"
	assert(update() == "NORMAL 1,4-4 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "ABCdefghi"
	type("3."
	assert(update() == "NORMAL 1,7-7 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "ABCABCghi"

nata.main(@ test("yank selection & paste", "abcdefghi", @(vi, type, update)
	type("v3l"
	assert(update() == "VISUAL 1,4-4 100% <0> "
	type("y"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("2p"
	assert(update() == "NORMAL 1,7-7 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abcabcabcdefghi"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abcdefghi"
	type("^R"
	assert(update() == "NORMAL 1,7-7 100% <1> "
	assert(vi.buffer().text.slice(0, -1) == "abcabcabcdefghi"
	type("3."
	assert(update() == "NORMAL 1,16-16 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "abcabcabcabcabcabcdefghi"

nata.main(@ test("search forward", "abcdefghiabcdefghi", @(vi, type, update)
	type("/"
	assert(update() == "/"
	assert(vi.buffer().view.position().text == 0
	type("d"
	assert(update() == "/d"
	assert(vi.buffer().view.position().text == 3
	type("f"
	assert(update() == "/df"
	assert(vi.buffer().view.position().text == 0
	type("^H"
	assert(update() == "/d"
	assert(vi.buffer().view.position().text == 3
	type("e"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 3
	type("^M"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 3
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("n"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 12
	type("^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("n"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3
	type("N"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 12
	type("N"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("search backward", "abcdefghiabcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("?"
	assert(update() == "?"
	assert(vi.buffer().view.position().text == 18
	type("d"
	assert(update() == "?d"
	assert(vi.buffer().view.position().text == 12
	type("f"
	assert(update() == "?df"
	assert(vi.buffer().view.position().text == 18
	type("^H"
	assert(update() == "?d"
	assert(vi.buffer().view.position().text == 12
	type("e"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 12
	type("^M"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 12
	type("^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("n"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("n"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 12
	type("N"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3
	type("N"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 12

nata.main(@ test("builtin map C", "abcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(3, false
	type("C"
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().view.text.slice(0, -1) == "abc"

nata.main(@ test("builtin map s", "abcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(3, false
	type("2s"
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().view.text.slice(0, -1) == "abcfghi"

nata.main(@ test("map", "abcdefghi", @(vi, type, update)
	type(":"
	assert(update() == ":"
	type("map __ l."
	assert(update() == ":map __ l."
	type("^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type(":map _ya iYAH!^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("_ya"
	assert(update() == "INSERT 1,5-5 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "YAH!abcdefghi"
	type("^["
	assert(update() == "NORMAL 1,5-5 100% <1> "
	type("_"
	assert(update() == "NORMAL 1,5-5 100% <1> _"
	type("_"
	assert(update() == "NORMAL 1,10-10 100% <2> "
	assert(vi.buffer().text.slice(0, -1) == "YAH!aYAH!bcdefghi"
	type(":map^M"
	assert(update() == "maps\n__ l.\n_ya iYAH!"
	type(":unmap __^M:map^M"
	assert(update() == "maps\n_ya iYAH!"
	type(":unmap _ya^M:map^M"
	assert(update() == "maps"

nata.main(@ test("map ambiguous", "abc", @(vi, type, update)
	type(":map _ $^M"
	type(":map _, l^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("_"
	assert(update() == "NORMAL 1,1-1 100% <0> _ t1000"
	type(","
	assert(update() == "NORMAL 1,2-2 100% <0> "
	type("_"
	assert(update() == "NORMAL 1,2-2 100% <0> _ t1000"
	type(String.from_code(0
	assert(update() == "NORMAL 1,4-4 100% <0> "

nata.main(@ test("map ambiguous operator pending", "abcdefghi", @(vi, type, update)
	type(":map c, 3l^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type("c"
	assert(update() == "NORMAL 1,1-1 100% <0> c t1000"
	type(","
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("cc"
	assert(update() == "NORMAL 1,4-4 100% <0> cc t1000"
	type(","
	assert(update() == "INSERT 1,4-4 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == "abcghi"

nata.main(@ test("buffers", "foo", @(vi, type, update)
	type(":buffers^M"
	assert(update() == "buffers\n1 % \"foo\""
	assert(vi.buffer().text.slice(0, -1) == "foo"
	type(":edit bar^M"
	assert(vi.buffer().text.slice(0, -1) == "bar"
	type(":buffers^M"
	assert(update() == "buffers\n1   \"foo\"\n2 % \"bar\""
	type(":edit foo^M"
	assert(vi.buffer().text.slice(0, -1) == "foo"
	type(":buffers^M"
	assert(update() == "buffers\n1 % \"foo\"\n2   \"bar\""
	type(":quit^M"
	assert(vi.buffer().text.slice(0, -1) == "bar"
	type(":buffers^M"
	assert(update() == "buffers\n1 % \"bar\""

nata.main(@ test("insert literally", "", @(vi, type, update)
	type("i^V"
	assert(update() == "INSERT 1,1-1 100% <0> ^V"
	assert(vi.buffer().text.slice(0, -1) == ""
	type("^["
	assert(update() == "INSERT 1,2-2 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b
	type("^V03"
	assert(update() == "INSERT 1,2-2 100% <0?2> ^V03"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b)
	type("2"
	assert(update() == "INSERT 1,3-3 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " "
	type("^V33"
	assert(update() == "INSERT 1,3-3 100% <0?2> ^V33"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " "
	type("a"
	assert(update() == "INSERT 1,5-5 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a"
	type("^Vo04"
	assert(update() == "INSERT 1,5-5 100% <0?2> ^Vo04"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a"
	type("2"
	assert(update() == "INSERT 1,6-6 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\""
	type("^Vo43"
	assert(update() == "INSERT 1,6-6 100% <0?2> ^Vo43"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\""
	type("8"
	assert(update() == "INSERT 1,8-8 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\"#8"
	type("^Vx2"
	assert(update() == "INSERT 1,8-8 100% <0?2> ^Vx2"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\"#8"
	type("4"
	assert(update() == "INSERT 1,9-9 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\"#8$"
	type("^Vxf"
	assert(update() == "INSERT 1,9-9 100% <0?2> ^Vxf"
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\"#8$"
	type("g"
	assert(update() == "INSERT 1,11-11 100% <0?2> "
	assert(vi.buffer().text.slice(0, -1) == String.from_code(0x1b) + " !a\"#8$" + String.from_code(0xf) + "g"

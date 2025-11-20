assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"
testing = Module("testing"
test = testing.test

nata.main(@ test("/ searches forward", "abcdefghi", @(vi, type, update)
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

nata.main(@ test("/ searches forward count times", "abcabcabc", @(vi, type, update)
	type("2/"
	assert(update() == "/"
	assert(vi.buffer().view.position().text == 0
	type("ab"
	assert(update() == "/ab"
	assert(vi.buffer().view.position().text == 6
	type("^M"
	assert(update() == "/ab"
	assert(vi.buffer().view.position().text == 6
	type("2/ab^M"
	assert(update() == "continuing at TOP: ab"
	assert(vi.buffer().view.position().text == 3
	type("4/ab^M"
	assert(update() == "continuing at TOP: ab"
	assert(vi.buffer().view.position().text == 6

nata.main(@ test("just / repeats the latest /", "abcdefabcdefabcdef", @(vi, type, update)
	type("/^M"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("/^M"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 9
	type("^["
	assert(update() == "NORMAL 1,10-10 100% <0> "
	type("2/^M"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("n repeats the latest /", "abcdefabcdefabcdef", @(vi, type, update)
	type("n"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("n"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 9
	type("^["
	assert(update() == "NORMAL 1,10-10 100% <0> "
	type("2n"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("N repeats the latest / in opposite direction", "abcdefabcdefabcdef", @(vi, type, update)
	type("N"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("N"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 15
	type("2N"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("? searches backward", "abcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(9, false
	type("?"
	assert(update() == "?"
	assert(vi.buffer().view.position().text == 9
	type("d"
	assert(update() == "?d"
	assert(vi.buffer().view.position().text == 3
	type("f"
	assert(update() == "?df"
	assert(vi.buffer().view.position().text == 9
	type("^H"
	assert(update() == "?d"
	assert(vi.buffer().view.position().text == 3
	type("e"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3
	type("^M"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <0> "

nata.main(@ test("? searches backward count times", "abcabcabc", @(vi, type, update)
	assert(vi.buffer().view.position__(9, false
	type("2?"
	assert(update() == "?"
	assert(vi.buffer().view.position().text == 9
	type("ab"
	assert(update() == "?ab"
	assert(vi.buffer().view.position().text == 3
	type("^M"
	assert(update() == "?ab"
	assert(vi.buffer().view.position().text == 3
	type("2?ab^M"
	assert(update() == "continuing at BOTTOM: ab"
	assert(vi.buffer().view.position().text == 6
	type("4?ab^M"
	assert(update() == "continuing at BOTTOM: ab"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("just ? repeats the latest ?", "abcdefabcdefabcdef", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("?^M"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,16-16 100% <0> "
	type("?^M"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 9
	type("^["
	assert(update() == "NORMAL 1,10-10 100% <0> "
	type("2?^M"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 15

nata.main(@ test("n repeats the latest ?", "abcdefabcdefabcdef", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("n"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,16-16 100% <0> "
	type("n"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 9
	type("^["
	assert(update() == "NORMAL 1,10-10 100% <0> "
	type("2n"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 15

nata.main(@ test("N repeats the latest ? in opposite direction", "abcdefabcdefabcdef", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("N"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,16-16 100% <0> "
	type("N"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3
	type("2N"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 15

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

nata.main(@ test("just / repeats the latest /", "abcdefghiabcdefghi", @(vi, type, update)
	type("/^M"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("/^M"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 12
	type("^["
	assert(update() == "NORMAL 1,13-13 100% <0> "

nata.main(@ test("n repeats the latest /", "abcdefghiabcdefghi", @(vi, type, update)
	type("n"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("n"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 12
	type("^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("n"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3

nata.main(@ test("N repeats the latest / in opposite direction", "abcdefghiabcdefghi", @(vi, type, update)
	type("N"
	assert(update() == "no last pattern"
	type("/de^M^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("N"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 12
	type("N"
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

nata.main(@ test("just ? repeats the latest ?", "abcdefghiabcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("?^M"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("?^M"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <0> "

nata.main(@ test("n repeats the latest ?", "abcdefghiabcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("n"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("n"
	assert(update() == "?de"
	assert(vi.buffer().view.position().text == 3
	type("^["
	assert(update() == "NORMAL 1,4-4 100% <0> "
	type("n"
	assert(update() == "continuing at BOTTOM: de"
	assert(vi.buffer().view.position().text == 12

nata.main(@ test("N repeats the latest ? in opposite direction", "abcdefghiabcdefghi", @(vi, type, update)
	assert(vi.buffer().view.position__(18, false
	type("N"
	assert(update() == "no last pattern"
	type("?de^M^["
	assert(update() == "NORMAL 1,13-13 100% <0> "
	type("N"
	assert(update() == "continuing at TOP: de"
	assert(vi.buffer().view.position().text == 3
	type("N"
	assert(update() == "/de"
	assert(vi.buffer().view.position().text == 12

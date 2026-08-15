assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"
testing = Module("testing"
test = testing.test

nata.main(@ test("m sets mark at cursor position", "abc\nabc\nabc", @(vi, type, update)
	type("jlma"
	assert(update() == "NORMAL 2,2-2 0% <0> "
	type(":marks^M"
	assert(update() == "marks\n a 2 2"
	type("jhmb"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	type(":marks^M"
	assert(update() == "marks\n a 2 2\n b 3 1"

nata.main(@ test("marks lists the marks", "", @(vi, type, update)
	type("mambmcmdme:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 1\n c 1 1\n d 1 1\n e 1 1"
	type(":marks bd^M"
	assert(update() == "marks\n b 1 1\n d 1 1"

nata.main(@ test("marks lists the marks in the range", "", @(vi, type, update)
	type("mambmcmdme:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 1\n c 1 1\n d 1 1\n e 1 1"
	type(":marks b-d^M"
	assert(update() == "marks\n b 1 1\n c 1 1\n d 1 1"

nata.main(@ test("delm deletes the mark", "abc\nabc\nabc", @(vi, type, update)
	type("jlmajhmb"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	type(":marks^M"
	assert(update() == "marks\n a 2 2\n b 3 1"
	type(":delm a^M"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	type(":marks^M"
	assert(update() == "marks\n b 3 1"

nata.main(@ test("delm deletes the marks", "", @(vi, type, update)
	type("mambmcmdme:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 1\n c 1 1\n d 1 1\n e 1 1"
	type(":delm bd^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n c 1 1\n e 1 1"

nata.main(@ test("delm deletes the marks in the range", "", @(vi, type, update)
	type("mambmcmdme:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 1\n c 1 1\n d 1 1\n e 1 1"
	type(":delm ac-d^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type(":marks^M"
	assert(update() == "marks\n b 1 1\n e 1 1"

nata.main(@ test("delm! deletes all marks", "", @(vi, type, update)
	type("mambmc:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 1\n c 1 1"
	type(":delm!^M"
	assert(update() == "NORMAL 1,1-1 100% <0> "
	type(":marks^M"
	assert(update() == "marks"

nata.main(@ test("` jumps to the mark", "abc\nabc\nabc", @(vi, type, update)
	type("jlma"
	assert(update() == "NORMAL 2,2-2 0% <0> "
	type("jh"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	type("`a"
	assert(update() == "NORMAL 2,2-2 0% <0> "
	type("jhd`a"
	assert(update() == "NORMAL 2,2-2 0% <1>* "
	assert(vi.buffer().text.slice(0, -1) == "abc\naabc"

nata.main(@ test("' jumps to the mark linewise", "abc\n abc\nabc", @(vi, type, update)
	type("jllma"
	assert(update() == "NORMAL 2,3-3 0% <0> "
	type("j0"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	type("'a"
	assert(update() == "NORMAL 2,2-2 0% <0> "
	type("j0d'a"
	assert(update() == "NORMAL 2,1-1 0% <1>* "
	assert(vi.buffer().text.slice(0, -1) == "abc\n"

nata.main(@ test("marks are adjusted by changes", "abcde", @(vi, type, update)
	type("malmblmclmdlmelmf:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 2\n c 1 3\n d 1 4\n e 1 5\n f 1 6"
	type("0l3x"
	assert(update() == "NORMAL 1,2-2 100% <1>* "
	assert(vi.buffer().text.slice(0, -1) == "ae"
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n b 1 2\n e 1 2\n f 1 3"
	type("ibcd^["
	assert(update() == "NORMAL 1,5-5 100% <2>* "
	assert(vi.buffer().text.slice(0, -1) == "abcde"
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n b 1 5\n e 1 5\n f 1 6"

nata.main(@ test("marks are restored by undo/redo", "abc", @(vi, type, update)
	type("malmblmc:marks^M"
	assert(update() == "marks\n a 1 1\n b 1 2\n c 1 3"
	type("02x"
	assert(update() == "NORMAL 1,1-1 100% <1>* "
	assert(vi.buffer().text.slice(0, -1) == "c"
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n c 1 1"
	type("u"
	assert(update() == "NORMAL 1,1-1 100% <0|1> "
	assert(vi.buffer().text.slice(0, -1) == "abc"
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n b 1 2\n c 1 3"
	type(":delm a^M:marks^M"
	assert(update() == "marks\n b 1 2\n c 1 3"
	type("^R"
	assert(update() == "NORMAL 1,1-1 100% <1>* "
	assert(vi.buffer().text.slice(0, -1) == "c"
	type(":marks^M"
	assert(update() == "marks\n a 1 1\n c 1 1"
	type("u:marks^M"
	assert(update() == "marks\n a 1 3\n b 1 2\n c 1 3"

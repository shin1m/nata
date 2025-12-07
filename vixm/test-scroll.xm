assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"
testing = Module("testing"
test = testing.test

nata.main(@ test("CTRL-E scrolls window count lines downwards", "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10", @(vi, type, update)
	type("2j"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0
	type("^E"
	assert(update() == "NORMAL 3,1-1 10% <0> "
	assert(vi.buffer().view.top() == 1
	type("3^E"
	assert(update() == "NORMAL 5,1-1 40% <0> "
	assert(vi.buffer().view.top() == 4
	type("7^E"
	assert(update() == "NORMAL 11,1-1 100% <0> "
	assert(vi.buffer().view.top() == 10

nata.main(@ test("CTRL-Y scrolls window count lines upwards", "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n10", @(vi, type, update)
	type("9j"
	vi.buffer().view.into_view(8, 1
	assert(update() == "NORMAL 10,1-1 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("^Y"
	assert(update() == "NORMAL 10,1-1 70% <0> "
	assert(vi.buffer().view.top() == 7
	type("3^Y"
	assert(update() == "NORMAL 7,1-1 40% <0> "
	assert(vi.buffer().view.top() == 4
	type("5^Y"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("CTRL-D scrolls window downwards", "0\n1\n2\n3\n 4\n5\n6\n7\n8\n  9\n   10", @(vi, type, update)
	type("2j"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0
	type("^D"
	assert(update() == "NORMAL 5,2-2 20% <0> "
	assert(vi.buffer().view.top() == 2
	type("5^D"
	assert(update() == "NORMAL 10,3-3 70% <0> "
	assert(vi.buffer().view.top() == 7
	type("2^D"
	assert(update() == "NORMAL 11,4-4 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("l^D"
	assert(update() == "NORMAL 11,5-5 80% <0> "
	assert(vi.buffer().view.top() == 8

nata.main(@ test("CTRL-U scrolls window upwards", " 0\n1\n2\n3\n4\n5\n6\n  7\n8\n9\n10", @(vi, type, update)
	type("9j"
	vi.buffer().view.into_view(8, 1
	assert(update() == "NORMAL 10,1-1 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("^U"
	assert(update() == "NORMAL 8,3-3 60% <0> "
	assert(vi.buffer().view.top() == 6
	type("9^U"
	assert(update() == "NORMAL 1,2-2 0% <0> "
	assert(vi.buffer().view.top() == 0
	type("l^U"
	assert(update() == "NORMAL 1,3-3 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("CTRL-F scrolls window count pages downwards", "0\n1\n2\n 3\n4\n5\n6\n7\n8\n  9\n   10", @(vi, type, update)
	vi.buffer().view._height = 5
	type("2j"
	assert(update() == "NORMAL 3,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0
	type("^F"
	assert(update() == "NORMAL 4,2-2 30% <0> "
	assert(vi.buffer().view.top() == 3
	type("2^F"
	assert(update() == "NORMAL 10,3-3 90% <0> "
	assert(vi.buffer().view.top() == 9
	type("^F"
	assert(update() == "NORMAL 11,4-4 100% <0> "
	assert(vi.buffer().view.top() == 10
	type("l^F"
	assert(update() == "NORMAL 11,5-5 100% <0> "
	assert(vi.buffer().view.top() == 10

nata.main(@ test("CTRL-B scrolls window count pages upwards", "0\n1\n2\n3\n 4\n5\n6\n7\n8\n  9\n10", @(vi, type, update)
	vi.buffer().view._height = 5
	type("9j"
	vi.buffer().view.into_view(8, 1
	assert(update() == "NORMAL 10,1-1 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("^B"
	assert(update() == "NORMAL 10,3-3 50% <0> "
	assert(vi.buffer().view.top() == 5
	type("2^B"
	assert(update() == "NORMAL 5,2-2 0% <0> "
	assert(vi.buffer().view.top() == 0
	type("l^B"
	assert(update() == "NORMAL 5,3-3 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("z<CR> shows line at top of window with ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5z^M"
	assert(update() == "NORMAL 5,2-2 40% <0> "
	assert(vi.buffer().view.top() == 4
	type("kz^M"
	assert(update() == "NORMAL 4,3-3 30% <0> "
	assert(vi.buffer().view.top() == 3
	type("12z^M"
	assert(update() == "NORMAL 11,4-4 100% <0> "
	assert(vi.buffer().view.top() == 10
	type("1z^M"
	assert(update() == "NORMAL 1,5-5 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("zt shows line at top of window without ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5zt"
	assert(update() == "NORMAL 5,1-1 40% <0> "
	assert(vi.buffer().view.top() == 4
	type("kzt"
	assert(update() == "NORMAL 4,1-1 30% <0> "
	assert(vi.buffer().view.top() == 3
	type("12zt"
	assert(update() == "NORMAL 11,1-1 100% <0> "
	assert(vi.buffer().view.top() == 10
	type("1zt"
	assert(update() == "NORMAL 1,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("z. shows line at center of window with ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5z."
	assert(update() == "NORMAL 5,2-2 30% <0> "
	assert(vi.buffer().view.top() == 3
	type("kz."
	assert(update() == "NORMAL 4,3-3 20% <0> "
	assert(vi.buffer().view.top() == 2
	type("12z."
	assert(update() == "NORMAL 11,4-4 90% <0> "
	assert(vi.buffer().view.top() == 9
	type("1z."
	assert(update() == "NORMAL 1,5-5 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("zz shows line at center of window without ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5zz"
	assert(update() == "NORMAL 5,1-1 30% <0> "
	assert(vi.buffer().view.top() == 3
	type("kzz"
	assert(update() == "NORMAL 4,1-1 20% <0> "
	assert(vi.buffer().view.top() == 2
	type("12zz"
	assert(update() == "NORMAL 11,1-1 90% <0> "
	assert(vi.buffer().view.top() == 9
	type("1zz"
	assert(update() == "NORMAL 1,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("z- shows line at bottom of window with ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5z-"
	assert(update() == "NORMAL 5,2-2 20% <0> "
	assert(vi.buffer().view.top() == 2
	type("kz-"
	assert(update() == "NORMAL 4,3-3 10% <0> "
	assert(vi.buffer().view.top() == 1
	type("12z-"
	assert(update() == "NORMAL 11,4-4 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("1z-"
	assert(update() == "NORMAL 1,5-5 0% <0> "
	assert(vi.buffer().view.top() == 0

nata.main(@ test("zb shows line at bottom of window without ^", "    0\n1\n2\n  3\n 4\n5\n6\n7\n8\n9\n   10", @(vi, type, update)
	type("5zb"
	assert(update() == "NORMAL 5,1-1 20% <0> "
	assert(vi.buffer().view.top() == 2
	type("kzb"
	assert(update() == "NORMAL 4,1-1 10% <0> "
	assert(vi.buffer().view.top() == 1
	type("12zb"
	assert(update() == "NORMAL 11,1-1 80% <0> "
	assert(vi.buffer().view.top() == 8
	type("1zb"
	assert(update() == "NORMAL 1,1-1 0% <0> "
	assert(vi.buffer().view.top() == 0

system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"

nata.main(@
	text = nata.Text(
	print(text.slice(0, -1
	assert(text.slice(0, -1) == ""
	text.replace(0, -1, "Hello."
	print(text.slice(0, -1
	assert(text.slice(0, -1) == "Hello."
	text.replace(5, 1, ", World!"
	print(text.slice(0, -1
	assert(text.slice(0, -1) == "Hello, World!"

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "abc\nde"
	assert(text.lines() == 2
	l = text.line_at(0
	assert(l.index == 0
	assert(l.from == 0
	assert(l.count == 4
	l = text.line_at(1
	assert(l.index == 1
	assert(l.from == 4
	assert(l.count == 3
	try
		text.line_at(2
		assert(false
	catch Throwable t
		assert(t.__string() == "out of range."

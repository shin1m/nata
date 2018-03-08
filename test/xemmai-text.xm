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

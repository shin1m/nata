system = Module("system"
print = system.out.write_line
assert = @(x) if !x: throw Throwable("Assertion failed."
nata = Module("nata"

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello, World!"
	search = nata.Search(text
	search.pattern("(or)?l+", nata.Search.ECMASCRIPT
	search.reset(
	match = search.next(
	print(match
	assert(match == '('(2, 2), '(13, 0
	print(text.slice(match[0][0], match[0][1]
	match = search.next(
	print(match
	assert(match == '('(8, 3), '(8, 2
	print(text.slice(match[0][0], match[0][1]
	match = search.next(
	print(match
	assert(match == '(

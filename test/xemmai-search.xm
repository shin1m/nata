system = Module("system"
print = system.out.write_line
assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"

dump = @(xs)
	ys = [
	xs.each(@(x) ys.push('(x.from, x.count
	'(*ys

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello, World!"
	search = nata.Search(text
	search.pattern("(or)?l+", nata.Search.ECMASCRIPT
	search.reset(0, -1
	match = search.next(
	print(match
	assert(dump(match) == '('(2, 2), '(13, 0
	print(text.slice(match[0].from, match[0].count
	match = search.next(
	print(match
	assert(dump(match) == '('(8, 3), '(8, 2
	print(text.slice(match[0].from, match[0].count
	match = search.next(
	print(match
	assert(match == '(

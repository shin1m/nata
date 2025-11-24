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
	pattern = nata.Pattern("(or)?l+", nata.Pattern.ECMASCRIPT
	match = pattern.search(text, 0, -1
	print(match
	assert(dump(match) == '('(2, 2), '(13, 0
	print(text.slice(match[0].from, match[0].count
	match = pattern.search(text, match[0].from + match[0].count, -1
	print(match
	assert(dump(match) == '('(8, 3), '(8, 2
	print(text.slice(match[0].from, match[0].count
	match = pattern.search(text, match[0].from + match[0].count, -1
	print(match
	assert(match == '(

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello, World!"
	pattern = nata.Pattern("(or)?l+", nata.Pattern.ECMASCRIPT
	search = nata.Search(text, pattern
	match = search.first(0, -1
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

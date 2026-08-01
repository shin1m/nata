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

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello."
	Mark = Object + @ $key
	m0 = Mark(
	m0.key = text.mark_at_text_with(0, m0
	print(m0.key
	assert(m0.key == 0.25
	m1 = Mark(
	m1.key = text.mark_at_text_with(5, m1
	print(m1.key
	assert(m1.key == 0.625
	m2 = Mark(
	m2.key = text.mark_at_text_with(5, m2
	print(m2.key
	assert(m2.key == 0.6875
	m3 = Mark(
	m3.key = text.mark_at_text_with(6, m3
	print(m3.key
	assert(m3.key == 0.8125
	marks = text.marks_in_text_range(0, 0
	assert(marks.size() == 0
	marks = text.marks_in_text_range(0, 1
	assert(marks.size() == 1
	assert(marks[0] === m0
	marks = text.marks_in_text_range(1, 1
	assert(marks.size() == 0
	marks = text.marks_in_text_range(5, 1
	assert(marks.size() == 2
	assert(marks[0] === m1
	assert(marks[1] === m2
	marks = text.marks_in_text_range(6, 1
	assert(marks.size() == 1
	assert(marks[0] === m3
	marks = text.marks_in_text_range(0, -1
	assert(marks.size() == 4
	assert(marks[0] === m0
	assert(marks[1] === m1
	assert(marks[2] === m2
	assert(marks[3] === m3
	m = text.mark_of_key(m0.key
	assert(m[0] === m0
	assert(m[1] == 0
	m = text.mark_of_key(m1.key
	assert(m[0] === m1
	assert(m[1] == 5
	m = text.mark_of_key(m2.key
	assert(m[0] === m2
	assert(m[1] == 5
	m = text.mark_of_key(m3.key
	assert(m[0] === m3
	assert(m[1] == 6
	text.unmark_by_key(m0.key
	assert(text.marks_in_text_range(0, 1).size() == 0
	text.unmark_by_key(m1.key
	marks = text.marks_in_text_range(5, 1
	assert(marks.size() == 1
	assert(marks[0] === m2
	text.unmark_by_key(m2.key
	assert(text.marks_in_text_range(5, 1).size() == 0
	text.unmark_by_key(m3.key
	assert(text.marks_in_text_range(6, 1).size() == 0

nata.main(@
	text = nata.Text(
	text.replace(0, -1, "Hello."
	Mark = Object + @ $key
	m0 = Mark(
	m0.key = text.mark_at_text_with(0, m0
	m1 = Mark(
	m1.key = text.mark_at_text_with(5, m1
	m2 = Mark(
	m2.key = text.mark_at_text_with(5, m2
	text.replace(0, 0, "Hi, "
	m = text.mark_of_key(m0.key
	assert(m[0] === m0
	assert(m[1] == 4
	m = text.mark_of_key(m1.key
	assert(m[0] === m1
	assert(m[1] == 9
	m = text.mark_of_key(m2.key
	assert(m[0] === m2
	assert(m[1] == 9
	text.replace(0, 4, ""
	m = text.mark_of_key(m0.key
	assert(m[0] === m0
	assert(m[1] == 0
	m = text.mark_of_key(m1.key
	assert(m[0] === m1
	assert(m[1] == 5
	m = text.mark_of_key(m2.key
	assert(m[0] === m2
	assert(m[1] == 5
	text.replace(1, 0, "i, H"
	m = text.mark_of_key(m0.key
	assert(m[0] === m0
	assert(m[1] == 0
	m = text.mark_of_key(m1.key
	assert(m[0] === m1
	assert(m[1] == 9
	m = text.mark_of_key(m2.key
	assert(m[0] === m2
	assert(m[1] == 9
	text.replace(1, 9, "ello, World!"
	m = text.mark_of_key(m0.key
	assert(m[0] === m0
	assert(m[1] == 0
	m = text.mark_of_key(m1.key
	assert(m[0] !== m1
	assert(m[1] == 0
	m = text.mark_of_key(m2.key
	assert(m[0] !== m2
	assert(m[1] == 0

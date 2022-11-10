system = Module("system"
print = system.out.write_line
io = Module("io"
assert = @(x) x || throw Throwable("Assertion failed."
nata = Module("nata"
nata_tree_sitter = Module("nata-tree-sitter"
nata_tree_sitter_json = Module("nata-tree-sitter-json").language

Stream = Object + @
	$read
	$__initialize = @(read) $read = read
	$close = @

from_utf8 = @(bytes)
	remain = bytes.size(
	i = 0
	reader = io.Reader(
		Stream(@(buffer, offset, size)
			n = remain < size ? remain : size
			for j = 0; j < n; j = j + 1: buffer[offset + j] = bytes[i + j]
			:i = i + n
			:remain = remain - n
			n
	, "utf-8"
	try
		reader.read(bytes.size(
	finally
		reader.close(

nata.main(@
	text = nata.Text(
	#text.replace(0, -1, "[1, null, \"$????\"]"
	text.replace(0, -1, from_utf8('(
		0x5b
		0x31, 0x2c, 0x20
		0x6e, 0x75, 0x6c, 0x6c, 0x2c, 0x20
		0x22
		0x24
		0xc2, 0xa3
		0xe0, 0xa4, 0xb9
		0xe2, 0x82, 0xac
		0xf0, 0x90, 0x8d, 0x88
		0x22
		0x5d
	query = nata_tree_sitter.Query(nata_tree_sitter_json, "
\"{\" @begin_object
\"}\" @end_object
\"[\" @begin_array
\"]\" @end_array
[(object) (array)] @crease
"
	captures = query.captures(
	print(captures
	assert(captures == '(
		"begin_object"
		"end_object"
		"begin_array"
		"end_array"
		"crease"
	parser = nata_tree_sitter.Parser(text, query
	next = @(xs)
		match = parser.next(
		print(match
		assert(match == xs
	next('(0, 18, 4
	next('(0, 1, 2
	next('(17, 1, 3
	next(null
	#text.replace(4, 4, "{\"??\": \"??\"}"
	text.replace(4, 4, from_utf8('(
		0x7b
		0x22
		0xc2, 0xa3
		0xe0, 0xa4, 0xb9
		0x22
		0x3a
		0x20
		0x22
		0xe2, 0x82, 0xac
		0xf0, 0x90, 0x8d, 0x88
		0x22
		0x7d
	next('(0, 26, 4
	next('(0, 1, 2
	next('(4, 12, 4
	next('(4, 1, 0
	next('(15, 1, 1
	next('(25, 1, 3
	next(null

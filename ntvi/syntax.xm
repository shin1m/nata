system = Module("system"
io = Module("io"
os = Module("os"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_tree_sitter = Module("nata-tree-sitter"

open = @(path, action)
	reader = io.Reader(io.File("" + path, "r"), "utf-8"
	try
		action(reader
	finally
		reader.close(

find_index = @(text, i, predicate)
	n = text.size(
	for ; i < n; i = i + 1
		predicate(text.code_at(i)) && break
	i

read_pairs = @(path) open(path, @(reader)
	pairs = {
	while true
		line = reader.read_line(
		line == "" && break
		i = find_index(line, 0, @(x) x != 0x20
		j = find_index(line, i, @(x) x == 0x3a
		j < line.size() || continue
		name = line.substring(i, j - i
		i = find_index(line, j + 1, @(x) x != 0x20
		j = find_index(line, i, @(x) x == 0x0a || x == 0x0d
		value = line.substring(i, j - i
		pairs[name] = value
	pairs

syntax_root = os.Path(system.script) / ".." / ".." / "syntax"

try_syntax = @(text, path, type)
	parent = syntax_root / type
	detect = read_pairs(parent / "detect"
	match = @
		if path !== null && detect.has("suffix")
			suffix = detect["suffix"]
			i = path.size() - suffix.size()
			i >= 0 && path.substring(i) == suffix && return true
		if detect.has("first")
			search = nata.Search(text
			try
				search.pattern(detect["first"], nata.Search.ECMASCRIPT
				search.reset(0, -1
				search.next().size() > 0 && return true
			finally
				search.dispose(
		false
	!match() ? null : '(
		nata_tree_sitter.Query(
			Module("nata-tree-sitter-" + type).language
			open(parent / "query.scm", @(reader)
				text = ""
				while true
					s = reader.read(256
					s == "" && break
					text = text + s
				text
		read_pairs(parent / "colors"

load_syntax = @(text, path)
	root = os.Directory("" + syntax_root)
	try
		while (type = root.read()) !== null
			x = try_syntax(text, path, type.name
			x !== null && return x
	finally
		root.close(

color2token = {

define = @(pair, color, name)
	nata_curses.define_pair(pair, color, -1
	color2token[name] = nata_curses.Token(nata_curses.color_pair(pair

$initialize = @(from)
	define(from, nata_curses.COLOR_RED, "red"
	define(from + 1, nata_curses.COLOR_GREEN, "green"
	define(from + 2, nata_curses.COLOR_YELLOW, "yellow"
	define(from + 3, nata_curses.COLOR_BLUE, "blue"
	define(from + 4, nata_curses.COLOR_MAGENTA, "magenta"
	define(from + 5, nata_curses.COLOR_CYAN, "cyan"

$new = @(text, path, view, more)
	x = load_syntax(text, path
	x === null && return
	query = x[0]
	capture2color = x[1]
	parser = nata_tree_sitter.Parser(text, query
	painter = nata_curses.Painter(view
	creaser = nata_curses.Creaser(view
	paint = @(p) while true
		token = tokens[0]
		to = p < token[1] ? p : token[1]
		q = painter.current(
		q < to && painter.push(token[0], to - q, 64
		to < token[1] && break
		tokens.shift(
	captures = [
	query.captures().each(@(x)
		if x == "crease"
			captures.push(@(p, n) creaser.push(p - creaser.current(), n
		else
			if capture2color.has(x)
				color = capture2color[x]
				token = color2token.has(color) ? color2token[color] : null
			captures.push(@(p, n)
				paint(p
				tokens.unshift('(token, p + n
	UNIT = 4
	Object + @
		$dispose = @
			parser.dispose(
			query.dispose(
			painter.dispose(
			creaser.dispose(
		$step = @
			parser.parsed() || (::tokens = ['(null, text.size() + 1)])
			for i = 0; i < UNIT; i = i + 1
				if (match = parser.next()) === null
					paint(text.size(
					painter.flush(
					return creaser.end(
				captures[match[2]](match[0], match[1]
			painter.flush(
			more(text.size(), painter.current()

io = Module("io"

$remove = @(xs, x)
	n = xs.size(
	for i = 0; i < n; i = i + 1: xs[i] === x && break xs.remove(i
$do = @(f) f(
$letter = letter = @(x) x.code_at(0
$control = @(x) letter(x) - letter("@")
$each_code = @(s, f)
	n = s.size(
	for i = 0; i < n; i = i + 1: f(s.code_at(i
$find_index = @(s, i, predicate)
	n = s.size(
	for ; i < n; i = i + 1: predicate(s.code_at(i)) && break
	i
$open = @(path, action)
	reader = io.Reader(io.File("" + path, "r"), "utf-8"
	try
		action(reader
	finally
		reader.close(

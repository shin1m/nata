system = Module("system"
io = Module("io"
suisha = Module("suisha"
nata = Module("nata"
nata_curses = Module("nata-curses"
nata_syntax = Module("syntax"
natavi = Module("natavi"

Text = nata.Text + @
	$replaced
	$version
	$__initialize = @
		nata.Text.__initialize[$](
		$replaced = [
		$version = 0
	$location = @(p)
		l = $line_at_in_text(p
		'(l.index, $in_bytes(p) - $in_bytes(l.from)
	$replace = @(p, n, s)
		l0 = $location(p
		l1 = $location(p + n
		nata.Text.replace[$](p, n, s
		$version = $version + 1
		$replaced.each(@(x) x(l0[0], l0[1], l1[0], l1[1], s

View = nata_curses.View + @
	$__initialize = @(text, x, y, width, height)
		nata_curses.View.__initialize[$](text, x, y, width, height
		$attributes(
			nata_curses.A_DIM | nata_curses.color_pair(1
			nata_curses.A_DIM | nata_curses.color_pair(2

Buffer = natavi.Buffer + @
	$disposing
	$syntax
	$lsps
	$__initialize = @(path, text, view, syntax, lsps)
		natavi.Buffer.__initialize[$](path, text, view, nata_curses.Overlay(view, nata_curses.A_REVERSE
		$disposing = [
		$syntax = syntax
		$lsps = {
		lsps.each(@(k, v) v.register(:$
	$dispose = @
		$disposing.each(@(x) x(
		lsps = [
		$lsps.each(@(k, v) lsps.push(k
		lsps.each(@(x) x.unregister($
		natavi.Buffer.dispose[$](

remove = @(xs, x)
	n = xs.size(
	for i = 0; i < n; i = i + 1
		xs[i] === x && break xs.remove(i

open = @(path, action)
	reader = io.Reader(io.File("" + path, "r"), "utf-8"
	try
		action(reader
	finally
		reader.close(

read = @(text, path) open(path, @(reader) while true
	s = reader.read(256
	s == "" && break
	text.replace(text.size(), 0, s

letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")

suisha.main(@(loop) nata.main(@ nata_curses.main_with_resized(@(resized)
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	nata_syntax.initialize(4
	tasks = [
	lsps = {
	size = nata_curses.size(
	status = nata.Text(
	strip = View(status, 0, size[1] - 1, size[0], 1
	vi = natavi.new(Object + @
		$quit = loop.exit
		$buffer = @(path)
			text = Text(
			path !== null && read(text, path
			view = View(text, 0, 0, size[0], size[1] - 1
			syntax = nata_syntax.new(text, path, view
			buffer = Buffer(path, text, view, syntax, lsps
			if syntax !== null
				buffer.disposing.unshift(@
					remove(tasks, syntax.step
					syntax.dispose(
				tasks.push(@
					current = syntax.step(
					current === null && return
					vi.progress("running: " + current * 100 / text.size() + "%"
					invalidate(
			buffer
		$timeout = @(timeout, action) loop.timer(@
			action(
			invalidate(
		, timeout, true).stop
		$overlay_iterator = nata_curses.OverlayIterator
		$KEY_BACKSPACE = nata_curses.KEY_BACKSPACE
		$KEY_ENTER = nata_curses.KEY_ENTER
	, status, strip, system.arguments.size() > 0 ? system.arguments[0] : null
	verbify = @(verbs) @(i)
		match = vi.match_status("^\\s*(\\S+)", i
		match.size() > 0 || return
		group = match[1]
		verb = status.slice(group.from, group.count
		if verbs.has(verb)
			verbs[verb](group.from + group.count
		else
			vi.message("unknown verb: " + verb
	lsp_logs = @
		if lsp__logs === null
			:lsp__logs = vi.add_buffer(null
			lsp__logs.disposing.unshift(@ ::lsp__logs = null
		lsp__logs
	lsp_mode_choose = (Object + @
		done = @(ok)
			vi.current__(vi.buffer().view
			vi.mode__($caller
			$done(ok
		map = {
			control("M"): @ done[$](true
			control("["): @ done[$](false
			letter("j"): @
				line = strip.row().line
				line < strip.size().line - 1 && strip.line__(line + 1
			letter("k"): @
				line = strip.row().line
				line > 0 && strip.line__(line - 1
			nata_curses.KEY_ENTER: @ done[$](true
		$caller
		$done
		$render = @
		$__call = @(c)
			try
				map[c][$](
			catch Throwable t
	)(
	lsp_choose = @(done)
		strip.position__(0, false
		vi.current__(strip
		lsp_mode_choose.caller = vi.mode(
		lsp_mode_choose.done = done
		vi.mode__(lsp_mode_choose
	lsp_open = @(location)
		buffer = vi.open_buffer(location[0]
		text = buffer.text
		buffer.view.position__(text.in_text(text.in_bytes(text.line_at(location[1]).from) + location[2]), false
	lsp_goto = @(value, error)
		if value === null || value.@ === List && value.size() <= 0
			vi.message(error === null ? "no information" : "ERROR: " + error
		else if value.@ !== List
			lsp_open(value
		else if value.size() <= 1
			lsp_open(value[0]
		else
			status.replace(0, -1, ""
			value.each(@(x)
				s = x[0] + ": " + (x[1] + 1) + "," + (x[2] + 1)
				n = status.size(
				status.replace(n, 0, n > 0 ? "\n" + s : s
			lsp_choose(@(ok)
				vi.mode().reset(
				ok && lsp_open(value[strip.row().line]
	lsp_startup = @(file, arguments, environments, match, done)
		Module("lsp").startup(loop, invalidate, file, arguments, environments
			@(x)
				logs = lsp_logs(
				logs.begin(
				try
					logs.replace(logs.text.size(), 0, x
					logs.commit(null
				catch Throwable t
					logs.cancel(
				logs.undos.clear(
			@(client) done(client === null ? null : (Object + @
				$stop = client.shutdown
				$register = @(buffer) if match(buffer)
					text = buffer.text
					replaced = @(l0, c0, l1, c1, s) client.did_change(buffer.path, text.version, l0, c0, l1, c1, s
					buffer.lsps[$] = replaced
					text.replaced.push(replaced
					buffer.commands["lsp"] = verbify({
						"definition": @(i)
							l = text.location(buffer.view.position().text
							client.definition(buffer.path, l[0], l[1], lsp_goto
						"references": @(i)
							l = text.location(buffer.view.position().text
							client.references(buffer.path, l[0], l[1], false, lsp_goto
						"hover": @(i)
							l = text.location(buffer.view.position().text
							client.hover(buffer.path, l[0], l[1], @(value, error) vi.message(value !== null ? value : error !== null ? "ERROR: " + error : "no information"
					client.did_open(buffer.path, text.version, text.slice(0, text.size(
				$unregister = @(buffer)
					remove(buffer.text.replaced, buffer.lsps.remove($
					buffer.commands.remove("lsp"
					client.did_close(buffer.path
			)(
	vi.commands()["clangd"] = verbify({
		"start": @(i) if !lsps.has("clangd")
			lsp_startup("clangd", '("--log=verbose"), '()
				@(buffer) buffer.syntax !== null && buffer.syntax.type == "cpp"
				@(lsp)
					if lsp === null
						vi.message("clangd: failed."
					else
						lsps["clangd"] = lsp
						vi.buffers().each(lsp.register
						vi.message("clangd: started."
		"stop": @(i) if lsps.has("clangd")
			lsp = lsps.remove("clangd"
			vi.buffers().each(@(x) x.lsps.has(lsp) && lsp.unregister(x
			lsp.stop(@ vi.message("clangd: stopped."
		"log": @(i) vi.switch_buffer(lsp_logs(
	invalid = false
	invalidate = @ if !invalid
		:invalid = true
		loop.post(@
			::invalid = false
			tasks.each(@(x) x(
			vi.render(
			n = strip.size().index
			n > size[1] && (n = size[1])
			main = vi.buffer().view
			main.move(0, 0, size[0], size[1] - n
			main.into_view(main.position().text
			strip.move(0, size[1] - n, size[0], n
			strip.into_view(strip.position().text
			main.render(
			strip.render(
			vi.current().focus(
			nata_curses.flush(
	loop.poll(0, true, false, @(readable, writable) if readable
		vi(vi.current().get(
		invalidate(
	loop.poll(resized, true, false, @(readable, writable) if readable
		nata_curses.read_resized(
		:size = nata_curses.size(
		invalidate(
	invalidate(
	loop.run(

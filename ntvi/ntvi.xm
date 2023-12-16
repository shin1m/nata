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

Chooser = Object + @
	$text
	$view
	$selection
	$done
	$__initialize = @(done)
		$text = nata.Text(
		$view = View($text, 0, 0, 0, 0
		$selection = nata_curses.Overlay($view, nata_curses.A_REVERSE
		$done = done
	$dispose = @
		$selection.dispose(
		$view.dispose(
		$text.dispose(
	$at = @ $view.row().line
	$at__ = @(i)
		line = $text.line_at($view.row().line
		$selection.paint(line.from, line.count, false
		$view.line__(i
		line = $text.line_at($view.row().line
		$selection.paint(line.from, line.count, true
	map = {
		control("M"): @ $done(true
		control("["): @ $done(false
		letter("j"): @
			line = $view.row().line
			line < $view.size().line - 1 && $at__(line + 1
		letter("k"): @
			line = $view.row().line
			line > 0 && $at__(line - 1
		nata_curses.KEY_ENTER: @ $done(true
	$__call = @(c)
		try
			map[c][$](
		catch Throwable t

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
	mode = vi
	verbify = @(verbs) @(i)
		match = vi.match_status("^\\s*(\\S+)", i
		match.size() > 0 || return
		group = match[1]
		verb = status.slice(group.from, group.count
		if verbs.has(verb)
			verbs[verb](group.from + group.count
		else
			vi.message("unknown verb: " + verb
	lsp_modal = @(message, action)
		cancel = action(@ ::mode = vi
		vi.message(message + " (ESC to cancel)"
		:mode = @(c) c == control("[") && cancel(
	lsp_logs = @
		if lsp__logs === null
			:lsp__logs = vi.add_buffer(null
			lsp__logs.disposing.unshift(@ ::lsp__logs = null
		lsp__logs
	lsp_open = @(location)
		buffer = vi.open_buffer(location[0]
		text = buffer.text
		buffer.view.position__(text.in_text(text.in_bytes(text.line_at(location[1]).from) + location[2]), false
	lsp_goto = @(message, action) lsp_modal(message, @(done)
		locations = [
		append = @(x)
			s = x[0] + ": " + (x[1] + 1) + "," + (x[2] + 1)
			n = chooser.text.size(
			chooser.text.replace(n, 0, n > 0 ? "\n" + s : s
		push = @(xs)
			xs.each(locations.push
			locations.size() > 1 || return
			:chooser = Chooser(@(ok)
				ok && lsp_open(locations[chooser.at()]
				::::mode = vi
				::::popup = null
				chooser.dispose(
			locations.each(append
			:push = @(xs) xs.each(@(x)
				locations.push(x
				append(x
			chooser.at__(0
			:::mode = chooser
			:::popup = chooser.view
		action(@(xs) push(xs), @(xs, error)
			done(
			error === null || return vi.message("ERROR: " + error
			push(xs
			locations.size() > 0 || return vi.message("no information"
			vi.message(""
			chooser === null && lsp_open(locations[0]
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
			@(token, title, cancellable, message, percentage)
				s = "" + token + " " + title + ":"
				message !== null && (s = s + " " + message)
				percentage !== null && (s = s + " " + percentage + "%")
				vi.progress(s
			@(client) done(client === null ? null : (Object + @
				$stop = client.shutdown
				$register = @(buffer) if match(buffer)
					text = buffer.text
					replaced = @(l0, c0, l1, c1, s) client.did_change(buffer.path, text.version, l0, c0, l1, c1, s
					buffer.lsps[$] = replaced
					text.replaced.push(replaced
					buffer.commands["lsp"] = verbify({
						"definition": @(i) lsp_goto("definition...", @(partial, done)
							l = text.location(buffer.view.position().text
							client.definition(buffer.path, l[0], l[1], done, partial
						"references": @(i) lsp_goto("references...", @(partial, done)
							l = text.location(buffer.view.position().text
							client.references(buffer.path, l[0], l[1], false, done, partial
						"hover": @(i) lsp_modal("hover...", @(done)
							l = text.location(buffer.view.position().text
							client.hover(buffer.path, l[0], l[1], @(value, error)
								vi.message(value !== null ? value : error !== null ? "ERROR: " + error : "no information"
								done(
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
			sh = strip.size().y
			limit = size[1] - 2
			sh > limit && (sh = limit)
			strip.move(0, size[1] - sh, size[0], sh
			strip.into_view(strip.position().text
			mh = size[1] - sh
			main = vi.buffer().view
			if popup === null
				main.move(0, 0, size[0], mh
				main.into_view(main.position().text
			else
				limit = size[1] - sh - 1
				ph = popup.size().y
				ph > limit && (ph = limit)
				row = main.row().y
				b = main.size().y
				mh > b && (b = mh)
				if b - row < ph + 1
					y = size[1] - sh - ph
					main.move(0, 0, size[0], y
					main.into_view(row, 1
				else
					main.move(0, 0, size[0], mh
					main.into_view(row, ph + 1
					y = row - main.top() + 1
				popup.move(0, y, size[0], ph
				popup.into_view(popup.position().text
			main.render(
			strip.render(
			popup !== null && popup.render(
			vi.current().focus(
			nata_curses.flush(
	loop.poll(0, true, false, @(readable, writable) if readable
		mode(vi.current().get(
		invalidate(
	loop.poll(resized, true, false, @(readable, writable) if readable
		nata_curses.read_resized(
		:size = nata_curses.size(
		invalidate(
	invalidate(
	loop.run(

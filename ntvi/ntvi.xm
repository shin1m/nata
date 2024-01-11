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
	$to_location = @(x)
		l = $line_at_in_text(x
		'(l.index, $in_bytes(x) - $in_bytes(l.from)
	$from_location = @(x) $in_text($in_bytes($line_at(x.line).from) + x.character
	$replace = @(p, n, s)
		l0 = $to_location(p
		l1 = $to_location(p + n
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
	$__initialize = @(path, maps, text, view, syntax, lsps)
		natavi.Buffer.__initialize[$](path, maps, text, view, nata_curses.Overlay(view, nata_curses.A_REVERSE
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

Popup = Object + @
	$text
	$view
	$__initialize = @
		$text = nata.Text(
		$view = View($text, 0, 0, 0, 0
	$dispose = @
		$view.dispose(
		$text.dispose(

do = @(f) f(
letter = @(x) x.code_at(0
control = @(x) letter(x) - letter("@")

Chooser = Popup + @
	$selection
	$done
	$__initialize = @(done)
		Popup.__initialize[$](
		$selection = nata_curses.Overlay($view, nata_curses.A_REVERSE
		$done = done
	$dispose = @
		$selection.dispose(
		Popup.dispose[$](
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

nata_lsp = @(loop, lsps, status, vi, mode__, popup__, invalidate)
	cancellable = @(message, cancel)
		vi.message(message + " (ESC to cancel)"
		mode__(@(c) c == control("[") && cancel(
	verbify = @(verbs) @(i)
		match = vi.match_status("^\\s*(\\S+)", i
		match.size() > 0 || return
		group = match[1]
		verb = status.slice(group.from, group.count
		if verbs.has(verb)
			verbs[verb](group.from + group.count
		else
			vi.message("unknown verb: " + verb
	logs = do(@ @
		if logs === null
			:logs = vi.add_buffer(null
			logs.disposing.unshift(@ ::logs = null
		logs
	log = @(x)
		logs = :logs(
		logs.begin(
		try
			logs.replace(logs.text.size(), 0, x
			logs.commit(null
		catch Throwable t
			logs.cancel(
		logs.undos.clear(
	choose = @(message, action, popup, result2items, item2text, mode, choose, finalize)
		items = [
		append = @(x)
			s = item2text(x
			n = chooser.text.size(
			chooser.text.replace(n, 0, n > 0 ? "\n" + s : s
		push = @(result)
			result2items(result).each(items.push
			items.size() > popup || return
			:chooser = Chooser(@(ok)
				if ok
					cancel === null || cancel(
					choose(items[chooser.at()]
				else
					cancel === null || return cancel(
				::push = @(result)
				::done = @(result, error)
				mode__(vi
				popup__(null
				chooser.dispose(
				finalize(
			items.each(append
			chooser.at__(0
			mode__(mode(chooser, @
				cancel === null || cancel(
				::push = @(result)
					xs = result2items(result
					xs.size() > 0 || return
					:::items = xs
					chooser.text.replace(0, -1, ""
					items.each(append
					chooser.at__(0
					:::push = push_items
				call(
			popup__(chooser.view
			:push = push_items = @(result) result2items(result).each(@(x)
				items.push(x
				append(x
			:done = @(result, error)
				error === null || return vi.message("ERROR: " + error
				push(result
				vi.message(""
		done = @(result, error)
			mode__(vi
			if error !== null
				vi.message("ERROR: " + error
				return finalize(
			push(result
			vi.message(""
			chooser === null || return
			items.size() > 0 || return vi.message("no information"
			choose(items[0]
			finalize(
		call = @ :cancel = action(@(result) push(result), @(result, error)
			::cancel = null
			done(result, error
		cancellable(message, call(
	goto = @(message, action) choose(message, action, 1
		@(x) x
		@(x) x.path + ": " + (x.line + 1) + "," + (x.character + 1)
		@(chooser, reload) chooser
		@(location)
			buffer = vi.open_buffer(location.path
			text = buffer.text
			buffer.view.position__(text.from_location(location), false
		@
	literal = @(c)
		vi(0x16
		if c >= 0x30 && c < 0x3a
			vi(0x58
			vi(0x33
			vi(c
		else if c == 0x4f
			vi(0x58
			vi(0x34
			vi(0x46
		else if c == 0x58
			vi(0x58
			vi(0x35
			vi(0x38
		else if c == 0x6f
			vi(0x58
			vi(0x36
			vi(0x46
		else if c == 0x78
			vi(0x58
			vi(0x37
			vi(0x38
		else
			vi(c
	completion = do(@ @(client, buffer, popup) if running === null
		:running = true
		choose("completion..."
			@(partial, done)
				l = buffer.text.to_location(buffer.view.position().text
				client.completion(buffer.path, l[0], l[1], done, partial
			popup
			@(x)
				x.incomplete !== null && (:incomplete = x.incomplete)
				x.items
			@(x) x.label
			@(chooser, reload) @(c)
				c == control("I") && return chooser(control("M"
				c == control("[") && return chooser(c
				vi(c
				#if incomplete
				#	::incomplete = null
				#	reload(
				reload(
			@(x)
				times = @(n, f) for ; n > 0; n = n - 1: f(
				p = buffer.view.position().text
				text = buffer.text
				if x.edit === null
					# match and delete identifier
					s = x.insert
					n = s.size(
					n > p && (n = p)
					while n > 0 && text.slice(p - n, n) != s.substring(0, n): n = n - 1
					s = s.substring(n
				else
					times(text.from_location(x.edit.end) - p, @vi(0x7f
					times(p - text.from_location(x.edit.start), @ vi(0x8
					s = x.edit.text
				natavi.each_code(s, literal
			@ ::running = null
	startup = @(file, arguments, environments, match, done) Module("lsp").startup(loop, invalidate, file, arguments, environments, log
		@(token, title, cancellable, message, percentage)
			s = "" + token + " " + title + ":"
			message !== null && (s = s + " " + message)
			percentage !== null && (s = s + " " + percentage + "%")
			vi.progress(s
		@(client) done(client === null ? null : do(Object + @
			completion_triggers = [
			if client.capabilities.has("completionProvider")
				completion_privider =  client.capabilities["completionProvider"]
				completion_privider.has("triggerCharacters") && completion_privider["triggerCharacters"].each(completion_triggers.push
			$stop = client.shutdown
			$register = @(buffer) if match(buffer)
				text = buffer.text
				replaced = @(l0, c0, l1, c1, s) client.did_change(buffer.path, text.version, l0, c0, l1, c1, s
				buffer.lsps[$] = replaced
				text.replaced.push(replaced
				buffer.commands["lsp"] = verbify({
					"definition": @(i) goto("definition...", @(partial, done)
						l = text.to_location(buffer.view.position().text
						client.definition(buffer.path, l[0], l[1], done, partial
					"references": @(i) goto("references...", @(partial, done)
						l = text.to_location(buffer.view.position().text
						client.references(buffer.path, l[0], l[1], false, done, partial
					"hover": @(i)
						l = text.to_location(buffer.view.position().text
						cancellable("hover...", client.hover(buffer.path, l[0], l[1], @(value, error)
							mode__(vi
							error === null || return vi.message("ERROR: " + error
							value === null && return vi.message("no information"
							vi.message(""
							popup = Popup(
							popup.text.replace(0, -1, value
							mode__(@(c)
								mode__(vi
								popup__(null
								popup.dispose(
								vi(c
							popup__(popup.view
					"completion": @(i) loop.post(@
						vi(letter("i"
						completion(client, buffer, 1
				vi.map(buffer.maps.NORMAL, "\fc", ":lsp completion\r"
				vi.map(buffer.maps.NORMAL, "\fd", ":lsp definition\r"
				vi.map(buffer.maps.NORMAL, "\fh", ":lsp hover\r"
				vi.map(buffer.maps.NORMAL, "\fr", ":lsp references\r"
				vi.map_action(buffer.maps.INSERT, "\fc", "lsp completion", @ completion(client, buffer, 1
				completion_triggers.each(@(x)
					c = x.code_at(0
					vi.map_action(buffer.maps.INSERT, x, "lsp completion", @
						literal(c
						completion(client, buffer, 0
				client.did_open(buffer.path, text.version, text.slice(0, text.size(
			$unregister = @(buffer)
				remove(buffer.text.replaced, buffer.lsps.remove($
				buffer.commands.remove("lsp"
				buffer.maps.NORMAL.remove("\fc"
				buffer.maps.NORMAL.remove("\fd"
				buffer.maps.NORMAL.remove("\fh"
				buffer.maps.NORMAL.remove("\fr"
				buffer.maps.INSERT.remove("\fc"
				completion_triggers.each(buffer.maps.INSERT.remove
				client.did_close(buffer.path
	@(name, file, arguments, environments, match) vi.commands["clangd"] = verbify({
		"start": @(i) lsps.has(name) || startup(file, arguments, environments, match, @(lsp)
			lsp === null && return vi.message(name + ": failed."
			lsps[name] = lsp
			vi.buffers.each(lsp.register
			vi.message(name + ": started."
		"stop": @(i) if lsps.has(name)
			lsp = lsps.remove(name
			vi.buffers.each(@(x) x.lsps.has(lsp) && lsp.unregister(x
			lsp.stop(@ vi.message(name + ": stopped."
		"log": @(i) vi.switch_buffer(logs(

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
		$buffer = @(path, maps)
			text = Text(
			path !== null && read(text, path
			view = View(text, 0, 0, size[0], size[1] - 1
			syntax = nata_syntax.new(text, path, view
			buffer = Buffer(path, maps, text, view, syntax, lsps
			if syntax !== null
				tasks.push(step = @
					current = syntax.step(
					current === null && return
					vi.progress("running: " + current * 100 / text.size() + "%"
					invalidate(
				buffer.disposing.unshift(@
					remove(tasks, step
					syntax.dispose(
			buffer
		$timeout = @(timeout, action) loop.timer(@
			action(
			invalidate(
		, timeout, true).stop
		$overlay_iterator = nata_curses.OverlayIterator
		$KEY_BACKSPACE = nata_curses.KEY_BACKSPACE
		$KEY_ENTER = nata_curses.KEY_ENTER
	, status, strip, system.arguments.size() > 0 ? system.arguments[0] : null
	mode__ = @(x) :mode = x
	mode__(vi
	popup__ = @(x) :popup = x
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
				bh = main.size().y
				bh = (mh > bh ? mh : bh) - row
				if bh < ph + 1
					y = size[1] - sh - ph
					main.move(0, 0, size[0], y + bh - 1
					main.into_view(row, bh
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
	lsp = nata_lsp(loop, lsps, status, vi, mode__, popup__, invalidate
	lsp("clangd", "clangd", '("--log=verbose"), '(), @(buffer) buffer.syntax !== null && buffer.syntax.type == "cpp"
	loop.poll(0, true, false, @(readable, writable) if readable
		mode(vi.current().get(
		invalidate(
	loop.poll(resized, true, false, @(readable, writable) if readable
		nata_curses.read_resized(
		:size = nata_curses.size(
		invalidate(
	invalidate(
	loop.run(

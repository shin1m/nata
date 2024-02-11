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
	$hooks
	$__initialize = @(path, maps, text, view, syntax)
		natavi.Buffer.__initialize[$](path, maps, text, view, nata_curses.Overlay(view, nata_curses.A_REVERSE
		$disposing = [
		$syntax = syntax
		$hooks = {
	$dispose = @
		$hooks.each(@(k, v) v(
		$disposing.each(@(x) x(
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
	$__call = @(c) try
		map[c][$](
	catch Throwable t

nata_lsp = @(loop, hooks, status, vi, mode__, popup__, invalidate)
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
	choose = @(action, popup, result2items, item2text, opening, choose, finalize)
		end = @
			mode__(vi
			finalize(
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
				popup__(null
				chooser.dispose(
				end(
			items.each(append
			chooser.at__(0
			mode__(opening(chooser, @(done)
				cancel === null || cancel(
				items = [
				::push = @(result)
					result2items(result).each(items.push
					items.size() > 0 || return
					:::items = items
					chooser.text.replace(0, -1, ""
					items.each(append
					chooser.at__(0
					:::push = push_items
				::done = @(result, error)
					error === null || return vi.message("ERROR: " + error
					push(result
					done(items
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
			if error !== null
				vi.message("ERROR: " + error
				return end(
			push(result
			vi.message(""
			chooser === null || return
			items.size() > 0 ? choose(items[0]) : vi.message("no information"
			end(
		call = @
			discard = @
				::cancel = null
				:push = @(result)
				:done = @(result, error)
			push = @(result) ::push(result)
			done = @(result, error)
				discard(
				::done(result, error)
			cancel = action(@(result) push(result), @(result, error) done(result, error
			:cancel = @
				discard(
				cancel(
		call(
		@
			cancel(
			end(
	goto = @(message, action) cancellable(message, choose(action, 1
		@(x) x
		@(x) x.path + ": " + (x.line + 1) + "," + (x.character + 1)
		@(chooser, reload) chooser
		@(location)
			buffer = vi.open_buffer(location.path
			text = buffer.text
			buffer.view.position__(text.from_location(location), false
		@
	completion = @(client, buffer, search)
		search.pattern("\\b[_A-Za-z]\\w*\\b", nata.Search.ECMASCRIPT | nata.Search.OPTIMIZE
		target = @(p)
			line = buffer.text.line_at_in_text(p
			search.reset(line.from, line.count
			while true
				m = search.next(
				if m.size() <= 0 || m[0].from > p
					empty = nata.Span(
					empty.from = p
					empty.count = 0
					return empty
				m[0].from + m[0].count < p || return m[0]
		call = start = @(popup)
			cancel = choose(
				@(partial, done)
					l = buffer.text.to_location(buffer.view.position().text
					client.completion(buffer.path, l[0], l[1], done, partial
				popup
				@(x)
					x.incomplete !== null && (:incomplete = x.incomplete)
					x.items
				@(x) x.label
				@(chooser, reload)
					::call = @(popup)
					t = target(buffer.view.position().text
					@(c)
						c == control("I") && return chooser(control("M"
						c == control("[") && return chooser(c
						t = :t
						vi(c
						:t = target(buffer.view.position().text
						:t.from < t.from && return chooser(control("["
						#if incomplete
						#	::incomplete = null
						#	reload(
						reload(@(items) items.size() > 0 || ::t.count > 0 || chooser(control("["
				@(x)
					times = @(n, f) for ; n > 0; n = n - 1: f(
					p = buffer.view.position().text
					t = target(p
					vi.nomap(@
						times(t.from + t.count - p, @ vi(0x7f
						times(p - t.from, @ vi(0x8
						natavi.each_code(x.insert, vi
				@ ::call = start
			popup > 0 && return cancellable("completion...", cancel
			:call = @(popup)
				cancel(
				call(popup
		@(popup) call(popup
	startup = @(file, arguments, environments, done) Module("lsp").startup(loop, invalidate, file, arguments, environments, log
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
			$register = @(buffer)
				text = buffer.text
				replaced = @(l0, c0, l1, c1, s) client.did_change(buffer.path, text.version, l0, c0, l1, c1, s
				text.replaced.push(replaced
				search = nata.Search(text
				completion = :completion(client, buffer, search
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
						vi.nomap(@ vi(letter("i"
						completion(1
				for_n = @(f) f(buffer.maps.NORMAL
				vi.map(for_n, "\fc", ":lsp completion\r", true
				vi.map(for_n, "\fd", ":lsp definition\r", true
				vi.map(for_n, "\fh", ":lsp hover\r", true
				vi.map(for_n, "\fr", ":lsp references\r", true
				for_i = @(f) f(buffer.maps.INSERT
				vi.map_action(for_i, "\fc", "lsp completion", @ completion(1
				completion_triggers.each(@(x)
					c = x.code_at(0
					vi.map_action(for_i, x, "lsp completion", @
						vi.nomap(@ vi(c
						completion(0
				client.did_open(buffer.path, text.version, text.slice(0, text.size(
				@
					buffer.commands.remove("lsp"
					for_n(@(x)
						x.remove("\fc"
						x.remove("\fd"
						x.remove("\fh"
						x.remove("\fr"
					for_i(@(x)
						x.remove("\fc"
						completion_triggers.each(x.remove
					remove(text.replaced, replaced
					search.dispose(
					client.did_close(buffer.path
	instances = {
	@(name, file, arguments, environments, match) vi.commands[name] = verbify({
		"start": @(i) if !instances.has(name): instances[name] = startup(file, arguments, environments, @(hook)
			if hook === null
				instances.remove(name
				return vi.message(name + ": failed."
			register = @(x) if match(x): x.hooks[hook] = hook.register(x
			hooks.push(register
			instances[name] = @
				instances.remove(name
				vi.buffers.each(@(x) x.hooks.has(hook) && x.hooks.remove(hook)(
				remove(hooks, register
				hook.stop(@ vi.message(name + ": stopped."
			vi.buffers.each(register
			vi.message(name + ": started."
		"stop": @(i) instances.has(name) && instances[name](
		"log": @(i) vi.switch_buffer(logs(

suisha.main(@(loop) nata.main(@ nata_curses.main_with_resized(@(resized)
	nata_curses.define_pair(1, nata_curses.COLOR_WHITE, -1
	nata_curses.define_pair(2, nata_curses.COLOR_BLACK, nata_curses.COLOR_WHITE
	nata_curses.define_pair(3, -1, nata_curses.COLOR_YELLOW
	nata_syntax.initialize(4
	tasks = [
	hooks = [
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
			buffer = Buffer(path, maps, text, view, syntax
			if syntax !== null
				tasks.push(step = @
					current = syntax.step(
					current === null && return
					vi.progress("running: " + current * 100 / text.size() + "%"
					invalidate(
				buffer.disposing.unshift(@
					remove(tasks, step
					syntax.dispose(
			hooks.each(@(x) x(buffer
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
	lsp = nata_lsp(loop, hooks, status, vi, mode__, popup__, invalidate
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

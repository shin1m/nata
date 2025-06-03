nata = Module("nata"
utilities = Module("utilities"
remove = utilities.remove
do = utilities.do
letter = utilities.letter
control = utilities.control
each_code = utilities.each_code

$new = @(host)
	vi = host.vi
	cancellable = @(message, cancel)
		vi.message(message + " (ESC to cancel)"
		host.mode__(@(c) c == control("[") && cancel(
	verbify = @(verbs) @(i)
		match = vi.match_status("^\\s*(\\S+)", i
		match.size() > 0 || return
		group = match[1]
		verb = host.status.slice(group.from, group.count
		if verbs.has(verb)
			verbs[verb](group.from + group.count
		else
			vi.message("unknown verb: " + verb
	logs = do(@ @
		if !logs
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
			host.mode__(vi
			finalize(
		items = [
		append = @(x)
			s = item2text(x
			n = chooser.text.size(
			chooser.text.replace(n, 0, n > 0 ? "\n" + s : s
		push = @(result)
			result2items(result).each(items.push
			items.size() > popup || return
			:chooser = host.chooser(@(ok)
				if ok
					cancel && cancel(
					choose(items[chooser.at()]
				else
					cancel && return cancel(
				host.popup__(null
				chooser.dispose(
				end(
			items.each(append
			chooser.at__(0
			host.mode__(opening(chooser, @(done)
				cancel && cancel(
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
					error && return vi.message("ERROR: " + error
					push(result
					done(items
				call(
			host.popup__(chooser.view
			:push = push_items = @(result) result2items(result).each(@(x)
				items.push(x
				append(x
			:done = @(result, error)
				error && return vi.message("ERROR: " + error
				push(result
				vi.message(""
		done = @(result, error)
			if error
				vi.message("ERROR: " + error
				return end(
			push(result
			vi.message(""
			chooser && return
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
		@(x) !x ? [] : x.@ === List ? x : [x
		@(x)
			s = x["range"]["start"]
			x["uri"].substring(7) + ": " + (s["line"] + 1) + "," + (s["character"] + 1)
		@(chooser, reload) chooser
		@(x)
			buffer = vi.open_buffer(x["uri"].substring(7
			text = buffer.text
			buffer.view.position__(text.from_location(x["range"]["start"]), false
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
					!x && return [
					x.@ === List && return x
					:incomplete = x["isIncomplete"]
					x["items"]
				@(x) x["label"]
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
					times = @(n, f) for ; n > 0; n = n - 1; f(
					p = buffer.view.position().text
					t = target(p
					vi.nomap(@
						times(t.from + t.count - p, @ vi(0x7f
						times(p - t.from, @ vi(0x8
						each_code(x[x.has("insertText") ? "insertText" : "label"], vi
				@ ::call = start
			popup > 0 && return cancellable("completion...", cancel
			:call = @(popup)
				cancel(
				call(popup
		@(popup) call(popup
	startup = @(file, arguments, environments, done) Module("lsp").startup(host.loop, host.invalidate, file, arguments, environments, log
		@(token, title, cancellable, message, percentage)
			s = "" + token + " " + title + ":"
			message && (s = s + " " + message)
			percentage && (s = s + " " + percentage + "%")
			vi.progress(s
		@(client) done(client && do(Object + @
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
						cancellable("hover...", client.hover(buffer.path, l[0], l[1], @(result, error)
							host.mode__(vi
							error && return vi.message("ERROR: " + error
							result || return vi.message("no information"
							vi.message(""
							popup = host.popup(
							popup.text.replace(0, -1, result["contents"]["value"]
							host.mode__(@(c)
								host.mode__(vi
								host.popup__(null
								popup.dispose(
								vi(c
							host.popup__(popup.view
					"completion": @(i) host.loop.post(@
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
		"start": @(i) if !instances.has(name); instances[name] = startup(file, arguments, environments, @(hook)
			if !hook
				instances.remove(name
				return vi.message(name + ": failed."
			register = @(x) if match(x); x.hooks[hook] = hook.register(x
			host.hooks.push(register
			instances[name] = @
				instances.remove(name
				vi.buffers.each(@(x) x.hooks.has(hook) && x.hooks.remove(hook)(
				remove(host.hooks, register
				hook.stop(@ vi.message(name + ": stopped."
			vi.buffers.each(register
			vi.message(name + ": started."
		"stop": @(i) instances.has(name) && instances[name](
		"log": @(i) vi.switch_buffer(logs(

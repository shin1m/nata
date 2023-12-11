io = Module("io"
os = Module("os"
json = Module("json"

find_index = @(text, i, predicate)
	n = text.size(
	for ; i < n; i = i + 1: predicate(text.code_at(i)) && break
	i

jsonrpc = @(in, out, notified)
	writer = io.Writer(in, "utf-8"
	lastID = 0
	buffer = Bytes(1024
	head = tail = 0
	reader = io.Reader((Object + @
		$close = out.close
		$read = @(buffer, offset, size)
			if head >= tail
				::tail = out.read(::buffer, 0, ::buffer.size(
				::head = 0
			j = tail
			size < j - head && (j = head + size)
			for i = head; i < j; i = i + 1: if ::buffer[i] == 0xa
				i = i + 1
				break
			n = i - head
			::buffer.copy(head, n, buffer, offset
			::head = i
			n
	)(), "utf-8"
	read_limited = @(buffer, offset, size)
		n = out.read(buffer, offset, size
		:length = length - n
		n
	read_buffered = @(buffer, offset, size)
		n = tail - head
		if n <= 0
			:read = read_limited
			return read(buffer, offset, size
		n > size && (n = size)
		:buffer.copy(head, n, buffer, offset
		:head = head + n
		:length = length - n
		n
	send = @(message)
		buffer = [
		length = 0
		s = json.generate(message, 2, @(buffer, offset, size)
			bytes = Bytes(size
			buffer.copy(offset, size, bytes, 0
			:buffer.push(bytes
			:length = length + size
			size
		writer.write("Content-Length: " + length + "\r\n\r\n"
		writer.flush(
		buffer.each(@(x) in.write(x, 0, x.size(
	receivers = {
	(Object + @
		$dispatch = @ while true
			::length = 0
			while true
				line = reader.read_line(
				i = find_index(line, 0, @(x) x == 0x3a
				i < line.size() || break
				name = line.substring(0, i
				if name == "Content-Length"
					i = find_index(line, i + 1, @(x) x != 0x20
					::length = Integer(line.substring(i
			::read = read_buffered
			message = json.parse(@(buffer, offset, size) read(buffer, offset, size < length ? size : length
			if message.has("id")
				received = receivers.remove(message["id"]
				if message.has("error")
					received(null, message["error"]
				else
					received(message["result"], null
			else
				notified(message["method"], message["params"]
			head < tail || break
		$request = @(method, params, received)
			for id = lastID + 1; receivers.has(id); id = id + 1
			receivers[id] = received
			send({
				"jsonrpc": "2.0"
				"id": id
				"method": method
				"params": params
			::lastID = id
		$notify = @(method, params) send({
			"jsonrpc": "2.0"
			"method": method
			"params": params
	)(

$startup = @(loop, invalidate, file, arguments, environments, log, done)
	child = os.Child(file, arguments, environments, '(0, 1, 1
	rpc = jsonrpc(child.pipe(0), child.pipe(1), @(method, params)
		log("notified: " + method + ": " + json.stringify(params, 2) + "\n"
	child.pipe(2).blocking__(false
	error = io.Reader(child.pipe(2), "utf-8"
	read_error = @ while true
		x = error.read_line(
		(x === null || x == "") && break
		log(x
	loop.poll(child.pipe(2).fd(), true, false, @(readable, writable) if readable
		read_error(
		invalidate(
	loop.poll(child.pipe(1).fd(), true, false, @(readable, writable) if readable
		rpc.dispatch(
		invalidate(
	exit = @(done)
		loop.poll(child.pipe(0).fd(), false, false, @(readable, writable) if child.exited() !== null
			loop.unpoll(child.pipe(0).fd(
			loop.unpoll(child.pipe(1).fd(
			loop.unpoll(child.pipe(2).fd(
			read_error(
			child.close(
			done(
			invalidate(
		rpc.notify("exit", null
	rpc.request("initialize", {
		"processId": null
		"capabilities": {
			"general": {
				"positionEncodings": ["utf-8"
			"offsetEncoding": ["utf-8"
		#"workDoneToken": 0
	}, @(result, error)
		log("initialize: " + json.stringify(result, 2) + "\n"
		error === null || return exit(@ done(null
		rpc.notify("initialized", {
		done(Object + @
			$shutdown = @(done) rpc.request("shutdown", null, @(result, error)
				log("shutdown: " + json.stringify(result, 2) + "\n"
				exit(done
			$did_open = @(path, version, text) rpc.notify("textDocument/didOpen", {
				"textDocument": {
					"uri": "file:" + path
					"languageId": "cpp"
					"version": version
					"text": text
			$did_close = @(path) rpc.notify("textDocument/didClose", {
				"textDocument": {
					"uri": "file:" + path
			$did_change = @(path, version, line0, character0, line1, character1, text) rpc.notify("textDocument/didChange", {
				"textDocument": {
					"uri": "file:" + path
					"version": version
				"contentChanges": [{
					"range": {
						"start": {
							"line": line0
							"character": character0
						"end": {
							"line": line1
							"character": character1
					"text": text
			$definition = @(path, line, character, done) rpc.request("textDocument/definition", {
				"textDocument": {
					"uri": "file:" + path
				"position": {
					"line": line
					"character": character
				#"workDoneToken": token
			}, @(result, error)
				result !== null && error === null || return done(null, error
				xs = [
				result.each(@(x)
					start = x["range"]["start"]
					xs.push('(x["uri"].substring(7), start["line"], start["character"]
				done(xs, null
			$references = @(path, line, character, declaration, done) rpc.request("textDocument/references", {
				"textDocument": {
					"uri": "file:" + path
				"position": {
					"line": line
					"character": character
				"context": {
					"includeDeclaration": declaration
				#"workDoneToken": token
			}, @(result, error)
				result !== null && error === null || return done(null, error
				xs = [
				result.each(@(x)
					start = x["range"]["start"]
					xs.push('(x["uri"].substring(7), start["line"], start["character"]
				done(xs, null
			$hover = @(path, line, character, done) rpc.request("textDocument/hover", {
				"textDocument": {
					"uri": "file:" + path
				"position": {
					"line": line
					"character": character
				#"workDoneToken": token
			}, @(result, error) error === null ? done(result === null ? null : result["contents"]["value"], null) : done(null, error)

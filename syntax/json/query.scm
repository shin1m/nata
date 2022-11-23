(number) @number
(string) @string
(escape_sequence) @escape
[(null) (true) (false)] @literal
(pair key: (_) @key)
["{" "}" "[" "]"] @bracket
(comment) @comment
(object) @object
(array) @array
[(object) (array)] @crease

; Functions

(call_expression
  function: (qualified_identifier
    name: (identifier) @function))

(template_function
  name: (identifier) @function)

(template_method
  name: (field_identifier) @function)

(template_function
  name: (identifier) @function)

(function_declarator
  declarator: (qualified_identifier
    name: (identifier) @function))

(function_declarator
  declarator: (qualified_identifier
    name: (identifier) @function))

(function_declarator
  declarator: (field_identifier) @function)

; Types

((namespace_identifier) @type
 (#match? @type "^[A-Z]"))

[
 (auto)
 (primitive_type)
] @type

; Constants

(this) @variable.builtin
[
 (nullptr)
 (false)
 (true)
] @constant

; Keywords

[
 "break"
 "case"
 "catch"
 "class"
 "co_await"
 "co_return"
 "co_yield"
 "constexpr"
 "constinit"
 "consteval"
 "continue"
 "default"
 "delete"
 "do"
 "else"
 "explicit"
 "final"
 "for"
 "friend"
 "if"
 "mutable"
 "namespace"
 "noexcept"
 "new"
 "operator"
 "override"
 "private"
 "protected"
 "public"
 "return"
 "struct"
 "switch"
 "template"
 "throw"
 "try"
 "typename"
 "using"
 "virtual"
 "while"
 "concept"
 "requires"
] @keyword

; Strings

[
 (number_literal)
 (char_literal)
 (string_literal)
 (raw_string_literal)
 (user_defined_literal)
] @string

(comment) @comment
(preproc_if ["#if" condition: (_) "#endif"] @preprocessor)
(preproc_ifdef ["#ifdef" "#ifndef" name: (_) "#endif"] @preprocessor)
(preproc_else "#else" @preprocessor)
(preproc_elif ["#elif" condition: (_)] @preprocessor)
(preproc_include) @preprocessor
(preproc_def) @preprocessor
(preproc_function_def) @preprocessor
(preproc_directive) @preprocessor

(function_definition body: (_) @crease)
(class_specifier body: (_) @crease)
(enum_specifier body: (_) @crease)
(struct_specifier body: (_) @crease)
(union_specifier body: (_) @crease)
(namespace_definition body: (_) @crease)

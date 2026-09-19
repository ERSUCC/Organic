from pygments.lexer import bygroups, include, RegexLexer, words
from pygments.style import Style
from pygments.token import Comment, Literal, Name, Operator, Punctuation, String, Whitespace

class OrganicLexer(RegexLexer):
    name = "Organic"
    aliases = [ "organic" ]
    filenames = [ ".organic" ]
    mimetypes = "text/organic"

    identifier = r"[a-zA-Z_][a-zA-Z0-9_\-]*"

    tokens = {
        "root": [
            (r"//.*?\n", Comment),
            (r"/\*", Comment, "comment"),
            (f"({identifier})" + r"(\s*)(\()", bygroups(Name.Function, Whitespace, Punctuation)),
            include("value"),
            (r"[\^*/+\-<>]|<=|>=|==", Operator),
            (r"[=:,\(\)\[\]\{\}]", Punctuation),
            (r"\s+", Whitespace)
        ],
        "comment": [
            (r"\*/", Comment, "#pop"),
            (r"[\s\S]+?", Comment)
        ],
        "value": [
            (r"\"", String, "string"),
            (r"\|" + identifier + r"\|", Name.Variable),
            (words((
                "pi", "tau", "e",
                "true", "false",
                "forward", "backward", "shuffle",
                "stay", "linear",
                "nearest", "up", "down"
            ), r"\b", r"\b"), Literal),
            (r"[a-g][sf]?\d", Literal),
            (identifier, Name.Variable),
            (r"-?(\d+\.)?\d+", Literal)
        ],
        "string": [
            (r"\\\"", String),
            (r"\"", String, "#pop"),
            (r".+?", String)
        ]
    }

class OrganicStyle(Style):
    styles = {
        Comment: "#808080",
        String: "#2ebfa5",
        Literal: "#e08f3e",
        Name.Function: "#279af1",
        Name.Variable: "#e06c9f",
        Operator: "#c46dec"
    }

#include <string>
#include <vector>

using namespace std;
extern const vector<string> reservedWords = {
        "alignas",
        "alignof",
        "and",
        "asm",
        "atomic_cancel",
        "atomic_commit",
        "atomic_noexcept",
        "auto",
        "bitand",
        "bitor",
        "bool",
        "break",
        "case",
        "catch",
        "char",
        "class",
        "const",
        "continue",
        "default",
        "delete",
        "do",
        "double",
        "else",
        "export",
        "extern",
        "false",
        "float",
        "for",
        "friend",
        "goto",
        "if",
        "inline",
        "int",
        "long",
        "namespace",
        "new",
        "noexcept",
        "not",
        "nullptr",
        "operator",
        "or",
        "private",
        "protected",
        "public",
        "reflexpr",
        "requires",
        "return",
        "short",
        "signed",
        "sizeof",
        "static",
        "static_cast",
        "struct",
        "switch",
        "template",
        "this",
        "throw",
        "true",
        "try",
        "typedef",
        "typeid",
        "typename",
        "union",
        "unsigned",
        "using",
        "virtual",
        "void",
        "while",
};
extern const vector<string> punctuation = { ",", ";", "{", "}", "<" , ">" };


extern const vector<string> operators = {
    "+", "-", "--", "++", "*", "/", "%", "**", "<<", ">>", "<", "<=", ">=", ">", "!=",
    "==", "=", "+=", "-=", "*=", "/=", "%=", "|=", "^=", "&=", "&", "|", "^", "&&", "."
    "||", "!", "~",
    "[", "]", "(", ")", "?", " : ", "::"
};

extern const vector<string> preprocessorDirectives = {
        "#include",
        "#define",
        "#undef",
        "#if",
        "#ifdef",
        "#ifndef",
        "#else",
        "#elif",
        "#endif",
        "#error",
        "#pragma"
};

extern const vector<char> spaces = { ' ', '\n', '\t', '\v', '\r', '\b' };

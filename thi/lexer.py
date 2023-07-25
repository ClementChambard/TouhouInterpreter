from enum import Enum
from typing import Tuple
from macros import parseMacro, processBlocFinal


class TOKEN(Enum):
    ERROR = -2
    EOF = -1
    IDENT = 0
    KEYWORD = 1
    SYMBOL = 2
    INTEGER = 3
    CPPCODE = 4


SYMBOLES_1 = ["&", ",", "@", "(", ")"]
KEYWORDS = ["int", "float", "string"]


class Token:
    def __init__(self, val: str, typ: TOKEN, line: int, col: int):
        self.val = val
        self.typ = typ
        self.line = line
        self.col = col

    def __str__(self) -> str:
        return (
            self.typ.name + " " + self.val + " " + str(self.line) + ":" + str(self.col)
        )

    def __repr__(self) -> str:
        return f"Token({self.val}, TOKEN.{self.typ.name}, {self.line}, {self.col})"

    def parseStr(self) -> str:
        out = "Term__" + self.typ.name
        if self.typ == TOKEN.SYMBOL or self.typ == TOKEN.KEYWORD:
            out += "_" + self.val
        return out


def errorLex(fileContent: str, read: str) -> Tuple[str, TOKEN]:
    tok = read
    while fileContent != "" and fileContent[0].isalnum():
        tok += fileContent[0]
        fileContent = fileContent[1:]
    return tok, TOKEN.ERROR


def identifierLex(fileContent: str) -> str:
    ident = fileContent[0]
    fileContent = fileContent[1:]
    while fileContent != "" and (fileContent[0].isalnum() or fileContent[0] == "_"):
        ident += fileContent[0]
        fileContent = fileContent[1:]
    return ident


def tryLexCpp(fileContent: str):
    astin = ""
    nbbrace = 1
    commentSize = 0
    while True:
        if len(fileContent) < 1:
            return None
        if fileContent.startswith("//"):
            while len(fileContent) > 0 and fileContent[0] != '\n':
                fileContent = fileContent[1:]
                commentSize += 1
            if len(fileContent) == 0:
                return None
            fileContent = fileContent[1:]
            commentSize += 1
            continue
        if fileContent.startswith("/*"):
            while len(fileContent) > 0 and fileContent.startswith("*/"):
                fileContent = fileContent[1:]
                commentSize += 1
            if len(fileContent) == 0:
                return None
            commentSize += 2
            fileContent = fileContent[2:]
            continue
        if fileContent[0] == "{":
            nbbrace += 1
        if fileContent[0] == "}":
            nbbrace -= 1
            if nbbrace == 0:
                le = len(astin) + commentSize
                return processBlocFinal(astin), le + 2
        astin += fileContent[0]
        fileContent = fileContent[1:]


def lex(fileContent: str, line: int, col: int) -> Tuple[Token, str]:
    # EOF
    if fileContent == "":
        return Token("", TOKEN.EOF, line, col), fileContent

    # macros at lex time
    if fileContent[0] == "!":
        a = parseMacro(fileContent)
        if a is not None:
            _, fileContent, lin, c = a
            return lex(fileContent, lin, c)

    # comments
    if fileContent[0:2] == "//":
        while len(fileContent) > 0 and fileContent[0] != "\n":
            fileContent = fileContent[1:]
        return lex(fileContent[1:], line + 1, 0)
    if fileContent[0:2] == "/*":
        while len(fileContent) > 1 and fileContent[0:2] != "*/":
            if fileContent[0] == "\n":
                line += 1
                col = 0
            else:
                col += 1
            fileContent = fileContent[1:]
        return lex(fileContent[2:], line, col + 2)

    # white spaces
    if fileContent[0] == " ":
        return lex(fileContent[1:], line, col + 1)
    if fileContent[0] == "\n":
        return lex(fileContent[1:], line + 1, 0)

    # CPP
    if fileContent[0] == "{":
        cpp = tryLexCpp(fileContent[1:])
        if cpp is not None:
            cpp, lencpp = cpp
            return Token(cpp, TOKEN.CPPCODE, line, col), fileContent[lencpp:]

    # integers
    if fileContent[0].isdecimal():
        s = ""
        i = 0
        while fileContent[i].isdecimal():
            s += fileContent[i]
            i += 1
        return Token(s, TOKEN.INTEGER, line, col), fileContent[i:]

    # id
    if fileContent[0].isalpha() or fileContent[0] == "_":
        ident = identifierLex(fileContent)
        # KEYWORD
        if ident in KEYWORDS:
            return Token(ident, TOKEN.KEYWORD, line, col), fileContent[len(ident) :]
        return Token(ident, TOKEN.IDENT, line, col), fileContent[len(ident) :]

    # symboles
    if fileContent[0] in SYMBOLES_1:
        return Token(fileContent[0], TOKEN.SYMBOL, line, col), fileContent[1:]

    # error
    return Token(fileContent[0], TOKEN.ERROR, line, col), fileContent[1:]


def lexAll(fileContent: str):
    col = 0
    line = 0
    tokens = []
    while True:
        tok, fileContent = lex(fileContent, line, col)
        col = tok.col + len(tok.val)
        line = tok.line
        tokens.append(tok)
        if tok.typ == TOKEN.EOF:
            break
    return tokens


def main():
    txt = ""
    with open("grammarDef.txt") as f:
        txt = f.read()
    tokks = lexAll(txt)
    for t in tokks:
        print(t)


if __name__ == "__main__":
    main()

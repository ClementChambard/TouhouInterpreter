def cheq(c):
    def inner(d):
        return d == c

    return inner


def chne(c):
    def inner(d):
        return d != c

    return inner


def notfn(f):
    def inner(d):
        return not f(d)

    return inner


def chany(_):
    return True


def nextCharIs(s, p):
    if len(s) < 1:
        return False
    if type(p) == str:
        return s[0] == p
    return p(s[0])


def identChar(start):
    def identCharSt(c):
        return c == "_" or c.isalpha()

    def identChar(c):
        return c == "_" or c.isalnum()

    return [identChar, identCharSt][start]


def parseWhile(s, p, p0=None):
    parsed = ""
    if p0 is not None:
        if not nextCharIs(s, p0):
            return "", s
        parsed += s[0]
        s = s[1:]
    while nextCharIs(s, p):
        parsed += s[0]
        s = s[1:]
    return parsed, s


def parseIdent(s):
    return parseWhile(s, identChar(False), identChar(True))

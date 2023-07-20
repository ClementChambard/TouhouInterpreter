from typing import List, Tuple


macro1 = """
!e = stripped {
    enemy.base
}
"""

macro2 = """
!tt(a) = stripped {
    std::cout << a << e.str << "\\n";
}
"""

macro3 = """
!m3(a b c) = stripped {
    tt(a)
    u = e.b + c + a
}
"""


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


def chany(c):
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


def parseArgList(s):
    sini = s
    if not nextCharIs(s, "("):
        return [], s
    s = s[1:].strip()
    args = []
    while True:
        if nextCharIs(s, ")"):
            return args, s[1:]
        i, s = parseIdent(s)
        if i == "":
            return [], sini
        args.append(i)
        s = s.strip()


def applyMacro(m, callArgs) -> Tuple[str, bool]:
    ret = ""
    for b in m["body"]:
        if b["t"] == "text":
            ret += b["val"]
        elif b["t"] == "arg":
            i = b["val"]
            if i >= len(callArgs):
                return m["name"], False
            ret += callArgs[i]
    return ret, True


def getCallArgs(txt):
    if len(txt) == 0 or txt[0] != "(":
        return [], 0
    txt = txt[1:]
    curArg = ""
    args = []
    parens = 1
    i = 1
    while len(txt) > 0:
        if txt[0] == "(":
            parens += 1
        elif txt[0] == ")":
            parens -= 1
            if parens == 0:
                args.append(curArg)
                return args, i + 1
        elif txt[0] == "#":
            args.append(curArg)
            curArg = ""
            txt = txt[1:]
            i += 1
            continue
        curArg += txt[0]
        txt = txt[1:]
        i += 1
    return [], 0


def processBloc(bloc, args: List[str], macros: List):
    # pass 1: separate idents from other chars to identify macro calls & args. Also no @
    macronames: List[str] = [m["name"] for m in macros]
    splittedBloc: List[str] = []
    while len(bloc) > 0:
        id, bloc = parseIdent(bloc)
        if id != "":
            splittedBloc.append(id)
        else:
            if bloc[0] != "@":
                splittedBloc.append(bloc[0])
            bloc = bloc[1:]
    # pass 2: call macros and paste args
    curstr: str = ""
    splittedBloc2: List = []
    i = 0
    while True:
        b = splittedBloc[i]
        if b in args:
            splittedBloc2.append({"t": "text", "val": curstr})
            curstr = ""
            splittedBloc2.append({"t": "arg", "val": args.index(b)})
        elif b in macronames:
            splittedBloc2.append({"t": "text", "val": curstr})
            curstr = ""
            mi = macronames.index(b)
            callargs, ad = getCallArgs(splittedBloc[i + 1 :])
            i += ad
            applied, ok = applyMacro(macros[mi], callargs)
            if not ok:
                i -= ad
            if len(callargs) > 0:
                applied = processBloc(applied, args, macros)
                splittedBloc2 += applied
            else:
                splittedBloc2.append({"t": "text", "val": applied})
        else:
            curstr += b
        i += 1
        if i == len(splittedBloc):
            break
    if curstr != "":
        splittedBloc2.append({"t": "text", "val": curstr})
    # pass 3: join adjacent text blocks
    splittedBloc3 = []
    txt = ""
    while len(splittedBloc2) > 0:
        bl = splittedBloc2[0]
        splittedBloc2 = splittedBloc2[1:]
        if bl["t"] == "text":
            txt += bl["val"]
        elif bl["t"] == "arg":
            splittedBloc3.append({"t": "text", "val": txt})
            txt = ""
            splittedBloc3.append(bl)
    if txt != "":
        splittedBloc3.append({"t": "text", "val": txt})
    return splittedBloc3


macroList: List = []


def parseMacro(mStr):
    mStr = mStr.strip()
    if not nextCharIs(mStr, "!"):
        return None
    mStr = mStr[1:]
    name, mStr = parseIdent(mStr)
    mStr = mStr.strip()
    args, mStr = parseArgList(mStr)
    mStr = mStr.strip()
    if not nextCharIs(mStr, "="):
        return None
    mStr = mStr[1:].strip()
    kw, mStr = parseIdent(mStr)
    stripped = kw == "stripped"
    mStr = mStr.strip()
    if not nextCharIs(mStr, "{"):
        return None
    body = ""
    mStr = mStr[1:]
    parens = 1
    while True:
        if nextCharIs(mStr, "{"):
            parens += 1
        elif nextCharIs(mStr, "}"):
            parens -= 1
        elif not nextCharIs(mStr, chany):
            return None
        if parens == 0:
            break
        body += mStr[0]
        mStr = mStr[1:]
    if stripped:
        body = body.strip()
    body = processBloc(body, args, macroList)
    macroList.append({"name": name, "args": args, "body": body})
    return macroList[len(macroList) - 1], mStr[1:], 0, 0


def processBlocFinal(b):
    d = processBloc(b, [], macroList)
    return d[0]["val"]


if __name__ == "__main__":
    m1 = parseMacro(macro1)
    m2 = parseMacro(macro2)
    m3 = parseMacro(macro3)

    body = """
        e.die();
        tt("dead")
        m3(1#(2,3)#e)
    """

    print(processBlocFinal(body))

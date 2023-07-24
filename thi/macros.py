from typing import List, Tuple
import codeUtil
import parseUtil

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

def parseArgList(s):
    sini = s
    if not parseUtil.nextCharIs(s, "("):
        return [], s
    s = s[1:].strip()
    args = []
    while True:
        if parseUtil.nextCharIs(s, ")"):
            return args, s[1:]
        i, s = parseUtil.parseIdent(s)
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
    if bloc == "":
        return []
    # pass 1: separate idents from other chars to identify macro calls & args. Also no @
    macronames: List[str] = [m["name"] for m in macros]
    splittedBloc = codeUtil.splitForMacro(bloc)
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
    if not parseUtil.nextCharIs(mStr, "!"):
        return None
    mStr = mStr[1:]
    name, mStr = parseUtil.parseIdent(mStr)
    mStr = mStr.strip()
    args, mStr = parseArgList(mStr)
    mStr = mStr.strip()
    if not parseUtil.nextCharIs(mStr, "="):
        return None
    mStr = mStr[1:].strip()
    kw, mStr = parseUtil.parseIdent(mStr)
    stripped = kw == "stripped"
    mStr = mStr.strip()
    if not parseUtil.nextCharIs(mStr, "{"):
        return None
    body = ""
    mStr = mStr[1:]
    parens = 1
    while True:
        if parseUtil.nextCharIs(mStr, "{"):
            parens += 1
        elif parseUtil.nextCharIs(mStr, "}"):
            parens -= 1
        elif not parseUtil.nextCharIs(mStr, parseUtil.chany):
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
    if b == "":
        return ""
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

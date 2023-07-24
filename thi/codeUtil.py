from typing import List
import parseUtil

PRINT = False


def splitForMacro(bloc: str) -> List[str]:
    splittedBloc: List[str] = []
    while len(bloc) > 0:
        id, bloc = parseUtil.parseIdent(bloc)
        if id != "":
            splittedBloc.append(id)
        else:
            if bloc[0] != "@":
                splittedBloc.append(bloc[0])
            bloc = bloc[1:]
    return splittedBloc


def formatBloc(bloc: str, indent: int, startlevel: int) -> str:
    indentlevel = startlevel
    lines = [line.strip() for line in bloc.split("\n")]
    newlines = []
    nextIsIndented = False
    inquote = ""
    for line in lines:
        if line == "":
            continue
        modifier = 0
        if line.startswith("}") or line.startswith(")") or line.startswith("]"):
            modifier = -1
        if nextIsIndented:
            modifier += 1
            nextIsIndented = False
        if (
            line.startswith("if")
            or line.startswith("while")
            or line.startswith("else")
            or line.startswith("for")
        ) and line.endswith(")"):
            nextIsIndented = True
        newlines.append((indentlevel + modifier) * indent * " " + line)
        for c in line:
            if c in "([{" and inquote == "":
                indentlevel += 1
            elif c in "}])" and inquote == "":
                indentlevel -= 1
            elif c == inquote:
                inquote = ""
            elif c in "\"'":
                inquote = c

    return "\n".join(newlines) + "\n"


# define I_(n) enemy.ecl_int_vars[n]
# define f_(n) enemy.ecl_float_vars[n]
#
# elif b == "BOSS":
#           blocs[
#               i
#           ] = "EnemyManager::GetInstance()->EnmFind"+
# "(EnemyManager::GetInstance()->boss_ids[0])"
#       elif b == "ISBOSS":
#           blocs[
#               i
#           ] = "(EnemyManager::GetInstance()->boss_ids[0] > 0 && EnemyManager::"+
# "GetInstance()->EnmFind(EnemyManager::GetInstance()->boss_ids[0]))"


def checkInsForBuiltinVars(ins):
    blocs = splitForMacro(ins.code)

    skip = 0
    for i, b in enumerate(blocs):
        if skip > 0:
            blocs[i] = ""
            skip -= 1
            continue
        if b == "INS_NAME":
            blocs[i] = '"' + ins.name + '"'
        elif b == "INS_ID":
            blocs[i] = ins.id
        elif b == "WARN":
            if blocs[i + 1] != "(":
                continue
            start = i + 2
            end = start
            for ii in range(start, len(blocs)):
                if blocs[ii] == ")":
                    end = ii
                    break
            else:
                continue
            skip = end - start + 2
            data = blocs[start:end]
            string = "".join(data)
            blocs[i] = (
                f'std::cout << "\\e[32m/!\\\\ warning at {ins.name}'
                + f' (ins_{ins.id}) : \\e[0m {string}\\n";'
            )
        elif b == "return":
            blocs[i] = "        return 0"
            if PRINT and not ins.noprint:
                blocs[i] = 'std::cout << ")\\n"\n' + blocs[i]

    ins.code = "".join(blocs)
    if ins.code.strip().endswith("return 0;"):
        ins.nobreak = True

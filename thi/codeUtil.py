from typing import List
import parseUtil

PRINT = False


def after_args():
    return (
        "        cont->stack.stackOffset -= stackToRemove;\n"
        + "        stackToRemove = 0;"
    )


def get_arg_vararg(t, name):
    iorf = ["I", "F"][t == "float"]
    __arg = "__arg"
    end = "        __arg+=4;\n"
    end += "        __argnum++;"
    return (
        f"        {t} {name} = *reinterpret_cast<{t}*>({__arg});\n"
        + "        if (1<<__argnum) "
        + f"{name} = checkVar{iorf}({name});\n{end}"
    )


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


def long_one_arg_macro(b, start):
    nparens = 1
    inquote = ""
    for i in range(start, len(b)):
        if b[i] == ")" and inquote == "":
            nparens -= 1
            if nparens == 0:
                return i
        elif b[i] == "(" and inquote == "":
            nparens += 1
        elif b[i] == inquote:
            inquote = ""
        elif b[i] in "'\"":
            inquote = b[i]
    return -1


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
            end = long_one_arg_macro(blocs, start)
            if end < 0:
                continue
            skip = end - start + 2
            data = blocs[start:end]
            string = "".join(data)
            blocs[i] = (
                f'std::cout << "\\e[32m/!\\\\ warning at {ins.name}'
                + f' (ins_{ins.id}) : \\e[0m {string}\\n";'
            )
        elif b == "VA_FLOAT":
            blocs[i] = "*reinterpret_cast<const char*>(__arg) == 'f'"
        elif b == "VA_INT":
            blocs[i] = "*reinterpret_cast<const char*>(__arg) == 'f'"
        elif b == "VA_GET_S":
            if blocs[i + 1] != "(":
                continue
            sskip = 1
            leng = 0
            for ii in range(i + 2, len(blocs)):
                if (
                    not blocs[ii].isspace()
                    and blocs[ii][0] != "_"
                    and not blocs[ii][0].isalpha()
                ):
                    break
                leng += 1
            sskip += leng
            name = blocs[i + 2 : i + 2 + leng]
            name = "".join(name).strip()
            if name == "" or blocs[i + 2 + leng] != ")":
                continue
            skip = sskip + 1
            string = "__arg += 4;\n"
            string += get_arg_vararg("int32_t", name)
            blocs[i] = string
        elif b == "VA_GET_F":
            if blocs[i + 1] != "(":
                continue
            sskip = 1
            leng = 0
            for ii in range(i + 2, len(blocs)):
                if (
                    not blocs[ii].isspace()
                    and blocs[ii][0] != "_"
                    and not blocs[ii][0].isalpha()
                ):
                    break
                leng += 1
            sskip += leng
            name = blocs[i + 2 : i + 2 + leng]
            name = "".join(name).strip()
            if name == "" or blocs[i + 2 + leng] != ")":
                continue
            skip = sskip + 1
            string = "__arg += 4;\n"
            string += get_arg_vararg("float", name)
            blocs[i] = string
        elif b == "VA_END":
            blocs[i] = after_args()
        elif b == "VA_COUNT":
            blocs[i] = "__va_count"
        elif b == "ON_PRINT":
            if blocs[i + 1] != "(":
                continue
            start = i + 2
            end = long_one_arg_macro(blocs, start)
            if end < 0:
                continue
            skip = end - start + 2
            data = blocs[start:end]
            string = "".join(data)
            if PRINT:
                blocs[i] = string
            else:
                blocs[i] = ""

        elif b == "return":
            blocs[i] = "        break"
        elif b == "delete_self":
            blocs[i] = "        return -1"
        elif b == "exit":
            blocs[i] = "        return 0"
            if PRINT and not ins.noprint:
                blocs[i] = 'std::cout << ")\\n"\n' + blocs[i]

    ins.code = "".join(blocs)
    if ins.code.strip().endswith("return 0;"):
        ins.nobreak = True

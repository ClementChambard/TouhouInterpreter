import codeUtil

PRINT: bool = False
g_instr = None


def print_header(name):
    return f"""std::cout << "\\e[90m"
            << fileManager->getSubName(cont->currentLocation.sub_id)
            << ":  \\e[0m";
        if (instr->param_count == 0)                                      
            std::cout << {name} << "(";\n"""


def print_footer():
    return '        std::cout << ")\\n";\n'

def exec_end():
    return """cont->stack.stackOffset -= stackToRemove;
    stackToRemove = 0;"""


def goto_next_instr():
    return "cont->currentLocation.offset += instr->size;"


def init():
    return ""


def exec_condition():
    return "(instr->rank_mask & 63 & cont->difficultyMask)"


def get_ins_id():
    return "instr->id"

def is_arg(i):
    return f"instr->param_mask&{1<<i}"

def after_args():
    return (
        "        cont->stack.stackOffset -= stackToRemove;\n"
        + "        stackToRemove = 0;"
    )


def get_arg(i, t, name, vs):
    iorf = ["I", "F"][t == "float"]
    __arg = "__arg"
    end = "        __arg+=4;\n"
    if not vs:
        end = ""
        __arg = f"&instr->data[{i*4}]"
    if PRINT:
        return (
            f"        {t} {name} = *reinterpret_cast<{t}*>({__arg});\n{end}"
            + f"        {t} __{name}_var=0;\n"
            + f"        if ({is_arg(i)}) {'{'}\n"
            + f"            __{name}_var = {name};\n"
            + f"            {name} = checkVar{iorf}({name});\n"
            + "        }"
        )
    else:
        return (
            f"        {t} {name} = *reinterpret_cast<{t}*>({__arg});\n{end}"
            + f"        if ({is_arg(i)}) "
            + f"{name} = checkVar{iorf}({name});"
        )


def get_arg_ref(i, t, name, vs):
    __arg = "__arg"
    end = "\n        __arg+=4;"
    if not vs:
        end = ""
        __arg = f"&instr->data[{i*4}]"
    if PRINT:
        return (
            f"        const {t} __{name}_var = *reinterpret_cast<const {t}*>({__arg});"
            + f"{end}\n        {t}& {name}=checkVarRI(__{name}_var);"
        )
    else:
        return (
            f"        {t}& {name}=checkVarRI(*reinterpret_cast<const {t}*>({__arg}));"
            + end
        )


def print_arg(i, name):
    string = ""
    if i > 0:
        string += ", "
    else:
        string += f"{name}("
    string += f"std::cout<<{string};if (__{name}_isvar)"
    string += f'std::cout<<"["<<__{name}_var<<"] : ";std::cout<<{name}'
    return string


def arg_S(instr, i, name):
    string = get_arg(i, "int32_t", name, instr.has_variable_size()) + "\n"
    if PRINT and not instr.noprint:
        string += print_arg(i, name)
    return string


def arg_f(instr, i, name):
    string = get_arg(i, "float", name, instr.has_variable_size()) + "\n"
    if PRINT and not instr.noprint:
        string += print_arg(i, name)
    return string


def arg_rS(instr, i, name):
    string = get_arg_ref(i, "int32_t", name, instr.has_variable_size()) + "\n"
    if PRINT and not instr.noprint:
        string += print_arg(i, name)
    return string


def arg_rf(instr, i, name):
    string = get_arg_ref(i, "float", name, instr.has_variable_size()) + "\n"
    if PRINT and not instr.noprint:
        string += print_arg(i, name)
    return string


def arg_z(instr, i, name):
    string = f"int32_t __{name}_size= *reinterpret_cast<int32_t*>(__arg);"
    string += f'__arg += 4;std::string {name}="";'
    string += f"for(int i=0;i<__{name}_size;i++)if(__arg[i]!=0)x+=__arg[i];"
    string += f"__arg+=__{name}_size;\n"
    if PRINT and not instr.noprint:
        string += print_arg(i, name)
    return string


def ins_header(instr) -> str:
    global g_instr
    g_instr = instr
    string = ""
    if instr.has_variable_size():
        string += (
            "        unsigned char* __arg = const_cast<unsigned char*>(instr->data);\n"
        )
    if PRINT and not instr.noprint:
        string += print_header(instr.name)
    for i, a in enumerate(instr.args):
        if a[0] == "_S":
            string += arg_S(instr, i, a[1])
        if a[0] == "_f":
            string += arg_f(instr, i, a[1])
        if a[0] == "_rS":
            string += arg_rS(instr, i, a[1])
        if a[0] == "_rf":
            string += arg_rf(instr, i, a[1])
        if a[0] == "_z":
            string += arg_z(instr, i, a[1])
    if len(instr.args) > 0:
        string += after_args()
    return string


def ins_footer() -> str:
    string = ""
    if g_instr is not None and not g_instr.nobreak:
        string += "        break;\n"
    elif PRINT and g_instr is not None and not g_instr.noprint:
        string += print_footer() + "\n"
    return string


def switchStart(instr_0) -> str:
    string = f"{init()}\n"
    string += f"""if ({exec_condition()}) {'{'}
    switch ({get_ins_id()}) {'{'}
    case {instr_0.id}: {'{'}
{ins_header(instr_0)}"""
    return string


def nextInstr(instr) -> str:
    string = ins_footer() + "    }\n"
    string += f"""    case {instr.id}: {'{'}
{ins_header(instr)}
"""
    if instr.notimpl:
        return string + f"        std::cout << \"\\e[33m/!\\\\ {instr.name}" \
        + f" (ins_{instr.id}) is not implemented\\e[0m\\n\";\n"
    if instr.alert:
        string += f"        std::cout << \"e[34/!\\\\ alert ---> {instr.name}" \
        + f" (ins_{instr.id}) has been used\\e[0m\\n\";\n"
    string += codeUtil.formatBloc(instr.code, 4, 2)
    return string

def switchEnd() -> str:
    string = ins_footer() + "    }\n"
    string += f"""    default: {'{'}
        std::cout << \"\\e[92m ERROR: undefined instruction opcode \"
            << {get_ins_id()} << \" \\e[0m\\n\";
    {'}'}
    {'}'}
    {exec_end()}
{'}'}
{goto_next_instr()}"""
    return string



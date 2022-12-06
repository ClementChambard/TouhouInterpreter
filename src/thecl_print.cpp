#include "thecl_print.h"
#include "thecl_format.h"
#include "thecl_value.h"

#include <string>

std::string ins_get_name(int i)
{
    if (i == 0) return "nop";
    if (i == 1) return "delete";
    if (i == 10) return "return";
    if (i == 12) return "jump";
    if (i == 13) return "jumpEq";
    if (i == 14) return "jumpNeq";
    if (i == 22) return "debug22";
    if (i == 23) return "wait";
    if (i == 42 || i == 44) return "push";
    return "ins_" + std::to_string(i);
}

std::string int_print_n(int i, int n, char c = ' ')
{
    auto s = std::to_string(i);
    while (s.length() < n) s = c + s;
    return s;
}

std::string rank_str(int r)
{
    std::string s = "";
    if (r & 1) s += "E"; else s += "-";
    if (r & 2) s += "N"; else s += "-";
    if (r & 4) s += "H"; else s += "-";
    if (r & 8) s += "L"; else s += "-";
    return s;
}

void print_ecl(EclRaw_t* ecl)
{
    for (auto anim : ecl->anim_list) std::cout << "#anim <" << anim << ">\n";
    std::cout << "\n";
    for (auto ecli : ecl->ecli_list) std::cout << "#include<" << ecli << ">\n";
    for (auto sp : ecl->subs) {
        std::cout << "\nsub " << sp.name << "()\n{\n";
        for (auto instr : sp.instrs)
        {
            std::cout << int_print_n(instr->time, 3) << "-" << int_print_n(instr->size, 3, '-') << "-" << rank_str(instr->rank_mask) << ": ";
            std::cout << ins_get_name(instr->id) << "(";
            if (instr->param_count > 0)
            {
                size_t param_size_total = instr->size - sizeof(EclRawInstr_t);
                std::string format = thecl_find_format(18, instr->id, false);
                thecl_value_t* values = thecl_value_list_from_data(instr->data, instr->size - sizeof(EclRawInstr_t), format.c_str());
                for (int i = 0; i < instr->param_count; i++)
                {
                    if (i != 0) std::cout << ", ";
                    std::cout << thecl_value_to_text(&values[i]);
                }
            }
            std::cout << ");\n";
        }
        std::cout << "}\n";
    }
}

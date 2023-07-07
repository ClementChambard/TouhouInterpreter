#include "./EclRaw.h"
#include <fstream>

EclRaw_t* ecl_open(std::string filename) {
    std::ifstream file(filename, std::ios::binary);
    if (file.fail()) {
        std::cerr << "Invalid file name: " << filename << '\n';
        return nullptr;
    }
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    fileSize -= file.tellg();
    unsigned char* buf = new unsigned char[fileSize];
    file.read(reinterpret_cast<char*>(buf), fileSize);
    file.close();

    /* Input. */
    int64_t file_size;
    unsigned char* map;

    /* Temporary. */
    const char* string_data;
    size_t i;

    /* Output data. */

    file_size = fileSize;
    if (file_size == -1) {
        delete[] buf;
        return NULL;
    }

    map = buf;
    if (!map)
        return NULL;

    EclRaw_t* ecl = new EclRaw_t();

    ecl->header = reinterpret_cast<EclRawHeader_t*>(map);
    std::string magic = std::string(ecl->header->magic);
    if (magic[0] != 'S' || magic[1] != 'C' ||
        magic[2] != 'P' || magic[3] != 'T') {
        delete[] map;
        delete ecl;
        std::cerr << "thecl:" << filename << ": SCPT signature missing\n";
        return NULL;
    }

    const EclRawIncList_t* anim_list =
        reinterpret_cast<EclRawIncList_t*>(map + ecl->header->include_offset);
    magic = std::string(anim_list->magic);
    if (magic[0] != 'A' || magic[1] != 'N' ||
        magic[2] != 'I' || magic[3] != 'M') {
        delete[] map;
        delete ecl;
        std::cerr << "thecl:" << filename << ": ANIM signature missing\n";
        return NULL;
    }

    string_data = reinterpret_cast<const char*>(anim_list->data);
    for (i = 0; i < anim_list->count; ++i) {
        ecl->anim_list.push_back(std::string(string_data));
        string_data += ecl->anim_list[i].length() + 1;
    }

    while (reinterpret_cast<ptrdiff_t>(string_data) % 4)
        ++string_data;
    const EclRawIncList_t* ecli_list =
        reinterpret_cast<const EclRawIncList_t*>(string_data);
    magic = std::string(ecli_list->magic);
    if (magic[0] != 'E' || magic[1] != 'C' ||
        magic[2] != 'L' || magic[3] != 'I') {
        std::cerr << "thecl:" << filename << ": ECLI signature missing\n";
        return NULL;
    }

    string_data = reinterpret_cast<const char*>(ecli_list->data);
    for (i = 0; i < ecli_list->count; ++i) {
        ecl->ecli_list.push_back(std::string(string_data));
        string_data += ecl->ecli_list[i].length() + 1;
    }

    while (reinterpret_cast<ptrdiff_t>(string_data) % 4)
        ++string_data;
    const uint32_t* sub_offsets = reinterpret_cast<const uint32_t*>(
        string_data + ((4 - reinterpret_cast<ptrdiff_t>(string_data)) % 4));

    string_data =
        reinterpret_cast<const char*>(sub_offsets + ecl->header->sub_count);

    for (i = 0; i < ecl->header->sub_count; ++i) {

        std::string name = std::string(string_data);
        string_data += name.length() + 1;

        const EclRawSub_t* raw_sub =
            reinterpret_cast<const EclRawSub_t*>(map + sub_offsets[i]);

        ecl->subs.push_back(EclSubPtr_t { name, raw_sub, {} });

        magic = std::string(raw_sub->magic);
        if (magic[0] != 'E' || magic[1] != 'C' ||
            magic[2] != 'L' || magic[3] != 'H') {
            std::cerr << "thecl:" << filename << ": ECLH signature missing\n";
            return NULL;
        }

        const EclRawInstr_t* instr;
        for (instr = reinterpret_cast<const EclRawInstr_t*>(raw_sub->data);
             reinterpret_cast<const unsigned char*>(instr) != map + file_size &&
             reinterpret_cast<const unsigned char*>(instr) != map +
             sub_offsets[i + 1]; instr = reinterpret_cast<const EclRawInstr_t*>(
                reinterpret_cast<const unsigned char*>(instr) + instr->size))
            ecl->subs.back().instrs.push_back(instr);
    }

    return ecl;
}

void ecl_close(EclRaw_t* ecl) {
    delete[] reinterpret_cast<const char*>(ecl->header);
    delete ecl;
}

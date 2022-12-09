#include "stdOpener.h"
#include <fstream>

namespace StdOpener {

    const std::vector<std::pair<uint32_t, std::string>> formats_v2 = {
        { 0, "" },
        { 1, "SS" },
        { 2, "fff" },
        { 3, "SSfff" },
        { 4, "fff" },
        { 5, "SSfff" },
        { 6, "fff" },
        { 7, "f" },
        { 8, "Cff" },
        { 9, "SSCff" },
        { 10, "SSfffffffff" },
        { 12, "S" },
        { 13, "S" },
        { 14, "SSS" },
        { 16, "S" },
        { 17, "S" },
        { 18, "SSfff" },
        { 19, "S" },
        { 21, "SSf"},
        { 0, "NULL" }
    };

    thstd_t* std_read_file(std::string const& filename)
    {
        std::ifstream file(filename, std::ios::binary);
        if (file.fail()) return nullptr;
        file.seekg(0, std::ios::end);
        std::streamoff fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        fileSize -= file.tellg();
        unsigned char* buf = new unsigned char[fileSize];
        file.read(reinterpret_cast<char*>(buf), fileSize);
        file.close();

        long file_size;
        unsigned char* map;
        unsigned char* map_base;

        std_header_t* header;
        std_entry_t* entry;
        std_entry_header_t* entry_header;
        std_object_instance_t* instance;
        std_instr_t* instr;
        uint32_t* offsets;

        thstd_t* std = new thstd_t();
        std->map_size = file_size = static_cast<long>(fileSize);
        std->map = map_base = buf;
        map = map_base;

        std->header = header = reinterpret_cast<std_header_t*>(map);
        std->header_06 = reinterpret_cast<std_header_06_t*>(map);
        std->header_10 = reinterpret_cast<std_header_10_t*>(map);

        offsets = reinterpret_cast<uint32_t*>(map_base + sizeof(std_header_10_t));

        for (size_t i = 0; i < header->nb_objects; i++)
        {
            entry = new std_entry_t();
            std->entries.push_back(entry);

            map = map_base + offsets[i];
            entry_header = reinterpret_cast<std_entry_header_t*>(map);
            entry->header = entry_header;
            map += sizeof(*entry_header);

            while (true)
            {
                if (*reinterpret_cast<unsigned int*>(map) == 0x0004FFFF) break;
                entry->quads.push_back(reinterpret_cast<std_object_t*>(map));

                map = map + sizeof(std_object_t);
            }
        }

        map = map_base + std->header_10->faces_offset;

        while (true)
        {
            instance = reinterpret_cast<std_object_instance_t*>(map);
            if (instance->object_id == 0xFFFF) break;

            std->instances.push_back(instance);
            map = map + sizeof(std_object_instance_t);
        }

        instr = reinterpret_cast<std_instr_t*>(map_base + std->header_10->script_offset);

        while (true)
        {
            if (instr->size == 0xFFFF) break;
            std->instrs.push_back(instr);
            instr = reinterpret_cast<std_instr_t*>(reinterpret_cast<char*>(instr) + instr->size);
        }

        return std;
    }

    void std_free(thstd_t* std)
    {
        for (auto entry : std->entries)
        {
            entry->quads.clear();
            delete entry;
        }

        std->entries.clear();

        std->instances.clear();
        std->instrs.clear();

        delete[] std->map;
        delete std;
    }

    void quad_dump(const std_object_t* object)
    {
        std::cout << "\n    QUAD:\n";
        std::cout << "        Type: " << object->unknown << '\n';
        std::cout << "        Scr_id: " << object->script_index << '\n';
        std::cout << "        Position: " << object->x << ' ' << object->y << ' ' << object->z << '\n';
        std::cout << "        Padding: " << object->_padding << '\n';
        std::cout << "        Width: " << object->width << '\n';
        std::cout << "        Height: " << object->height << '\n';
    }

    void entry_dump(const std_entry_t* entry, uint16_t& objID)
    {
        std_entry_header_t* header = entry->header;
        std::cout << "\nENTRY " << objID++ << ":\n";
        std::cout << "    Unknown: " << header->unknown << '\n';
        std::cout << "    Position: " << header->x << ' ' << header->y << ' ' << header->z << '\n';
        std::cout << "    Depth: " << header->depth << '\n';
        std::cout << "    Width: " << header->width << '\n';
        std::cout << "    Height: " << header->height << '\n';
        for (auto q : entry->quads) quad_dump(q);
    }

    void objInst_dump(const std_object_instance_t* instance)
    {
        std::cout << "    FACE: " << instance->object_id
                  << ' ' << instance->unknown1 << ' '
                  << instance->x << ' ' << instance->y << ' ' << instance->z << '\n';
    }

    void instr_dump(const std_instr_t* instr, uint32_t& time)
    {
        // TODO
    }

    void std_dump(const thstd_t* std)
    {
        uint16_t object_id = 0;
        uint32_t time = 0;

        std::cout << "ANM: " << std->header_10->anm_name << '\n';
        std::cout << "Std_unknown: " << std->header_10->unknown << '\n';

        for (auto e : std->entries) entry_dump(e, object_id);

        std::cout << '\n';

        for (auto i : std->instances) objInst_dump(i);

        std::cout << "\nSCRIPT:\n";

        for (auto i : std->instrs) instr_dump(i, time);
    }

}

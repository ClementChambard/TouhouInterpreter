#include "./anmOpener.h"
#include "./image.h"
#include <cassert>
#include <fstream>
#include <string.h>
#include <Error.h>

namespace anm::opener {

const std::vector<std::pair<u32, cstr>> formats_v8 = {
    { 0, "" }, // nop()
    { 1, "" }, // delete()
    { 2, "" }, // static()
    { 3, "" }, // stop()
    { 4, "" }, // stopHide()
    { 5, "S" }, // interruptLabel(id)
    { 6, "S" }, // wait(time)
    { 7, "" }, // caseReturn()
    { 100, "SS" }, // =
    { 101, "ff" }, // =
    { 102, "SS" }, // +
    { 103, "ff" }, // +
    { 104, "SS" }, // -
    { 105, "ff" }, // -
    { 106, "SS" }, // *
    { 107, "ff" }, // *
    { 108, "SS" }, // /
    { 109, "ff" }, // /
    { 110, "SS" }, // %
    { 111, "ff" }, // %
    { 112, "SSS" }, // = +
    { 113, "fff" }, // = +
    { 114, "SSS" }, // = +
    { 115, "fff" }, // = +
    { 116, "SSS" }, // = +
    { 117, "fff" }, // = +
    { 118, "SSS" }, // = +
    { 119, "fff" }, // = +
    { 120, "SSS" }, // = +
    { 121, "fff" }, // = +
    { 122, "SS" }, // = rand(n)
    { 123, "ff" }, // = rand(n)
    { 124, "ff" }, // = sin(x)
    { 125, "ff" }, // = cos(x)
    { 126, "ff" }, // = tan(x)
    { 127, "ff" }, // = acos(x)
    { 128, "ff" }, // = atan(x)
    { 129, "f" }, // %2pi
    { 130, "ffff" }, // x,y = circlePos(r,a)
    { 131, "ffff" }, // x,y = circlePos(rand(r1,r2),randAngle)
    { 200, "SS" }, // jmp          dest t
    { 201, "SSS" }, // jmdec x      dest t
    { 202, "SSSS" }, // jmpif a == b dest t
    { 203, "ffff" }, // jmpif a == b dest t
    { 204, "SSSS" }, // jmpif a != b dest t
    { 205, "ffff" }, // jmpif a != b dest t
    { 206, "SSSS" }, // jmpif a <  b dest t
    { 207, "ffff" }, // jmpif a <  b dest t
    { 208, "SSSS" }, // jmpif a <= b dest t
    { 209, "ffff" }, // jmpif a <= b dest t
    { 210, "SSSS" }, // jmpif a >  b dest t
    { 211, "ffff" }, // jmpif a >  b dest t
    { 212, "SSSS" }, // jmpif a >= b dest t
    { 213, "ffff" }, // jmpif a >= b dest t
    { 300, "S" }, // sprite
    { 301, "SS" }, // sprite rand
    { 302, "S" }, // type
    { 303, "S" }, // blend
    { 304, "S" }, // layer
    { 305, "S" }, // zWriteDisable
    { 306, "S" }, // ins_306
    { 307, "S" }, // randMode
    { 308, "" }, // flipX
    { 309, "" }, // flipY
    { 310, "S" }, // visible
    { 311, "S" }, // resampleMode
    { 312, "SS" }, // scrollMode
    { 313, "S" }, // resolutionMode
    { 314, "S" }, // ins_314
    { 315, "S" }, // colorizeChildren
    { 316, "" }, // ins_316
    { 317, "" }, // ins_317
    { 400, "fff" }, // pos
    { 401, "fff" }, // rotate
    { 402, "ff" }, // scale
    { 403, "S" }, // alpha
    { 404, "SSS" }, // color
    { 405, "S" }, // alpha2
    { 406, "SSS" }, // color2
    { 407, "SSfff" }, // posTime
    { 408, "SSSSS" }, // colorTime
    { 409, "SSS" }, // alphaTime
    { 410, "SSfff" }, // rotateTime
    { 411, "SSf" }, // rotateTime2D
    { 412, "SSff" }, // scaleTime
    { 413, "SSSSS" }, // color2Time
    { 414, "SSS" }, // alpha2Time
    { 415, "fff" }, // angleVel
    { 416, "ff" }, // scaleGrowth
    { 417, "SS" }, // alphaTimeLinear
    { 418, "" }, // ins_418
    { 419, "S" }, // ins_419
    { 420, "SffSSSSffS" }, // moveBezier
    { 421, "S" }, // anchor (4bits + 4bits)
    { 422, "" }, // ins_422
    { 423, "S" }, // colorMode
    { 424, "S" }, // rotateAuto
    { 425, "f" }, // scrollX
    { 426, "f" }, // scrollY
    { 427, "SSf" }, // scrollXTime
    { 428, "SSf" }, // scrollYTime
    { 429, "ff" }, // zoomOut
    { 430, "SSff" }, // zoomOutTime
    { 431, "S" }, // ins_431
    { 432, "S" }, // ins_432
    { 433, "SSff" }, // ins_433
    { 434, "ff" }, // scale2
    { 435, "SSff" }, // scale2Time
    { 436, "ff" }, // anchorOffset
    { 437, "S" }, // rotationMode
    { 438, "S" }, // originMode
    { 439, "Sff" }, // fadeNearCamera
    { 500, "S" }, // scriptNew
    { 501, "S" }, // scriptNewUI
    { 502, "S" }, // scriptNewFront
    { 503, "S" }, // scriptNewUIFront
    { 504, "S" }, // scriptNewRoot
    { 505, "Sff" }, // scriptNewPos
    { 506, "Sff" }, // scriptNewRootPos
    { 507, "S" }, // ins_507
    { 508, "S" }, // effectNew
    { 509, "" }, // copyVars
    { 600, "S" }, // texCircle
    { 601, "S" }, // texArcEven
    { 602, "S" }, // texArc
    { 603, "ff" }, // drawRect
    { 604, "fS" }, // drawPoly
    { 605, "fS" }, // drawPolyBorder
    { 606, "ff" }, // drawRectGrad
    { 607, "ff" }, // drawRectRot
    { 608, "ff" }, // drawRectRotGrad
    { 609, "S" }, // texCylinder3D
    { 610, "S" }, // texRing3D
    { 611, "ffS" }, // drawRing
    { 612, "ff" }, // drawRectBorder
    { 613, "ff" }, // drawLine
    { 614, "ff" }, // ins_614
    { 0xffff, "" },
};

const std::vector<std::pair<u32, cstr>>& getFormats() { return formats_v8; }

float formatBpp(u32 format)
{
    auto f = static_cast<format_t>(format);
    if (f == format_t::GRAY8)
        return 1;
    if (f == format_t::ARGB4444 || f == format_t::RGB565)
        return 2;
    return 4;
}

pstr anm_get_name(anm_archive_t* archive, cstr name)
{
    pstr other_name;
    for (auto s : archive->names)
        if (s == name)
            return s;

    other_name = strdup(name);
    archive->names.push_back(other_name);
    return other_name;
}

anm_archive_t* anm_read_file(cstr filename)
{

    std::ifstream file(filename, std::ios::binary);
    if (file.fail())
        return nullptr;
    file.seekg(0, std::ios::end);
    std::streamoff fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    fileSize -= file.tellg();
    bytes buf = new byte[fileSize];
    file.read(reinterpret_cast<pstr>(buf), fileSize);
    file.close();

    anm_archive_t* archive = new anm_archive_t();

    isize file_size = archive->map_size = static_cast<isize>(fileSize);
    bytes map_base;
    bytes map;
    archive->map = map_base = buf;
    map = map_base;

    while (true) {
        anm_entry_t* entry = new anm_entry_t();
        anm_header11_t* header = reinterpret_cast<anm_header11_t*>(map);

        archive->entries.push_back(entry);
        entry->header = header;

        assert(header->version == 8);
        assert(header->lowresscale == 0 || header->lowresscale == 1);
        assert(header->hasdata == 0 || header->hasdata == 1);
        // assert( header->rt_textureslot == 0 );
        assert(header->zero1 == 0);

        entry->name = anm_get_name(archive, reinterpret_cast<const char*>(map) + header->nameoffset);
        entry->name2 = nullptr;

        assert(
            (header->hasdata == 0 || (entry->name && entry->name[0] == '@')) == (header->thtxoffset == 0));

        if (header->sprites) {
            u32* sprite_offsets = reinterpret_cast<u32*>(map + sizeof(*header));
            for (u32 s = 0; s < header->sprites; s++) {
                entry->sprites.push_back(reinterpret_cast<sprite_t*>(map + sprite_offsets[s]));
            }
        }

        if (header->scripts) {
            anm_offset_t* script_offsets = reinterpret_cast<anm_offset_t*>(map + sizeof(*header) + header->sprites * sizeof(u32));
            for (u32 s = 0; s < header->scripts; s++) {
                anm_script_t* script = new anm_script_t();
                script->offset = &(script_offsets[s]);

                unsigned char* limit = map;
                if (s < static_cast<u32>(header->scripts - 1))
                    limit += script_offsets[s + 1].offset;
                else if (header->thtxoffset)
                    limit += header->thtxoffset;
                else if (header->nextoffset)
                    limit += header->nextoffset;
                else
                    limit += file_size;

                unsigned char* instr_ptr = map + script->offset->offset;
                while (true) {
                    anm_instr_t* temp_instr = reinterpret_cast<anm_instr_t*>(instr_ptr);

                    if (instr_ptr + sizeof(anm_instr_t) > limit || temp_instr->type == 0xffff || instr_ptr + temp_instr->length > limit)
                        break;

                    script->instrs.push_back(temp_instr);

                    instr_ptr += temp_instr->length;
                }

                entry->scripts.push_back(script);
            }
        }

        if (header->hasdata) {
            thtx_header_t* thtx = entry->thtx = reinterpret_cast<thtx_header_t*>(map + header->thtxoffset);
            assert(strncmp(thtx->magic, "THTX", 4) == 0);
            assert(thtx->zero == 0);
            assert(thtx->w * thtx->h * formatBpp(thtx->format) <= thtx->size);
            assert(thtx->format == static_cast<u16>(format_t::BGRA8888) ||
                   thtx->format == static_cast<u16>(format_t::RGB565) ||
                   thtx->format == static_cast<u16>(format_t::ARGB4444) ||
                   thtx->format == static_cast<u16>(format_t::GRAY8) ||
                   thtx->format == static_cast<u16>(format_t::RGBA8888));
            entry->data = thtx->data;
        }

        if (!header->nextoffset)
            break;

        map = map + header->nextoffset;
    }

    return archive;
}

void anm_free(anm_archive_t* anm)
{
    for (auto s : anm->names)
        if (s != nullptr)
            free(s);
    anm->names.clear();

    for (auto entry : anm->entries) {
        if (entry == nullptr)
            continue;

        entry->sprites.clear();

        for (auto script : entry->scripts) {
            if (script == nullptr)
                continue;
            script->instrs.clear();
            delete script;
        }
        entry->scripts.clear();

        delete entry;
    }
    anm->entries.clear();

    delete[] anm->map;

    delete anm;
}

void util_total_entry_size(const anm_entry_t* entry, u32& widthptr, u32& heightptr)
{
    u32 width = 0;
    u32 height = 0;

    if (!entry->header->hasdata)
        return;
    if (entry->header->x + entry->thtx->w > width)
        width = entry->header->x + entry->thtx->w;
    if (entry->header->y + entry->thtx->h > height)
        height = entry->header->y + entry->thtx->h;

    widthptr = width;
    heightptr = height;
}

void image_data_from_format(anm_entry_t* entry, image_t& img)
{
    img.data = new byte[img.width * img.height * 4];

    struct pixel_t {
        u8 r = 0;
        u8 g = 0;
        u8 b = 0;
        u8 a = 0;
        pixel_t(u32 i)
        {
            r = (0xff & (i >> 0));
            g = (0xff & (i >> 8));
            b = (0xff & (i >> 16));
            a = (0xff & (i >> 24));
        }
        pixel_t(u8 r, u8 g, u8 b, u8 a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
        pixel_t()
        {
            r = 0;
            g = 0;
            b = 0;
            a = 0;
        }
    };

    format_t format = (format_t)entry->thtx->format;
    u32 pixels = (u32)entry->thtx->w * entry->thtx->h;
    bytes data = entry->thtx->data;
    std::vector<pixel_t> pixelvec(pixels);
    u16* asu16 = reinterpret_cast<u16*>(data);

    switch (format) {
    case format_t::GRAY8:
        for (u32 i = 0; i < pixels; i++)
            pixelvec[i] = { data[i], data[i], data[i], 0xff };
        break;
    case format_t::BGRA8888:
        for (u32 i = 0; i < pixels; i++)
            pixelvec[i] = { data[i * 4 + 2], data[i * 4 + 1], data[i * 4 + 0], data[i * 4 + 3] };
        break;
    case format_t::ARGB4444:
        for (u32 i = 0; i < pixels; i++) {
            /* Extends like this: 0x0 -> 0x00, 0x3 -> 0x33, 0xf -> 0xff.
             * It's required for proper alpha. */
            u32 out = ((data[i * sizeof(u16) + 1] & 0xf0) << 24 & 0xf0000000)
                | ((data[i * sizeof(u16) + 1] & 0xf0) << 20 & 0x0f000000)
                | ((data[i * sizeof(u16) + 0] & 0x0f) << 20 & 0xf00000)
                | ((data[i * sizeof(u16) + 0] & 0x0f) << 16 & 0x0f0000)
                | ((data[i * sizeof(u16) + 0] & 0xf0) << 8 & 0xf000)
                | ((data[i * sizeof(u16) + 0] & 0xf0) << 4 & 0x0f00)
                | ((data[i * sizeof(u16) + 1] & 0x0f) << 4 & 0xf0)
                | ((data[i * sizeof(u16) + 1] & 0x0f) << 0 & 0x0f);

            pixelvec[i] = out;
            // pixelvec[i].a = (float)(0x0f & data[2*i+1]); pixelvec[i].a += (pixelvec[i].a << 4);
            // pixelvec[i].r = (float)(0xf0 & data[2*i+1]); pixelvec[i].r += (pixelvec[i].r >> 4);
            // pixelvec[i].g = (float)(0x0f & data[2*i+0]); pixelvec[i].g += (pixelvec[i].g << 4);
            // pixelvec[i].b = (float)(0xf0 & data[2*i+0]); pixelvec[i].b += (pixelvec[i].b >> 4);
        }
        break;
    case format_t::RGB565:
        for (u32 i = 0; i < pixels; i++) {
            /* Bit-extends channels: 00001b -> 00001111b. */
            /*   pixelvec[i].a = 0xff;
               pixelvec[i].r = data[2*i] & 0b11111000;
               pixelvec[i].g = (((data[2*i] & 0b00000111) << 3) + ((data[2*i+1] & 0b11100000) >> 5)) * 4;
               pixelvec[i].b = (data[2*i+1] & 0b00011111) * 8;*/
            u32 out = 0xff000000
                | ((asu16[i] & 0x001f) << 19 & 0xf80000)
                | ((asu16[i] & 0x0001) << 16 & 0x040000)
                | ((asu16[i] & 0x0001) << 16 & 0x020000)
                | ((asu16[i] & 0x0001) << 16 & 0x010000)

                | ((asu16[i] & 0x07e0) << 5 & 0x00fc00)
                | ((asu16[i] & 0x0020) << 4 & 0x000200)
                | ((asu16[i] & 0x0020) << 3 & 0x000100)

                | ((asu16[i] & 0xf800) >> 8 & 0x0000f8)
                | ((asu16[i] & 0x0800) >> 9 & 0x000004)
                | ((asu16[i] & 0x0800) >> 10 & 0x000002)
                | ((asu16[i] & 0x0800) >> 11 & 0x000001);

            pixelvec[i] = out;
        }
        break;
    case format_t::RGBA8888:
        for (u32 i = 0; i < pixels; i++)
            pixelvec[i] = { data[4 * i], data[4 * i + 1], data[4 * i + 2], data[4 * 1 + 3] };
        break;
    }

    for (u32 yy = 0; yy < img.height; yy++)
        for (u32 xx = 0; xx < img.width; xx++) {
            u32 posOut = (xx + img.width * yy) * 4;
            u32 posIn = (xx - entry->header->x) + entry->thtx->w * (yy - entry->header->y);
            pixel_t p = (yy < entry->header->y || xx < entry->header->x) ? pixel_t({ 0xff, 0xff, 0xff, 0xff }) : pixelvec[posIn];
            img.data[posOut + 0] = p.r;
            img.data[posOut + 1] = p.g;
            img.data[posOut + 2] = p.b;
            img.data[posOut + 3] = p.a;
        }
}

image_t anm_get_image(anm_entry_t* entry)
{
    image_t image = {};

    image.width = 0;
    image.height = 0;
    image.format = format_t::RGBA8888;
    image.data = nullptr;

    util_total_entry_size(entry, image.width, image.height);

    if (image.width == 0 || image.height == 0)
        return image;

    image_data_from_format(entry, image);

    return image;
}

#define FIND_FORMAT_ERROR "FORMAT-NULL-ERROR"
cstr find_format(const std::vector<std::pair<u32, cstr>>& fmts, u32 id)
{
    for (size_t i = 0; i < fmts.size(); i++) {
        if (fmts[i].first == id)
            return fmts[i].second;
    }
    return FIND_FORMAT_ERROR;
}

std::vector<anm_instr_arg_t> anm_instr_get_args(anm_instr_t* instr)
{
    cstr format = find_format(formats_v8, instr->type);
    if (strcmp(format, FIND_FORMAT_ERROR) == 0) {
        ns::error("id", instr->type, "was not found in the format table");
        abort();
    }
    std::vector<anm_instr_arg_t> args;
    if (instr->length > sizeof(anm_instr_t))
        for (size_t i = 0; i < (instr->length - sizeof(anm_instr_t)); i += 4) {
            anm_instr_arg_t arg = { { 0 }, format[i / 4] };
            arg.val.S = *reinterpret_cast<int32_t*>(instr->data + i);
            args.push_back(arg);
        }
    return args;
}

} // namespace anm::opener

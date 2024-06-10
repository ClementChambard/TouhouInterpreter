#include "./thecl_value.h"

#include <logger.h>
#include <stdlib.h>
#include <memory.h>
#include <cstring>

isize thecl_value_from_data(robytes data,
        usize data_length, char type, thecl_value_t* value) {
#define READ(x, n)                                                             \
    if (data_length < n) {                                                     \
        NS_ERROR("thecl:value_from_data: unexpected"                           \
            "end of data, wanted to read %llu bytes for format '#x'", n);      \
        return -1;                                                             \
    }                                                                          \
    ns::mem_copy(&value->val.x, data, n);                                      \
    return n;

    value->type = type;

    switch (value->type) {
    case 'f':
        READ(f, sizeof(f32));
    case 'd':
        READ(d, sizeof(f64));
    case 'b':
        READ(b, sizeof(u8));
    case 'c':
        READ(c, sizeof(i8));
    case 'u':
        READ(u, sizeof(u16));
    case 's':
        READ(s, sizeof(i16));
    case 'U':
        READ(U, sizeof(u32));
    case 'o':
    case 't':
        value->type = 'S'; /* Fallthrough */
    case 'S':
        READ(S, sizeof(i32));
    case 'C':
        READ(C, sizeof(u32));
        break;
    case 'z':
        value->val.z = new char[data_length];
        ns::mem_copy(value->val.z, data, data_length);
        return data_length;
    case 'm':
        value->val.m.length = data_length;
        value->val.m.data = new byte[data_length];
        ns::mem_copy(value->val.m.data, data, data_length);
        return data_length;
    default:
        NS_ERROR("thecl:value_from_data: invalid type %d", value->type);
        return -1;
    }

#undef READ
}

struct thecl_sub_param_t {
    char from;
    char to;
    u16 zero; /* Padding, must be set to 0 */
    union {
        i32 S;
        f32 f;
    } val;
};

static isize th10_value_from_data(robytes data,
            usize data_length, char type, thecl_value_t* value) {
    switch (type) {
    case 'D':
        return thecl_value_from_data(data,
                                    sizeof(thecl_sub_param_t), 'm', value);
    case 'm':
    case 'x': {
        u32 length;
        ns::mem_copy(&length, data, sizeof(u32));
        thecl_value_t temp;

        thecl_value_from_data(data + sizeof(length), length, 'm', &temp);
        if (type == 'x') {
            for (size_t i = 0; i < length; ++i) {
                const int ip = i - 1;
                temp.val.m.data[i] ^= 0x77 + i * 7 + (ip * ip + ip) / 2 * 16;
            }
        }
        value->type = 'z';
        value->val.z = reinterpret_cast<char*>(temp.val.m.data);

        return sizeof(u32) + length;
    }
    default:
        return thecl_value_from_data(data, data_length, type, value);
    }
}

thecl_value_t* thecl_value_list_from_data(const unsigned char* data,
                                size_t data_length, const char* format) {
    size_t format_length = strlen(format);
    size_t i = 0;
    thecl_value_t* values = NULL;

    for (; i < format_length; ++i) {
        char f = format[i];
        int repeat = 0;

        if (f == '*') {
            repeat = 1;
            f = format[i + 1];
            if (!data_length)
                break;
        }

        do {
            values = reinterpret_cast<thecl_value_t*>(
                realloc(values, (i + 1) * sizeof(thecl_value_t)));
            isize incr = th10_value_from_data(data, data_length, f, &values[i]);
            if (incr == -1) {
                /* XXX: Leaks */
                // static_cast<unsigned char>( values, expecting program te
                return NULL;
            }

            data += incr;
            data_length -= incr;

            /* TODO: Check data_length and break when it
             * is 0, warn below if all formats haven't been parsed. */
            if (repeat)
                ++i;
        } while (repeat && data_length);

        if (repeat)
            break;
    }

    if (data_length)
        NS_ERROR("thecl:value_list_from_data: %zu bytes left over "
                "when parsing format \"%s\"\n", data_length, format);

    values = reinterpret_cast<thecl_value_t*>(realloc(values, (i + 1) *
                                                      sizeof(thecl_value_t)));
    ns::mem_set(&values[i], 0, sizeof(thecl_value_t));

    return values;
}

void thecl_value_free(thecl_value_t* value) {
    if (value->type == 'z') {
        delete[] value->val.z;
        value->val.z = NULL;
    } else if (value->type == 'm') {
        delete[] value->val.m.data;
        value->val.m.data = NULL;
        value->val.m.length = 0;
    }
    value->type = 0;
}

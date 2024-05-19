#ifndef THECL_VALUE_H_
#define THECL_VALUE_H_

#include <defines.h>

struct thecl_value_t {
    int type;
    union {
        f32 f;
        f64 d;
        u8 b;
        i8 c;
        u16 u;
        i16 s;
        u32 U;
        i32 S;
        pstr z;
        char C[4];
        struct {
            usize length;
            bytes data;
        } m;
    } val;
};

/* Reads a value_t from a block of data.  Returns the number of bytes read, or
 * -1 on error.  The passed value is filled in. */
isize thecl_value_from_data(robytes data, usize data_length,
                              char type, thecl_value_t* value);

/* Reads a list of values, the list is terminated by a value with type 0.  NULL
 * is returned on error. */
thecl_value_t* thecl_value_list_from_data(robytes data,
                              usize data_length, cstr data_format);

/* Returns a text representation of a value. */
pstr thecl_value_to_text(const thecl_value_t* value);

/* Returns the size of the value's data form, or -1 on error. */
isize thecl_value_size(const thecl_value_t* value);

/* Frees contained data. */
void thecl_value_free(thecl_value_t* value);


#endif // THECL_VALUE_H_

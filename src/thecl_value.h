#ifndef THECL_VALUE_H_
#define THECL_VALUE_H_

#include <iostream>
#include <cstdint>

struct thecl_value_t {
    int type;
    union {
        float f;
        double d;
        unsigned char b;
        signed char c;
        uint16_t u;
        int16_t s;
        uint32_t U;
        int32_t S;
        char* z;
        char C[4];
        struct {
            size_t length;
            unsigned char* data;
        } m;
    } val;
};

/* Reads a value_t from a block of data.  Returns the number of bytes read, or
 * -1 on error.  The passed value is filled in. */
ssize_t thecl_value_from_data(const unsigned char* data, size_t data_length,
                              char type, thecl_value_t* value);

/* Reads a list of values, the list is terminated by a value with type 0.  NULL
 * is returned on error. */
thecl_value_t* thecl_value_list_from_data(const unsigned char* data,
                              size_t data_length, const char* data_format);

/* Returns a text representation of a value. */
char* thecl_value_to_text(const thecl_value_t* value);

/* Returns the size of the value's data form, or -1 on error. */
ssize_t thecl_value_size(const thecl_value_t* value);

/* Frees contained data. */
void thecl_value_free(thecl_value_t* value);


#endif // THECL_VALUE_H_

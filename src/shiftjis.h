#ifndef SHIFTJIS_INCLUDED_H
#define SHIFTJIS_INCLUDED_H

// https://stackoverflow.com/questions/33165171/c-shiftjis-to-utf8-conversion
#include <string>
#include <cstdint>

std::string sj2utf8(const std::string &input);
void util_xor(unsigned char* data, size_t data_length, unsigned char key,
                     unsigned char step, unsigned char step2);

#endif // !SHIFTJIS_INCLUDED_H

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iomanip>
#include <sstream>
#include <random>
#include <array>

using uint32 = std::array<unsigned char, 32 / 8>;
using hash = uint32;

std::string string2hex(const std::uint8_t *data, int len);

std::string string2hex(const std::string &data);

std::string string2hex(const std::int8_t *data, int len);

std::string string2hex(const std::vector<std::uint8_t> data);


uint32 MakeUint32();


#endif // !UTIL_H
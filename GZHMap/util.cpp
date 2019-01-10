#include <string>
#include <iomanip>
#include <sstream>
#include <random>
#include "util.h"

std::string string2hex(const std::uint8_t *data, int len)
{
    std::string result;
    std::string tmp;
    std::stringstream ss;
    for (int i = 0; i < len; i++)
    {
        int r = int(data[i]);
        ss << std::hex << std::setw(2)
            << std::setfill('0') << r << std::endl;
        ss >> tmp;
        result += tmp;
    }
    return result;
}

std::string string2hex(const std::string &data)
{
    return string2hex((std::uint8_t*)data.data(), data.size());
}

std::string string2hex(const std::int8_t *data, int len)
{
    return string2hex((std::uint8_t*)data, len);
}

std::string string2hex(const std::vector<std::uint8_t> data)
{
    return string2hex(data.data(), data.size());
}


uint32 MakeUint32()
{
    static std::mt19937_64 eng{ 5 };
    uint32 u;
    auto e = eng();
    for (int i = 0; i < 4; i++)
    {
        u[i] = e & 0xFF;
        e = e >> 8;
    }
    return u;
}
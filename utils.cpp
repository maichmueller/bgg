//
// Created by Michael on 08/03/2019.
//

#include "utils.h"

using namespace utils;

std::string utils::repeat(std::string str, const std::size_t n)
{
    if (n == 0) {
        str.clear();
        str.shrink_to_fit();
        return str;
    } else if (n == 1 || str.empty()) {
        return str;
    }
    const auto period = str.size();
    if (period == 1) {
        str.append(n - 1, str.front());
        return str;
    }
    str.reserve(period * n);
    std::size_t m {2};
    for (; m < n; m *= 2) str += str;
    str.append(str.c_str(), (n - (m / 2)) * period);
    return str;
}

std::string utils::center(const std::string &str, int width, const char* fillchar) {

    int len = str.length();
    if(width < len) { return str; }

    int diff = width - len;
    int pad1 = diff/2;
    int pad2 = diff - pad1;
    return std::string(pad1, *fillchar) + str + std::string(pad2, *fillchar);
}

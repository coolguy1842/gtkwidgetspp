#pragma once

#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

namespace Util::String {

constexpr uint64_t hashFunc(const char* str, size_t offset = 0) {
    return !str[offset] ? 5381 : (hashFunc(str, offset + 1) * 33) ^ str[offset];
}

std::vector<std::string> split(std::string str, const char* delim);
std::string replace(std::string str, std::string find, std::string replace);

};  // namespace Util::String

constexpr uint64_t operator""_hash(const char* str, size_t len) {
    return Util::String::hashFunc(str, 0);
}
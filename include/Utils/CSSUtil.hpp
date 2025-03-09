#pragma once

#include <optional>
#include <string>

namespace Util::CSS {

std::optional<std::string> SCSStoCSS(std::string scss,
                                     std::string includePath = "./");
std::optional<std::string> loadCSSFromFile(std::string path,
                                           std::string includePath = "./");

};  // namespace Util::CSS
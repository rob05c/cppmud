#pragma once
#include <string>
#include <vector>

namespace util {

bool                     has_prefix(std::string a, std::string b);
std::string              to_lower(std::string st);
std::vector<std::string> split(std::string st, std::string token);
std::string join(std::vector<std::string> st, std::string separator); // TODO templatize

};

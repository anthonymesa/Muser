#pragma once

#include <vector>
#include <string>

void SetOptions(int argc, char *argv[]);
std::vector<std::string> SplitString(const std::string& line, const char& delimiter);

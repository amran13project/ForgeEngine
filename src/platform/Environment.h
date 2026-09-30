#pragma once
#include <string>

namespace forge::platform {

std::string getEnvironmentVariable(const std::string& name);
bool setEnvironmentVariable(const std::string& name, const std::string& value);
bool unsetEnvironmentVariable(const std::string& name);

} // namespace forge::platform
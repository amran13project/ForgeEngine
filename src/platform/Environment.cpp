#include "platform/Environment.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <cstdlib>
#endif

namespace forge::platform {

std::string getEnvironmentVariable(const std::string& name) {
#ifdef _WIN32
    DWORD size = GetEnvironmentVariableA(name.c_str(), nullptr, 0);
    if (size == 0) return {};
    std::string value(size, '\0');
    const DWORD written = GetEnvironmentVariableA(name.c_str(), value.data(), size);
    if (written == 0) return {};
    value.resize(written);
    return value;
#else
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : std::string();
#endif
}

bool setEnvironmentVariable(const std::string& name, const std::string& value) {
#ifdef _WIN32
    return SetEnvironmentVariableA(name.c_str(), value.c_str()) != FALSE;
#else
    return ::setenv(name.c_str(), value.c_str(), 1) == 0;
#endif
}

bool unsetEnvironmentVariable(const std::string& name) {
#ifdef _WIN32
    return SetEnvironmentVariableA(name.c_str(), nullptr) != FALSE;
#else
    return ::unsetenv(name.c_str()) == 0;
#endif
}

} // namespace forge::platform
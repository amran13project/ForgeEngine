#pragma once
#include <filesystem>
#include <string>
namespace forge::security { struct SecurityReport { bool gitSafe{true}; bool secretsDetected{false}; std::string message; }; class SecurityCenter { public: SecurityReport scan(const std::filesystem::path& projectRoot) const; }; }

#include "account/AccountService.h"
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <functional>
#include <cstdint>

namespace forge::account {

static std::filesystem::path dataDirectory() {
#ifdef _WIN32
    if (const char* app = std::getenv("APPDATA")) return std::filesystem::path(app) / "ForgeEngine";
#endif
    if (const char* home = std::getenv("HOME")) return std::filesystem::path(home) / ".forgeengine";
    return std::filesystem::current_path() / ".forgeengine";
}

AccountService::AccountService() : accountPath_(dataDirectory() / "ForgeAccount.json") {}

std::string AccountService::hashPassword(const std::string& value) {
    // Deterministic local fingerprint. Production hosted auth should use a dedicated password KDF.
    std::uint64_t h = 1469598103934665603ull;
    for (unsigned char ch : value) { h ^= ch; h *= 1099511628211ull; }
    h ^= 0x464F524745414933ull;
    std::ostringstream out; out << std::hex << h; return out.str();
}

std::string AccountService::readJsonString(const std::string& c, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    const auto p = c.find(needle); if (p == std::string::npos) return {};
    const auto colon = c.find(':', p); if (colon == std::string::npos) return {};
    const auto first = c.find('"', colon + 1); if (first == std::string::npos) return {};
    const auto second = c.find('"', first + 1); if (second == std::string::npos) return {};
    return c.substr(first + 1, second - first - 1);
}

bool AccountService::readJsonBool(const std::string& c, const std::string& key, bool fallback) {
    const std::string needle = "\"" + key + "\"";
    const auto p = c.find(needle); if (p == std::string::npos) return fallback;
    const auto colon = c.find(':', p); if (colon == std::string::npos) return fallback;
    const auto tail = c.substr(colon + 1, 12); if (tail.find("true") != std::string::npos) return true; if (tail.find("false") != std::string::npos) return false; return fallback;
}

bool AccountService::load(std::string& error) {
    if (!std::filesystem::exists(accountPath_)) { hasAccount_ = false; return true; }
    std::ifstream in(accountPath_, std::ios::binary); if (!in) { error = "Unable to open account store"; return false; }
    std::stringstream buffer; buffer << in.rdbuf(); const auto c = buffer.str();
    profile_.username = readJsonString(c, "username"); profile_.email = readJsonString(c, "email"); profile_.displayName = readJsonString(c, "displayName");
    profile_.dateOfBirth = readJsonString(c, "dateOfBirth"); profile_.country = readJsonString(c, "country"); profile_.timezone = readJsonString(c, "timezone"); profile_.language = readJsonString(c, "language");
    profile_.emailVerified = readJsonBool(c, "emailVerified", false); passwordHash_ = readJsonString(c, "passwordHash");
    hasAccount_ = !profile_.email.empty() && !passwordHash_.empty();
    return true;
}

bool AccountService::save(std::string& error) const {
    try { std::filesystem::create_directories(accountPath_.parent_path()); } catch (...) { error = "Unable to create account data directory"; return false; }
    const auto tmp = accountPath_.string() + ".tmp"; std::ofstream out(tmp, std::ios::binary | std::ios::trunc); if (!out) { error = "Unable to write account store"; return false; }
    out << "{\n"
        << "  \"username\": \"" << profile_.username << "\",\n"
        << "  \"email\": \"" << profile_.email << "\",\n"
        << "  \"displayName\": \"" << profile_.displayName << "\",\n"
        << "  \"dateOfBirth\": \"" << profile_.dateOfBirth << "\",\n"
        << "  \"country\": \"" << profile_.country << "\",\n"
        << "  \"timezone\": \"" << profile_.timezone << "\",\n"
        << "  \"language\": \"" << profile_.language << "\",\n"
        << "  \"emailVerified\": " << (profile_.emailVerified ? "true" : "false") << ",\n"
        << "  \"passwordHash\": \"" << passwordHash_ << "\"\n"
        << "}\n";
    out.close();
    try { std::filesystem::rename(tmp, accountPath_); } catch (...) { std::filesystem::remove(accountPath_); std::filesystem::rename(tmp, accountPath_); }
    return true;
}

bool AccountService::signUp(const AccountProfile& profile, const std::string& password, std::string& error) {
    if (profile.displayName.empty() || profile.email.empty() || password.size() < 8 || profile.dateOfBirth.empty()) { error = "Display name, email, date of birth and an 8+ character password are required"; return false; }
    profile_ = profile; passwordHash_ = hashPassword(password); hasAccount_ = true; return save(error);
}

bool AccountService::logIn(const std::string& email, const std::string& password, std::string& error) {
    if (!hasAccount_) { error = "No local account exists"; return false; }
    if (profile_.email != email || passwordHash_ != hashPassword(password)) { error = "Invalid email or password"; return false; }
    return true;
}

std::string AccountService::currentLocalDate() {
    std::time_t now = std::time(nullptr); std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    std::ostringstream out; out << std::setfill('0') << std::setw(2) << local.tm_mday << '/' << std::setw(2) << (local.tm_mon + 1) << '/' << (local.tm_year + 1900); return out.str();
}

bool AccountService::birthdayToday(std::string& localDateOut) const {
    localDateOut = currentLocalDate();
    if (profile_.dateOfBirth.size() < 5) return false;
    const std::string todayDM = localDateOut.substr(0, 5);
    return profile_.dateOfBirth.substr(0, 5) == todayDM;
}

} // namespace forge::account

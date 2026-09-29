#pragma once
#include <filesystem>
#include <string>

namespace forge::account {

struct AccountProfile {
    std::string username;
    std::string email;
    std::string displayName;
    std::string dateOfBirth;
    std::string country;
    std::string timezone;
    std::string language;
    bool emailVerified{false};
};

class AccountService {
public:
    AccountService();
    bool load(std::string& error);
    bool save(std::string& error) const;
    bool hasStoredAccount() const { return hasAccount_; }
    const AccountProfile& profile() const { return profile_; }
    AccountProfile& profile() { return profile_; }
    bool signUp(const AccountProfile& profile, const std::string& password, std::string& error);
    bool logIn(const std::string& email, const std::string& password, std::string& error);
    bool birthdayToday(std::string& localDateOut) const;
    std::filesystem::path accountPath() const { return accountPath_; }
private:
    static std::string hashPassword(const std::string& value);
    static std::string readJsonString(const std::string& content, const std::string& key);
    static bool readJsonBool(const std::string& content, const std::string& key, bool fallback);
    static std::string currentLocalDate();
    std::filesystem::path accountPath_;
    AccountProfile profile_{};
    std::string passwordHash_;
    bool hasAccount_{false};
};

} // namespace forge::account

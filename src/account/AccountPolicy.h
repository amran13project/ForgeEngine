#pragma once
#include <string>

namespace forge::account {

enum class AgeBracket { Unknown, Child, Teen, Adult };

class AccountPolicy {
public:
    static bool isValidDateOfBirth(const std::string& dob);
    static AgeBracket classify(const std::string& dob);
    static bool birthdayMatches(const std::string& dob, const std::string& localDate);
    static std::string birthdayGreeting(const std::string& displayName);
    static std::string ageBracketName(AgeBracket bracket);
};

} // namespace forge::account

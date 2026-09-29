#include "account/AccountPolicy.h"
#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace forge::account {

namespace {
bool parseDob(const std::string& value, int& day, int& month, int& year) {
    if (value.size() != 10 || value[2] != '/' || value[5] != '/') return false;
    for (size_t i = 0; i < value.size(); ++i) {
        if (i == 2 || i == 5) continue;
        if (!std::isdigit(static_cast<unsigned char>(value[i]))) return false;
    }
    day = std::stoi(value.substr(0, 2));
    month = std::stoi(value.substr(3, 2));
    year = std::stoi(value.substr(6, 4));
    if (year < 1900 || month < 1 || month > 12 || day < 1 || day > 31) return false;
    static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxDay = days[month - 1];
    const bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (month == 2 && leap) maxDay = 29;
    return day <= maxDay;
}

void today(int& day, int& month, int& year) {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
#ifdef _WIN32
    localtime_s(&local, &now);
#else
    localtime_r(&now, &local);
#endif
    day = local.tm_mday;
    month = local.tm_mon + 1;
    year = local.tm_year + 1900;
}
} // namespace

bool AccountPolicy::isValidDateOfBirth(const std::string& dob) {
    int d=0,m=0,y=0;
    if (!parseDob(dob,d,m,y)) return false;
    int td=0,tm=0,ty=0; today(td,tm,ty);
    return y <= ty && !(y == ty && (m > tm || (m == tm && d > td)));
}

AgeBracket AccountPolicy::classify(const std::string& dob) {
    int d=0,m=0,y=0;
    if (!isValidDateOfBirth(dob) || !parseDob(dob,d,m,y)) return AgeBracket::Unknown;
    int td=0,tm=0,ty=0; today(td,tm,ty);
    int age = ty - y;
    if (m > tm || (m == tm && d > td)) --age;
    if (age < 13) return AgeBracket::Child;
    if (age < 18) return AgeBracket::Teen;
    return AgeBracket::Adult;
}

bool AccountPolicy::birthdayMatches(const std::string& dob, const std::string& localDate) {
    if (!isValidDateOfBirth(dob) || localDate.size() < 5) return false;
    return dob.substr(0,5) == localDate.substr(0,5);
}

std::string AccountPolicy::birthdayGreeting(const std::string& displayName) {
    const std::string name = displayName.empty() ? "Creator" : displayName;
    return "Happy Birthday, " + name + "! Forge AI wishes you a great day and another year of creating.";
}

std::string AccountPolicy::ageBracketName(AgeBracket bracket) {
    switch (bracket) {
        case AgeBracket::Child: return "Child";
        case AgeBracket::Teen: return "Teen";
        case AgeBracket::Adult: return "Adult";
        default: return "Unknown";
    }
}

} // namespace forge::account

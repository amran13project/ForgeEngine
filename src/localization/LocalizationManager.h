#pragma once
#include <string>
#include <vector>
#include <utility>
namespace forge::localization {
class LocalizationManager { public: void setLanguage(std::string language){language_=std::move(language);} const std::string& language() const{return language_;} static std::vector<std::string> supportedLanguages(); private: std::string language_{"English"}; };
}

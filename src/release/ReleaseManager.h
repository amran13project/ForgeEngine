#pragma once
#include <filesystem>
#include <string>
namespace forge::release { struct ReleaseRecord { std::string version{"1.0.0"}; std::string buildNumber{"1"}; std::string channel{"Development"}; std::filesystem::path artifact; }; class ReleaseManager { public: bool writeManifest(const std::filesystem::path& root,const ReleaseRecord&,std::string& error) const; }; }

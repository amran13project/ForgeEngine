#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace forge::publish {

enum class Store { GooglePlay, AppleAppStore, MicrosoftStore, Steam, Direct };

struct StoreProfile {
    Store store{Store::Direct};
    std::string appName;
    std::string publisher;
    std::string description;
    std::string packageId;
    std::string version{"1.0.0"};
    std::string buildNumber{"1"};
    std::string category;
    std::string language{"English"};
    std::string privacyUrl;
};

struct ValidationIssue { bool error; std::string field; std::string message; };
struct PublishPlan { StoreProfile profile; std::vector<ValidationIssue> issues; std::filesystem::path artifactDirectory; };

class PublishCenter {
public:
    PublishPlan validate(const StoreProfile& profile, const std::filesystem::path& projectRoot) const;
    bool writeSubmissionManifest(const StoreProfile& profile, const std::filesystem::path& projectRoot, std::string& error) const;
    static std::vector<std::string> requiredFields(Store store);
    static std::string storeName(Store store);
    static std::string artifactExtension(Store store);
};

} // namespace forge::publish

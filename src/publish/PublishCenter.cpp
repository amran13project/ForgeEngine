#include "publish/PublishCenter.h"
#include <fstream>

namespace forge::publish {

namespace {
std::string jsonEscape(const std::string& value) {
    std::string out; out.reserve(value.size());
    for (char c : value) {
        if (c == '\\') out += "\\\\";
        else if (c == '"') out += "\\\"";
        else if (c == '\n') out += "\\n";
        else out += c;
    }
    return out;
}
}

std::string PublishCenter::storeName(Store s) {
    switch (s) {
        case Store::GooglePlay: return "Google Play";
        case Store::AppleAppStore: return "Apple App Store";
        case Store::MicrosoftStore: return "Microsoft Store";
        case Store::Steam: return "Steam";
        default: return "Direct Distribution";
    }
}

std::string PublishCenter::artifactExtension(Store s) {
    switch (s) {
        case Store::GooglePlay: return ".aab";
        case Store::AppleAppStore: return ".ipa";
        case Store::MicrosoftStore: return ".msix";
        case Store::Steam: return ".zip";
        default: return ".zip";
    }
}

std::vector<std::string> PublishCenter::requiredFields(Store store) {
    std::vector<std::string> fields{"App Name", "Publisher", "Description", "Version", "Build Number"};
    if (store != Store::Direct) fields.push_back("Package Identifier");
    if (store == Store::GooglePlay || store == Store::AppleAppStore || store == Store::MicrosoftStore) fields.push_back("Privacy Policy URL");
    return fields;
}

PublishPlan PublishCenter::validate(const StoreProfile& p, const std::filesystem::path& root) const {
    PublishPlan plan; plan.profile = p; plan.artifactDirectory = root / "Builds" / "Release";
    auto require = [&](const char* field, const std::string& value) {
        if (value.empty()) plan.issues.push_back({true, field, "Required value is missing"});
    };
    require("App Name", p.appName); require("Publisher", p.publisher); require("Description", p.description);
    require("Version", p.version); require("Build Number", p.buildNumber);
    if (p.store != Store::Direct) require("Package Identifier", p.packageId);
    if (p.store == Store::GooglePlay || p.store == Store::AppleAppStore || p.store == Store::MicrosoftStore) require("Privacy Policy URL", p.privacyUrl);
    if (p.description.size() > 4000) plan.issues.push_back({true, "Description", "Description exceeds Forge's safe metadata limit"});
    if (!std::filesystem::exists(root / "Assets")) plan.issues.push_back({true, "Assets", "Project Assets directory is missing"});
    if (p.version.find('.') == std::string::npos) plan.issues.push_back({false, "Version", "Use semantic-style versioning such as 1.0.0"});
    return plan;
}

bool PublishCenter::writeSubmissionManifest(const StoreProfile& p, const std::filesystem::path& root, std::string& error) const {
    try {
        const auto dir = root / "Publishing" / storeName(p.store);
        std::filesystem::create_directories(dir);
        std::ofstream out(dir / "SubmissionManifest.json", std::ios::trunc);
        if (!out) { error = "Unable to write publishing manifest"; return false; }
        out << "{\n"
            << "  \"platform\": \"" << jsonEscape(storeName(p.store)) << "\",\n"
            << "  \"appName\": \"" << jsonEscape(p.appName) << "\",\n"
            << "  \"publisher\": \"" << jsonEscape(p.publisher) << "\",\n"
            << "  \"description\": \"" << jsonEscape(p.description) << "\",\n"
            << "  \"packageId\": \"" << jsonEscape(p.packageId) << "\",\n"
            << "  \"version\": \"" << jsonEscape(p.version) << "\",\n"
            << "  \"buildNumber\": \"" << jsonEscape(p.buildNumber) << "\",\n"
            << "  \"category\": \"" << jsonEscape(p.category) << "\",\n"
            << "  \"language\": \"" << jsonEscape(p.language) << "\",\n"
            << "  \"privacyUrl\": \"" << jsonEscape(p.privacyUrl) << "\",\n"
            << "  \"artifactExtension\": \"" << artifactExtension(p.store) << "\",\n"
            << "  \"submissionAction\": \"manual-or-provider-api\"\n"
            << "}\n";
        return true;
    } catch (const std::exception& e) { error = e.what(); return false; }
}

} // namespace forge::publish

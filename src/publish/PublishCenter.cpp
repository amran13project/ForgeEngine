#include "publish/PublishCenter.h"
#include <fstream>
namespace forge::publish {
std::string PublishCenter::storeName(Store s){switch(s){case Store::GooglePlay:return"Google Play";case Store::AppleAppStore:return"Apple App Store";case Store::MicrosoftStore:return"Microsoft Store";case Store::Steam:return"Steam";default:return"Direct Distribution";}}
PublishPlan PublishCenter::validate(const StoreProfile& p, const std::filesystem::path& root) const {
    PublishPlan plan; plan.profile=p; plan.artifactDirectory=root/"Builds"/"Release";
    auto require=[&](const char* f,const std::string& v){ if(v.empty()) plan.issues.push_back({true,f,"Required value is missing"}); };
    require("App Name",p.appName); require("Publisher",p.publisher); require("Description",p.description); require("Version",p.version); require("Build Number",p.buildNumber);
    if (p.store != Store::Direct) require("Package Identifier",p.packageId);
    if (p.privacyUrl.empty()) plan.issues.push_back({false,"Privacy Policy URL","Add this before store submission when the target platform requires it"});
    if (p.description.size() > 4000) plan.issues.push_back({true,"Description","Description is unusually long for store metadata"});
    if (!std::filesystem::exists(root / "Assets")) plan.issues.push_back({true,"Assets","Project Assets directory is missing"});
    return plan;
}
} // namespace forge::publish

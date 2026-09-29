#include "release/ReleaseManager.h"
#include <fstream>
namespace forge::release { bool ReleaseManager::writeManifest(const std::filesystem::path& root,const ReleaseRecord& r,std::string& error) const { try{std::filesystem::create_directories(root/"Releases"); std::ofstream f(root/"Releases"/"ReleaseManifest.json"); if(!f){error="Unable to write release manifest";return false;} f<<"{\n  \"version\": \""<<r.version<<"\",\n  \"buildNumber\": \""<<r.buildNumber<<"\",\n  \"channel\": \""<<r.channel<<"\"\n}\n"; return true;}catch(...){error="Unable to create Releases directory";return false;} } }

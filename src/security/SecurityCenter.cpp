#include "security/SecurityCenter.h"
#include <fstream>
namespace forge::security { SecurityReport SecurityCenter::scan(const std::filesystem::path& root) const { SecurityReport r; r.message="No credential files are intentionally managed by Forge AI. Store signing secrets outside source control."; if(std::filesystem::exists(root/".env")){r.secretsDetected=true;r.gitSafe=false;r.message="Potential secret file detected: .env";} return r; } }

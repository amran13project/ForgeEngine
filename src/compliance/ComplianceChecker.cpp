#include "compliance/ComplianceChecker.h"
namespace forge::compliance {
Result ComplianceChecker::scan(const publish::StoreProfile& p, const std::filesystem::path& root) const { Result r; auto plan=publish::PublishCenter().validate(p,root); r.issues=plan.issues; return r; }
}

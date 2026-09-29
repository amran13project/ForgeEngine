#pragma once
#include "publish/PublishCenter.h"
#include <filesystem>
#include <string>
#include <vector>
namespace forge::compliance {
struct Result { std::vector<publish::ValidationIssue> issues; bool ready() const { for (const auto& i: issues) if (i.error) return false; return true; } };
class ComplianceChecker { public: Result scan(const publish::StoreProfile&, const std::filesystem::path&) const; };
}

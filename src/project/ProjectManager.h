#pragma once
#include <filesystem>
#include <string>
#include <vector>

namespace forge::project {
struct ProjectInfo {
    std::string id,name,engineVersion,projectVersion,defaultScene,templateName;
    std::filesystem::path root;
};
class ProjectManager {
public:
    static std::filesystem::path defaultProjectsDirectory();
    static std::string sanitizeName(const std::string& input);
    bool createProject(const std::string& rawName,const std::filesystem::path& requestedRoot,const std::string& templateName,ProjectInfo& out,std::string& error) const;
    bool openProject(const std::filesystem::path& root,ProjectInfo& out,std::string& error) const;
    bool saveProject(const ProjectInfo& p,std::string& error) const;
    std::vector<ProjectInfo> discoverProjects(const std::filesystem::path& base) const;
};
}

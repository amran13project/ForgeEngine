#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
namespace forge::assets {
enum class AssetType { Unknown, Model, Texture, Material, Animation, Audio, Scene, Script, UI, VFX, Data };
struct AssetRecord { std::string id; std::filesystem::path path; AssetType type{AssetType::Unknown}; uintmax_t size{0}; std::filesystem::file_time_type modified{}; bool cached{false}; };
class AssetDatabase {
public:
    bool scan(const std::filesystem::path&root,std::string&error);
    const std::vector<AssetRecord>& assets() const { return assets_; }
    size_t count(AssetType type) const;
    static AssetType classify(const std::filesystem::path&p);
    static const char* typeName(AssetType t);
private:
    std::vector<AssetRecord> assets_;
};
}

#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace forge::core {
enum class SystemState { NotLoaded, Loading, Initializing, Ready, Disabled, Warning, Error, ShuttingDown };
struct SystemInfo {
    std::string name;
    SystemState state{SystemState::NotLoaded};
    std::vector<std::string> dependencies;
    double initMs{0.0};
    std::string message;
};
class ForgeSystem {
public:
    ForgeSystem();
    bool initialize();
    void shutdown();
    bool isReady(const std::string& name) const;
    const std::vector<SystemInfo>& systems() const { return systems_; }
    const std::string& lastError() const { return lastError_; }
    void tick(double dt);
private:
    bool startSystem(SystemInfo& info);
    std::vector<SystemInfo> systems_;
    bool initialized_{false};
    std::string lastError_;
    uint64_t tickCount_{0};
};
const char* toString(SystemState state);
}

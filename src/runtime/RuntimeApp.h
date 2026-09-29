#pragma once
#include "platform/NativeWindow.h"
#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include "renderer/SoftwareRenderer.h"
#include <string>
#include <array>
#include <chrono>

namespace forge::runtime {
class RuntimeApp {
public:
    RuntimeApp(platform::NativeWindow& window, const project::ProjectInfo& project);
    bool initialize(std::string& error);
    void run();
private:
    void paint(int width, int height);
    void onEvent(const platform::InputEvent& event);
    bool isPressed(platform::Key key) const;
    void tick(double dt);
    platform::NativeWindow& window_;
    project::ProjectInfo project_{};
    scene::Scene scene_{};
    renderer::SoftwareRenderer renderer_{};
    std::array<bool, 32> keys_{};
    double runtimeTime_{0.0};
    std::chrono::steady_clock::time_point lastTick_{};
};
}

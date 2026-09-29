#pragma once
#include "platform/NativeWindow.h"
#include "project/ProjectManager.h"
#include "scene/Scene.h"
#include "renderer/SoftwareRenderer.h"
#include <string>

namespace forge::runtime {
class RuntimeApp {
public:
    RuntimeApp(platform::NativeWindow& window, const project::ProjectInfo& project);
    bool initialize(std::string& error);
    void run();
private:
    void paint(int width, int height);
    void onEvent(const platform::InputEvent& event);
    platform::NativeWindow& window_;
    project::ProjectInfo project_{};
    scene::Scene scene_{};
    renderer::SoftwareRenderer renderer_{};
};
}

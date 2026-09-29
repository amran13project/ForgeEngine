#include "core/ForgeSystem.h"
#include "platform/NativeWindow.h"
#include "project/ProjectManager.h"
#include "runtime/RuntimeApp.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "ForgeRuntime: usage: ForgeRuntime <project-directory>\n";
        return 64;
    }
    forge::project::ProjectManager pm;
    forge::project::ProjectInfo project;
    std::string error;
    if (!pm.openProject(argv[1], project, error)) {
        std::cerr << "ForgeRuntime: " << error << "\n";
        return 2;
    }
    forge::core::ForgeSystem system;
    if (!system.initialize()) {
        std::cerr << "ForgeRuntime: engine initialization failed: " << system.lastError() << "\n";
        return 3;
    }
    forge::platform::NativeWindow window("Forge Runtime - " + project.name, 1280, 720);
    if (!window.create()) {
        std::cerr << "ForgeRuntime: unable to create native window\n";
        system.shutdown();
        return 4;
    }
    forge::runtime::RuntimeApp app(window, project);
    if (!app.initialize(error)) {
        std::cerr << "ForgeRuntime: " << error << "\n";
        system.shutdown();
        return 5;
    }
    app.run();
    system.shutdown();
    return 0;
}

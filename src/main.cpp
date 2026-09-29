#include "core/ForgeSystem.h"
#include "editor/ForgeEditor.h"
#include <iostream>

int main() {
    std::cout << "Forge Engine Professional 3.1.0 - CREATE WITHOUT LIMITS\n";
    forge::core::ForgeSystem system;
    if (!system.initialize()) {
        std::cerr << "Forge Engine initialization failed: " << system.lastError() << "\n";
        return 2;
    }
    forge::platform::NativeWindow window("Forge Engine Professional", 1440, 900);
    if (!window.create()) {
        std::cerr << "Native window creation failed\n";
        system.shutdown();
        return 3;
    }
    forge::editor::ForgeEditor editor(window, system);
    editor.run();
    system.shutdown();
    return 0;
}

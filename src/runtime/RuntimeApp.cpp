#include "runtime/RuntimeApp.h"
#include <algorithm>

namespace forge::runtime {
static uint32_t rgb(uint32_t value) { return value & 0x00FFFFFFu; }

RuntimeApp::RuntimeApp(platform::NativeWindow& window, const project::ProjectInfo& project)
    : window_(window), project_(project) {
    window_.setEventHandler([this](const platform::InputEvent& e){ onEvent(e); });
    window_.setPaintHandler([this](int w, int h){ paint(w, h); });
}

bool RuntimeApp::initialize(std::string& error) {
    if (!scene_.load(project_.root / project_.defaultScene, error)) return false;
    renderer_.setCamera({{0.0f, 2.0f, 8.0f}, 0.0f, 0.0f, 60.0f});
    return true;
}

void RuntimeApp::run() { window_.run(); }

void RuntimeApp::onEvent(const platform::InputEvent& event) {
    if (event.type == platform::InputEvent::Type::Close) window_.requestClose();
}

void RuntimeApp::paint(int width, int height) {
    const int left = 24;
    const int top = 52;
    const int right = std::max(left + 100, width - 24);
    const int bottom = std::max(top + 100, height - 28);
    window_.drawRect(0, 0, width, height, rgb(0x0C1015), true);
    window_.drawRect(0, 0, width, 52, rgb(0x151C25), true);
    window_.drawText(20, 32, "FORGE RUNTIME", rgb(0xF3F6F9), 18);
    window_.drawText(std::max(220, width - 360), 32, project_.name + "  |  Development", rgb(0x91A0B1));
    window_.drawRect(left, top, right - left, bottom - top, rgb(0x10161E), true);
    window_.drawRect(left, top, right - left, bottom - top, rgb(0x39434F), false);

    const int cx = width / 2;
    const int cy = (top + bottom) / 2 + 10;
    for (int i = -12; i <= 12; ++i) {
        window_.drawLine(cx + i * 32, top + 30, cx + i * 32, bottom - 30, rgb(0x171E27));
        window_.drawLine(left + 30, cy + i * 24, right - 30, cy + i * 24, rgb(0x171E27));
    }

    static constexpr int edges[12][2] = {
        {0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}
    };
    for (const auto& e : scene_.entities()) {
        if (e.type != "Mesh") continue;
        const auto vertices = renderer_.cube(e.position, e.scale, cx, cy);
        for (const auto& edge : edges) {
            const auto a = vertices[edge[0]];
            const auto b = vertices[edge[1]];
            window_.drawLine(a.x, a.y, b.x, b.y, rgb(0xD6DEE8), 2);
        }
    }
    window_.drawRect(0, height - 52, width, 52, rgb(0x11171E), true);
    window_.drawText(20, height - 21, "Runtime session active", rgb(0x7F8D9E));
}
}

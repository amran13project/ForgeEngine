#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <algorithm>
#include <utility>

namespace forge::platform {

enum class Key {
    Unknown, Escape, Enter, Backspace, Tab, Left, Right, Up, Down, Space,
    A, D, W, S, P, N, O, R, F5, F6, F7, F8, F9, F10, DeleteKey
};

enum class MouseButton { Left, Right, Middle };

struct InputEvent {
    enum class Type { None, KeyDown, KeyChar, MouseDown, MouseUp, MouseMove, Close, Resize } type{Type::None};
    Key key{Key::Unknown};
    char32_t character{0};
    MouseButton mouseButton{MouseButton::Left};
    int x{0};
    int y{0};
    int width{0};
    int height{0};
};

class NativeWindow {
public:
    using EventHandler = std::function<void(const InputEvent&)>;
    using PaintHandler = std::function<void(int width, int height)>;

    NativeWindow(const std::string& title, int width, int height);
    ~NativeWindow();

    bool create();
    void run();
    void requestClose();
    void invalidate();
    void setEventHandler(EventHandler handler) { eventHandler_ = std::move(handler); }
    void setPaintHandler(PaintHandler handler) { paintHandler_ = std::move(handler); }
    int width() const { return width_; }
    int height() const { return height_; }

    void drawRect(int x, int y, int w, int h, uint32_t rgb, bool filled = true);
    void drawLine(int x1, int y1, int x2, int y2, uint32_t rgb, int thickness = 1);
    void drawText(int x, int y, const std::string& text, uint32_t rgb, int size = 14);
    void dispatchEvent(const InputEvent& event) { if (eventHandler_) eventHandler_(event); }
    void dispatchPaint() { if (paintHandler_) paintHandler_(width_, height_); }
#ifdef _WIN32
    void repaint();
#endif
    void setWindowSize(int width, int height) { width_ = std::max(1, width); height_ = std::max(1, height); }

private:
#ifdef _WIN32
    bool ensureBackBuffer();
    void paintFrame();
#endif

    struct Impl;
    Impl* impl_{};
    std::string title_;
    int width_{};
    int height_{};
    EventHandler eventHandler_;
    PaintHandler paintHandler_;
    bool closing_{false};
};

} // namespace forge::platform

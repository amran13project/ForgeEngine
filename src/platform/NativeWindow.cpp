#include "platform/NativeWindow.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <map>
#include <thread>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <windowsx.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <unistd.h>
#endif

namespace forge::platform {

struct NativeWindow::Impl {
#ifdef _WIN32
    HWND hwnd{};
    HDC backDc{};
    HBITMAP backBitmap{};
    HBITMAP oldBitmap{};
    int backWidth{};
    int backHeight{};
    std::map<int, HFONT> fonts;
#else
    Display* display{};
    Window window{};
    GC gc{};
    Atom wmDelete{};
    XFontStruct* font{};
#endif
};

static uint32_t clampRgb(uint32_t rgb) { return rgb & 0x00FFFFFFu; }

#ifdef _WIN32
static COLORREF toColor(uint32_t rgb) { return RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255); }

static Key mapVk(WPARAM vk) {
    switch (vk) {
        case VK_ESCAPE: return Key::Escape; case VK_RETURN: return Key::Enter;
        case VK_BACK: return Key::Backspace; case VK_TAB: return Key::Tab;
        case VK_LEFT: return Key::Left; case VK_RIGHT: return Key::Right;
        case VK_UP: return Key::Up; case VK_DOWN: return Key::Down; case VK_SPACE: return Key::Space;
        case 'A': return Key::A; case 'D': return Key::D; case 'W': return Key::W; case 'S': return Key::S;
        case 'P': return Key::P; case 'N': return Key::N; case 'O': return Key::O; case 'R': return Key::R;
        case VK_DELETE: return Key::DeleteKey; case VK_F5: return Key::F5; case VK_F6: return Key::F6;
        case VK_F7: return Key::F7; case VK_F8: return Key::F8; case VK_F9: return Key::F9; case VK_F10: return Key::F10;
        default: return Key::Unknown;
    }
}

static NativeWindow* getThis(HWND h) { return reinterpret_cast<NativeWindow*>(GetWindowLongPtrW(h, GWLP_USERDATA)); }

bool NativeWindow::ensureBackBuffer() {
    auto* impl = impl_;
    HDC windowDc = GetDC(impl->hwnd);
    if (!windowDc) return false;
    if (!impl->backDc) impl->backDc = CreateCompatibleDC(windowDc);
    if (!impl->backDc) { ReleaseDC(impl->hwnd, windowDc); return false; }
    if (impl->backBitmap && impl->backWidth == width_ && impl->backHeight == height_) {
        ReleaseDC(impl->hwnd, windowDc);
        return true;
    }
    if (impl->backBitmap) {
        SelectObject(impl->backDc, impl->oldBitmap);
        DeleteObject(impl->backBitmap);
        impl->backBitmap = nullptr;
        impl->oldBitmap = nullptr;
    }
    impl->backBitmap = CreateCompatibleBitmap(windowDc, width_, height_);
    if (!impl->backBitmap) { ReleaseDC(impl->hwnd, windowDc); return false; }
    impl->oldBitmap = static_cast<HBITMAP>(SelectObject(impl->backDc, impl->backBitmap));
    impl->backWidth = width_;
    impl->backHeight = height_;
    ReleaseDC(impl->hwnd, windowDc);
    return true;
}

template <typename ImplT>
static HFONT fontFor(ImplT* impl, HDC dc, int size) {
    const int clamped = std::clamp(size, 9, 36);
    auto it = impl->fonts.find(clamped);
    if (it != impl->fonts.end()) return it->second;
    const int dpi = std::max(96, GetDeviceCaps(dc, LOGPIXELSY));
    const int height = -MulDiv(clamped, dpi, 72);
    HFONT f = CreateFontW(height, 0, 0, 0, clamped >= 18 ? FW_SEMIBOLD : FW_NORMAL,
                           FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                           CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                           L"Segoe UI");
    impl->fonts.emplace(clamped, f);
    return f;
}

void NativeWindow::repaint() {
    paintFrame();
}

void NativeWindow::paintFrame() {
    PAINTSTRUCT ps{};
    HDC paintDc = BeginPaint(impl_->hwnd, &ps);
    if (!paintDc) return;
    if (ensureBackBuffer()) {
        RECT r{0, 0, width_, height_};
        HBRUSH bg = CreateSolidBrush(RGB(12, 15, 19));
        FillRect(impl_->backDc, &r, bg);
        DeleteObject(bg);
        dispatchPaint();
        BitBlt(paintDc, 0, 0, width_, height_, impl_->backDc, 0, 0, SRCCOPY);
    }
    EndPaint(impl_->hwnd, &ps);
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto* self = getThis(hwnd);
    if (!self) return DefWindowProcW(hwnd, msg, wParam, lParam);
    auto emit = [&](InputEvent e) { self->dispatchEvent(e); };
    switch (msg) {
        case WM_ERASEBKGND: return 1;
        case WM_CLOSE: { InputEvent e; e.type = InputEvent::Type::Close; emit(e); return 0; }
        case WM_SIZE: {
            const int w = static_cast<int>(LOWORD(lParam));
            const int h = static_cast<int>(HIWORD(lParam));
            self->setWindowSize(w, h);
            InputEvent e; e.type = InputEvent::Type::Resize; e.width = w; e.height = h; emit(e);
            InvalidateRect(hwnd, nullptr, FALSE); return 0;
        }
        case WM_LBUTTONDOWN: { InputEvent e; e.type = InputEvent::Type::MouseDown; e.mouseButton = MouseButton::Left; e.x = GET_X_LPARAM(lParam); e.y = GET_Y_LPARAM(lParam); emit(e); SetCapture(hwnd); return 0; }
        case WM_LBUTTONUP: { InputEvent e; e.type = InputEvent::Type::MouseUp; e.mouseButton = MouseButton::Left; e.x = GET_X_LPARAM(lParam); e.y = GET_Y_LPARAM(lParam); emit(e); ReleaseCapture(); return 0; }
        case WM_MOUSEMOVE: { InputEvent e; e.type = InputEvent::Type::MouseMove; e.x = GET_X_LPARAM(lParam); e.y = GET_Y_LPARAM(lParam); emit(e); return 0; }
        case WM_KEYDOWN: { InputEvent e; e.type = InputEvent::Type::KeyDown; e.key = mapVk(wParam); emit(e); return 0; }
        case WM_CHAR: { InputEvent e; e.type = InputEvent::Type::KeyChar; e.character = static_cast<char32_t>(wParam); emit(e); return 0; }
        case WM_PAINT: self->repaint(); return 0;
        default: return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}
#else
static Key mapKeySym(KeySym k) {
    switch (k) {
        case XK_Escape: return Key::Escape; case XK_Return: return Key::Enter; case XK_BackSpace: return Key::Backspace;
        case XK_Tab: return Key::Tab; case XK_Left: return Key::Left; case XK_Right: return Key::Right;
        case XK_Up: return Key::Up; case XK_Down: return Key::Down; case XK_space: return Key::Space;
        case XK_a: case XK_A: return Key::A; case XK_d: case XK_D: return Key::D; case XK_w: case XK_W: return Key::W;
        case XK_s: case XK_S: return Key::S; case XK_p: case XK_P: return Key::P; case XK_n: case XK_N: return Key::N;
        case XK_o: case XK_O: return Key::O; case XK_r: case XK_R: return Key::R; case XK_F5: return Key::F5;
        case XK_F6: return Key::F6; case XK_F7: return Key::F7; case XK_F8: return Key::F8; case XK_F9: return Key::F9;
        case XK_F10: return Key::F10; case XK_Delete: return Key::DeleteKey; default: return Key::Unknown;
    }
}
#endif

NativeWindow::NativeWindow(const std::string& title, int width, int height)
    : impl_(new Impl()), title_(title), width_(width), height_(height) {}

NativeWindow::~NativeWindow() {
#ifdef _WIN32
    if (impl_->backDc) {
        SelectObject(impl_->backDc, impl_->oldBitmap);
        if (impl_->backBitmap) DeleteObject(impl_->backBitmap);
        DeleteDC(impl_->backDc);
    }
    for (auto& [_, font] : impl_->fonts) if (font) DeleteObject(font);
#else
    if (impl_->font) XFreeFont(impl_->display, impl_->font);
    if (impl_->gc) XFreeGC(impl_->display, impl_->gc);
    if (impl_->display) XCloseDisplay(impl_->display);
#endif
    delete impl_;
}

bool NativeWindow::create() {
#ifdef _WIN32
    SetProcessDPIAware();
    HINSTANCE hInst = GetModuleHandleW(nullptr);
    const wchar_t* cls = L"ForgeEngineNativeWindowV2";
    static bool registered = false;
    if (!registered) {
        WNDCLASSW wc{};
        wc.lpfnWndProc = wndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = cls;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.style = CS_HREDRAW | CS_VREDRAW;
        if (!RegisterClassW(&wc)) return false;
        registered = true;
    }
    std::wstring wtitle(title_.begin(), title_.end());
    impl_->hwnd = CreateWindowExW(0, cls, wtitle.c_str(), WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, width_, height_, nullptr, nullptr, hInst, nullptr);
    if (!impl_->hwnd) return false;
    SetWindowLongPtrW(impl_->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    ShowWindow(impl_->hwnd, SW_SHOW);
    ensureBackBuffer();
    UpdateWindow(impl_->hwnd);
    return true;
#else
    impl_->display = XOpenDisplay(nullptr); if (!impl_->display) return false;
    int screen = DefaultScreen(impl_->display);
    unsigned long bg = WhitePixel(impl_->display, screen); unsigned long fg = BlackPixel(impl_->display, screen);
    impl_->window = XCreateSimpleWindow(impl_->display, RootWindow(impl_->display, screen), 40, 40, width_, height_, 1, fg, bg);
    XStoreName(impl_->display, impl_->window, title_.c_str());
    XSelectInput(impl_->display, impl_->window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | StructureNotifyMask);
    impl_->wmDelete = XInternAtom(impl_->display, "WM_DELETE_WINDOW", False); XSetWMProtocols(impl_->display, impl_->window, &impl_->wmDelete, 1);
    impl_->gc = XCreateGC(impl_->display, impl_->window, 0, nullptr); impl_->font = XLoadQueryFont(impl_->display, "fixed");
    XMapWindow(impl_->display, impl_->window); XFlush(impl_->display); return true;
#endif
}

void NativeWindow::run() {
#ifdef _WIN32
    MSG msg{};
    while (!closing_) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) { closing_ = true; break; }
            TranslateMessage(&msg); DispatchMessageW(&msg);
        }
        if (!closing_) InvalidateRect(impl_->hwnd, nullptr, FALSE);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
#else
    while (!closing_) {
        while (XPending(impl_->display)) {
            XEvent ev{}; XNextEvent(impl_->display, &ev);
            if (ev.type == Expose) { if (paintHandler_) paintHandler_(width_, height_); }
            else if (ev.type == ClientMessage && static_cast<Atom>(ev.xclient.data.l[0]) == impl_->wmDelete) { InputEvent e; e.type = InputEvent::Type::Close; dispatchEvent(e); }
            else if (ev.type == ConfigureNotify) { width_ = ev.xconfigure.width; height_ = ev.xconfigure.height; InputEvent e; e.type = InputEvent::Type::Resize; e.width = width_; e.height = height_; dispatchEvent(e); }
            else if (ev.type == MotionNotify) { InputEvent e; e.type = InputEvent::Type::MouseMove; e.x = ev.xmotion.x; e.y = ev.xmotion.y; dispatchEvent(e); }
            else if (ev.type == ButtonPress || ev.type == ButtonRelease) { InputEvent e; e.type = (ev.type == ButtonPress) ? InputEvent::Type::MouseDown : InputEvent::Type::MouseUp; e.mouseButton = ev.xbutton.button == Button3 ? MouseButton::Right : (ev.xbutton.button == Button2 ? MouseButton::Middle : MouseButton::Left); e.x = ev.xbutton.x; e.y = ev.xbutton.y; dispatchEvent(e); }
            else if (ev.type == KeyPress) { KeySym ks = XLookupKeysym(&ev.xkey, 0); InputEvent e; e.type = InputEvent::Type::KeyDown; e.key = mapKeySym(ks); dispatchEvent(e); }
        }
        if (paintHandler_) paintHandler_(width_, height_);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
#endif
}

void NativeWindow::requestClose() {
    closing_ = true;
#ifdef _WIN32
    PostQuitMessage(0);
#endif
}

void NativeWindow::invalidate() {
#ifdef _WIN32
    if (impl_->hwnd) InvalidateRect(impl_->hwnd, nullptr, FALSE);
#else
    XClearArea(impl_->display, impl_->window, 0, 0, 0, 0, True); XFlush(impl_->display);
#endif
}

void NativeWindow::drawRect(int x, int y, int w, int h, uint32_t rgb, bool filled) {
#ifdef _WIN32
    HDC dc = impl_->backDc; if (!dc) return;
    const COLORREF c = toColor(clampRgb(rgb));
    HBRUSH br = CreateSolidBrush(c);
    if (filled) {
        RECT r{x, y, x + w, y + h}; FillRect(dc, &r, br);
    } else {
        HPEN pen = CreatePen(PS_SOLID, 1, c);
        auto oldP = static_cast<HPEN>(SelectObject(dc, pen)); auto oldB = static_cast<HBRUSH>(SelectObject(dc, GetStockObject(NULL_BRUSH)));
        Rectangle(dc, x, y, x + w, y + h);
        SelectObject(dc, oldB); SelectObject(dc, oldP); DeleteObject(pen);
    }
    DeleteObject(br);
#else
    XSetForeground(impl_->display, impl_->gc, clampRgb(rgb)); if (filled) XFillRectangle(impl_->display, impl_->window, impl_->gc, x, y, w, h); else XDrawRectangle(impl_->display, impl_->window, impl_->gc, x, y, w, h);
#endif
}

void NativeWindow::drawLine(int x1, int y1, int x2, int y2, uint32_t rgb, int thickness) {
#ifdef _WIN32
    HDC dc = impl_->backDc; if (!dc) return; HPEN pen = CreatePen(PS_SOLID, std::max(1, thickness), toColor(clampRgb(rgb))); auto old = static_cast<HPEN>(SelectObject(dc, pen)); MoveToEx(dc, x1, y1, nullptr); LineTo(dc, x2, y2); SelectObject(dc, old); DeleteObject(pen);
#else
    XSetForeground(impl_->display, impl_->gc, clampRgb(rgb)); XSetLineAttributes(impl_->display, impl_->gc, std::max(1, thickness), LineSolid, CapButt, JoinMiter); XDrawLine(impl_->display, impl_->window, impl_->gc, x1, y1, x2, y2); XFlush(impl_->display);
#endif
}

void NativeWindow::drawText(int x, int y, const std::string& text, uint32_t rgb, int size) {
#ifdef _WIN32
    HDC dc = impl_->backDc; if (!dc) return; SetBkMode(dc, TRANSPARENT); SetTextColor(dc, toColor(clampRgb(rgb))); auto font = fontFor(impl_, dc, size); auto old = static_cast<HFONT>(SelectObject(dc, font)); std::wstring w(text.begin(), text.end()); TextOutW(dc, x, y, w.c_str(), static_cast<int>(w.size())); SelectObject(dc, old);
#else
    (void)size; XSetForeground(impl_->display, impl_->gc, clampRgb(rgb)); if (impl_->font) XSetFont(impl_->display, impl_->gc, impl_->font->fid); XDrawString(impl_->display, impl_->window, impl_->gc, x, y, text.c_str(), static_cast<int>(text.size())); XFlush(impl_->display);
#endif
}

} // namespace forge::platform

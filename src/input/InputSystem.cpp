#include "input/InputSystem.h"

namespace forge::input {
void InputSystem::bind(const std::string& action, int primary, int secondary) { bindings_[action] = {primary, secondary}; }
void InputSystem::press(int key) { keys_.insert(key); }
void InputSystem::release(int key) { keys_.erase(key); }
bool InputSystem::pressed(const std::string& action) const {
    auto it = bindings_.find(action); if (it == bindings_.end()) return false;
    return pressedKey(it->second.primary) || pressedKey(it->second.secondary);
}
bool InputSystem::pressedKey(int key) const { return key >= 0 && keys_.find(key) != keys_.end(); }
void InputSystem::setAxis(const std::string& axis, float value) { axes_[axis] = value; }
float InputSystem::axis(const std::string& axis) const { auto it = axes_.find(axis); return it == axes_.end() ? 0.0f : it->second; }
void InputSystem::clearFrame() { axes_.clear(); }
}

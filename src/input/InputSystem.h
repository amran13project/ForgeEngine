#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace forge::input {

struct ActionBinding { int primary{-1}; int secondary{-1}; };

class InputSystem {
public:
    void bind(const std::string& action, int primary, int secondary = -1);
    void press(int key);
    void release(int key);
    bool pressed(const std::string& action) const;
    bool pressedKey(int key) const;
    void setAxis(const std::string& axis, float value);
    float axis(const std::string& axis) const;
    void clearFrame();
private:
    std::unordered_map<std::string, ActionBinding> bindings_;
    std::unordered_map<std::string, float> axes_;
    std::unordered_set<int> keys_;
};

} // namespace forge::input

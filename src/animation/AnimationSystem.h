#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace forge::animation {
struct Clip { std::string name; float duration{1.0f}; bool loop{true}; };
struct State { std::string clip; float speed{1.0f}; std::string next; };
class Animator {
public:
    void addClip(const Clip& clip);
    void addState(const std::string& name, const State& state);
    bool play(const std::string& state);
    void update(float dt);
    const std::string& state()const{return state_;}
    float time()const{return time_;}
    bool playing()const{return playing_;}
private:
    std::unordered_map<std::string,Clip> clips_;
    std::unordered_map<std::string,State> states_;
    std::string state_;
    float time_{0}; bool playing_{false};
};
}

#include "core/ForgeSystem.h"
#include <chrono>
#include <unordered_set>

namespace forge::core {
const char* toString(SystemState state){
    switch(state){case SystemState::NotLoaded:return "NOT LOADED";case SystemState::Loading:return "LOADING";case SystemState::Initializing:return "INITIALIZING";case SystemState::Ready:return "READY";case SystemState::Disabled:return "DISABLED";case SystemState::Warning:return "WARNING";default:return "ERROR";}
}
ForgeSystem::ForgeSystem(){
    systems_={
        {"Core",SystemState::NotLoaded,{},0,{}},
        {"Memory",SystemState::NotLoaded,{"Core"},0,{}},
        {"Job System",SystemState::NotLoaded,{"Core","Memory"},0,{}},
        {"File System",SystemState::NotLoaded,{"Core"},0,{}},
        {"Project",SystemState::NotLoaded,{"File System"},0,{}},
        {"Asset Database",SystemState::NotLoaded,{"Project","File System"},0,{}},
        {"Scene",SystemState::NotLoaded,{"Project","Asset Database"},0,{}},
        {"Renderer",SystemState::NotLoaded,{"Core","Job System"},0,{}},
        {"2D",SystemState::NotLoaded,{"Renderer","Scene"},0,{}},
        {"3D",SystemState::NotLoaded,{"Renderer","Scene"},0,{}},
        {"Physics",SystemState::NotLoaded,{"Scene"},0,{}},
        {"Animation",SystemState::NotLoaded,{"Scene"},0,{}},
        {"Audio",SystemState::NotLoaded,{"Core"},0,{}},
        {"UI",SystemState::NotLoaded,{"Renderer"},0,{}},
        {"Input",SystemState::NotLoaded,{"Core"},0,{}},
        {"Scripting",SystemState::NotLoaded,{"Core","Project"},0,{}},
        {"AI",SystemState::NotLoaded,{"Scene","Job System"},0,{}},
        {"World",SystemState::NotLoaded,{"Scene","Renderer"},0,{}},
        {"Networking",SystemState::Disabled,{"Core"},0,"Optional until enabled"},
        {"Profiler",SystemState::NotLoaded,{"Core"},0,{}},
        {"Build",SystemState::NotLoaded,{"Project","Asset Database"},0,{}},
        {"Testing",SystemState::NotLoaded,{"Project"},0,{}},
        {"Recovery",SystemState::NotLoaded,{"Project","File System"},0,{}},
        {"Plugins",SystemState::NotLoaded,{"Project","File System"},0,{}}
    };
}
bool ForgeSystem::startSystem(SystemInfo& info){
    for(const auto& dep:info.dependencies){ if(!isReady(dep)){ info.state=SystemState::Error; info.message="Dependency not ready: "+dep; lastError_=info.message; return false; } }
    const auto t=std::chrono::steady_clock::now();
    info.state=SystemState::Loading;
    info.state=SystemState::Initializing;
    info.state=SystemState::Ready;
    info.initMs=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t).count();
    return true;
}
bool ForgeSystem::isReady(const std::string& name) const { for(const auto&s:systems_) if(s.name==name) return s.state==SystemState::Ready; return false; }
bool ForgeSystem::initialize(){
    if(initialized_) return true;
    lastError_.clear();
    for(auto& s:systems_) if(s.state!=SystemState::Disabled && !startSystem(s)) return false;
    initialized_=true; return true;
}
void ForgeSystem::shutdown(){
    if(!initialized_) return;
    for(auto it=systems_.rbegin();it!=systems_.rend();++it) if(it->state==SystemState::Ready) it->state=SystemState::ShuttingDown;
    for(auto& s:systems_) if(s.state==SystemState::ShuttingDown) s.state=SystemState::NotLoaded;
    initialized_=false;
}
void ForgeSystem::tick(double){ ++tickCount_; }
}

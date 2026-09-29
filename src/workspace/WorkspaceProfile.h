#pragma once
#include <string>
namespace forge::workspace { enum class Experience{Beginner,Creator,Developer,Professional}; class WorkspaceProfile { public: void set(Experience e){experience_=e;} Experience experience() const{return experience_;} std::string name()const{switch(experience_){case Experience::Beginner:return"Beginner";case Experience::Creator:return"Creator";case Experience::Developer:return"Developer";default:return"Professional";}} private: Experience experience_{Experience::Creator}; }; }

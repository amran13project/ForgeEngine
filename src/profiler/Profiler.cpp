#include "profiler/Profiler.h"
namespace forge::profiler {void Profiler::beginFrame(){frameStart_=std::chrono::steady_clock::now();samples_.clear();}void Profiler::endFrame(){frameMs_=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-frameStart_).count();}void Profiler::sample(const std::string&n,double ms){samples_.push_back({n,ms});}}

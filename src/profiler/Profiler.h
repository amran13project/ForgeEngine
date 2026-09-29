#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
namespace forge::profiler {struct Sample{std::string name;double ms{};};class Profiler{public:void beginFrame();void endFrame();void sample(const std::string&,double);double frameMs()const{return frameMs_;}double fps()const{return frameMs_>0?1000.0/frameMs_:0;}const std::vector<Sample>&samples()const{return samples_;}private:std::chrono::steady_clock::time_point frameStart_{};double frameMs_{0};std::vector<Sample>samples_;};class ScopedSample{public:ScopedSample(Profiler&p,const std::string&n):p_(p),n_(n),s_(std::chrono::steady_clock::now()){}~ScopedSample(){p_.sample(n_,std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-s_).count());}private:Profiler&p_;std::string n_;std::chrono::steady_clock::time_point s_;};}

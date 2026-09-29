#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace forge::gameplay {

class Health { public: explicit Health(float max=100.0f):max_(max),current_(max){} void damage(float v){current_=std::max(0.0f,current_-std::max(0.0f,v));} void heal(float v){current_=std::min(max_,current_+std::max(0.0f,v));} bool alive()const{return current_>0.0f;} float current()const{return current_;} float max()const{return max_;} private:float max_,current_; };

struct ItemStack { std::string id; int count{0}; };
class Inventory {
public:
    bool add(const std::string& id, int count=1);
    bool remove(const std::string& id, int count=1);
    int count(const std::string& id) const;
    const std::vector<ItemStack>& items() const { return items_; }
private: std::vector<ItemStack> items_;
};

class Currency { public: explicit Currency(long long value=0):value_(value){} void add(long long v){value_+=v;} bool spend(long long v){if(v<0||value_<v)return false;value_-=v;return true;} long long value()const{return value_;} private:long long value_; };

struct Quest { std::string id,title; int progress{0}, target{1}; bool completed{false}; };
class QuestLog { public: void add(const Quest&q){quests_.push_back(q);} bool advance(const std::string&id,int amount=1); const std::vector<Quest>& quests()const{return quests_;} private:std::vector<Quest> quests_; };

class Timer { public: void start(float seconds){remaining_=std::max(0.0f,seconds);running_=true;} void stop(){running_=false;remaining_=0;} void tick(float dt){if(running_){remaining_=std::max(0.0f,remaining_-std::max(0.0f,dt));if(remaining_<=0)running_=false;}} bool running()const{return running_;} bool done()const{return !running_&&remaining_<=0;} float remaining()const{return remaining_;} private:float remaining_{0};bool running_{false}; };

} // namespace forge::gameplay

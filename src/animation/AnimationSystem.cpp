#include "animation/AnimationSystem.h"
#include <algorithm>
#include <cmath>
namespace forge::animation {
void Animator::addClip(const Clip&c){if(!c.name.empty()&&c.duration>0)clips_[c.name]=c;}
void Animator::addState(const std::string&n,const State&s){if(!n.empty()&&!s.clip.empty())states_[n]=s;}
bool Animator::play(const std::string&s){auto it=states_.find(s);if(it==states_.end()||clips_.find(it->second.clip)==clips_.end())return false;state_=s;time_=0;playing_=true;return true;}
void Animator::update(float dt){if(!playing_)return;auto st=states_.find(state_);if(st==states_.end()){playing_=false;return;}auto c=clips_.find(st->second.clip);if(c==clips_.end()){playing_=false;return;}time_+=std::max(0.0f,dt)*std::max(0.0f,st->second.speed);if(time_>=c->second.duration){if(!c->second.loop){if(!st->second.next.empty()&&play(st->second.next))return;playing_=false;time_=c->second.duration;}else{time_=std::fmod(time_,c->second.duration);}}}
}

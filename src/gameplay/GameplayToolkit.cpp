#include "gameplay/GameplayToolkit.h"
namespace forge::gameplay {
bool Inventory::add(const std::string& id,int count){if(id.empty()||count<=0)return false;for(auto&i:items_)if(i.id==id){i.count+=count;return true;}items_.push_back({id,count});return true;}
bool Inventory::remove(const std::string&id,int count){if(id.empty()||count<=0)return false;for(auto it=items_.begin();it!=items_.end();++it)if(it->id==id){if(it->count<count)return false;it->count-=count;if(it->count==0)items_.erase(it);return true;}return false;}
int Inventory::count(const std::string&id)const{for(const auto&i:items_)if(i.id==id)return i.count;return 0;}
bool QuestLog::advance(const std::string&id,int amount){for(auto&q:quests_)if(q.id==id&&!q.completed){q.progress=std::min(q.target,q.progress+std::max(0,amount));q.completed=q.progress>=q.target;return true;}return false;}
}

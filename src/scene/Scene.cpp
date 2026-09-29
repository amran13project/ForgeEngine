#include "scene/Scene.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <sstream>
namespace forge::scene {
static std::string key(const std::string&s,const std::string&k,size_t from=0){auto p=s.find("\""+k+"\"",from);if(p==std::string::npos)return{};p=s.find(':',p);if(p==std::string::npos)return{};while(p+1<s.size()&&(s[p+1]==' '||s[p+1]=='"'))++p;auto e=s.find_first_of(",}\n\"]",p+1);return s.substr(p+1,e-p-1);}
void Scene::clear(){entities_.clear();nextId_=1;}
Entity& Scene::add(const std::string&name,const std::string&type){entities_.push_back({nextId_++,name,type});return entities_.back();}
Entity& Scene::addCube(){return add("Cube","Mesh");}
void Scene::select(int id){for(auto&e:entities_)e.selected=(e.id==id);}
Entity* Scene::selected(){for(auto&e:entities_)if(e.selected)return &e;return nullptr;}
const Entity* Scene::selected()const{for(const auto&e:entities_)if(e.selected)return &e;return nullptr;}
void Scene::removeSelected(){auto it=std::remove_if(entities_.begin(),entities_.end(),[](const Entity&e){return e.selected&&!e.locked;});entities_.erase(it,entities_.end());}
bool Scene::save(const std::filesystem::path&f,std::string&err)const{try{std::ofstream o(f,std::ios::trunc);if(!o){err="Cannot write scene";return false;}o<<"{\n  \"sceneVersion\": 2,\n  \"entities\": [\n";for(size_t i=0;i<entities_.size();++i){const auto&e=entities_[i];o<<"    {\"id\":"<<e.id<<",\"name\":\""<<e.name<<"\",\"type\":\""<<e.type<<"\",\"position\":["<<e.position.x<<","<<e.position.y<<","<<e.position.z<<"],\"rotation\":["<<e.rotation.x<<","<<e.rotation.y<<","<<e.rotation.z<<"],\"scale\":["<<e.scale.x<<","<<e.scale.y<<","<<e.scale.z<<"],\"visible\":"<<(e.visible?"true":"false")<<",\"locked\":"<<(e.locked?"true":"false")<<",\"selected\":"<<(e.selected?"true":"false")<<"}"<<(i+1<entities_.size()?",":"")<<"\n";}o<<"  ]\n}\n";return true;}catch(const std::exception&e){err=e.what();return false;}}
bool Scene::load(const std::filesystem::path&f,std::string&err){try{std::ifstream in(f);if(!in){err="Scene file not found";return false;}std::stringstream ss;ss<<in.rdbuf();auto s=ss.str();clear();size_t p=0;while((p=s.find("{\"id\"",p))!=std::string::npos){Entity e;e.id=std::stoi(key(s,"id",p));e.name=key(s,"name",p);e.type=key(s,"type",p);auto readVec=[&](const char*k,Vec3&v){auto q=s.find(std::string("\"")+k+"\"",p);auto x=s.find('[',q),y=s.find(']',x);if(x==std::string::npos||y==std::string::npos)return;std::string t=s.substr(x+1,y-x-1);std::replace(t.begin(),t.end(),',',' ');std::stringstream z(t);z>>v.x>>v.y>>v.z;};readVec("position",e.position);readVec("rotation",e.rotation);readVec("scale",e.scale);if(e.scale.x==0)e.scale={1,1,1};auto hasTrue=[&](const char* k){auto q=s.find(std::string("\"")+k+"\"",p);if(q==std::string::npos)return false;auto c=s.find(':',q);if(c==std::string::npos)return false;auto v=s.substr(c+1,5);return v.find("true")!=std::string::npos;};e.visible=hasTrue("visible");e.locked=hasTrue("locked");e.selected=hasTrue("selected");entities_.push_back(e);nextId_=std::max(nextId_,e.id+1);p+=5;}if(entities_.empty())add("Cube","Mesh");bool anySelected=false;for(const auto& e:entities_)if(e.selected){anySelected=true;break;}if(!anySelected&&!entities_.empty())select(entities_.front().id);return true;}catch(const std::exception&e){err=e.what();return false;}}
}

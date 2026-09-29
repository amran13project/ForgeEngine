#include "prefab/Prefab.h"
#include <fstream>
#include <algorithm>
#include <sstream>
namespace forge::prefab {
namespace { std::string val(const std::string&s,const char*k){auto p=s.find(std::string("\"")+k+"\"");if(p==std::string::npos)return{};p=s.find(':',p);if(p==std::string::npos)return{};p=s.find('"',p);if(p==std::string::npos)return{};auto e=s.find('"',p+1);return e==std::string::npos?std::string():s.substr(p+1,e-p-1);} }
bool PrefabStore::save(const scene::Entity&e,const std::filesystem::path&f,std::string&err)const{try{std::filesystem::create_directories(f.parent_path());std::ofstream o(f,std::ios::trunc);if(!o){err="Cannot write prefab";return false;}o<<"{\n  \"name\": \""<<e.name<<"\",\n  \"type\": \""<<e.type<<"\",\n  \"position\": ["<<e.position.x<<","<<e.position.y<<","<<e.position.z<<"],\n  \"scale\": ["<<e.scale.x<<","<<e.scale.y<<","<<e.scale.z<<"]\n}\n";return true;}catch(const std::exception&x){err=x.what();return false;}}
bool PrefabStore::load(scene::Entity&e,const std::filesystem::path&f,std::string&err)const{try{std::ifstream in(f);if(!in){err="Prefab file not found";return false;}std::stringstream ss;ss<<in.rdbuf();auto s=ss.str();e.name=val(s,"name");e.type=val(s,"type");auto readVec=[&](const char*k,forge::math::Vec3&v){auto q=s.find(std::string("\"")+k+"\""),x=s.find('[',q),y=s.find(']',x);if(x==std::string::npos||y==std::string::npos)return;std::string t=s.substr(x+1,y-x-1);std::replace(t.begin(),t.end(),',',' ');std::stringstream z(t);z>>v.x>>v.y>>v.z;};readVec("position",e.position);readVec("scale",e.scale);if(e.name.empty()){err="Invalid prefab";return false;}return true;}catch(const std::exception&x){err=x.what();return false;}}
}

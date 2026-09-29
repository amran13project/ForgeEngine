#include "hotreload/HotReload.h"
namespace forge::hotreload {
bool Watcher::changed(const std::filesystem::path&p){std::error_code ec;if(!std::filesystem::exists(p,ec)||!std::filesystem::is_regular_file(p,ec))return false;auto t=std::filesystem::last_write_time(p,ec);if(ec)return false;auto it=stamps_.find(p);if(it==stamps_.end()){stamps_[p]=t;return false;}if(it->second!=t){it->second=t;return true;}return false;}
void Watcher::forget(const std::filesystem::path&p){stamps_.erase(p);}
}

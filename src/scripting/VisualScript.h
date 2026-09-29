#pragma once
#include <string>
#include <unordered_map>
#include <vector>
namespace forge::scripting {
enum class Op { Set, Add, Print, BranchGreater };
struct Node { Op op{Op::Print}; std::string key; float value{0}; std::string text; };
class Graph {public: void add(Node n){nodes_.push_back(std::move(n));} bool execute(std::unordered_map<std::string,float>&vars,std::vector<std::string>&log) const; size_t size()const{return nodes_.size();} private: std::vector<Node>nodes_;};
}

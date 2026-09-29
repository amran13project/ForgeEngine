#pragma once
#include "core/Math.h"
#include <string>
#include <vector>
namespace forge::ai {
enum class NodeState { Success, Failure, Running };
struct Perception { bool seesPlayer{false}; bool hearsNoise{false}; float distance{999}; };
struct Agent { int entityId{}; std::string state{"Idle"}; forge::math::Vec3 target{}; Perception perception{}; std::vector<std::string> memory; };
class BehaviorBrain {public: NodeState tick(Agent&,float dt); const std::string& explanation()const{return explanation_;} private: std::string explanation_;};
class ForgeAIProvider {public: virtual ~ForgeAIProvider()=default; virtual std::string name()const=0; virtual std::string complete(const std::string&prompt)=0;};
class RuleAIProvider final: public ForgeAIProvider {public: std::string name()const override{return "Offline Rule Provider";} std::string complete(const std::string&prompt) override;};
}

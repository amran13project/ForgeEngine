#pragma once
#include <cstdint>
#include <deque>
#include <string>
#include <random>
namespace forge::network {
struct Packet {uint32_t sequence{};double sentAt{};std::string payload;};
struct Stats {uint64_t sent{},delivered{},dropped{};double simulatedLatencyMs{0};};
class NetworkLab {public:void configure(double latencyMs,double jitterMs,double lossPercent);void send(const std::string&payload,double now);void tick(double now);size_t pending()const{return queue_.size();}const Stats&stats()const{return stats_;}private:double latency_{50},jitter_{5},loss_{0};std::mt19937 rng_{42};uint32_t seq_{1};std::deque<Packet>queue_;Stats stats_{};};
}

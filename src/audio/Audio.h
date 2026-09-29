#pragma once
#include <cstdint>
#include <filesystem>
#include <string>
namespace forge::audio {
struct WavInfo {uint32_t sampleRate{};uint16_t channels{},bitsPerSample{};uint32_t dataBytes{};};
class AudioEngine {public: bool loadWav(const std::filesystem::path&,WavInfo&,std::string&); void setMasterVolume(float v){master_=v;} float masterVolume()const{return master_;} private: float master_{1};};
}

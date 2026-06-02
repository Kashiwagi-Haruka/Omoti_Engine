#pragma once
#include "Audio.h"
#include <array>
#include <cstddef>
#include <optional>

class BGMManager {
public:
	enum class BGMType {
		Title,
		Tutorial,
		Game,
		GameOver,
		Result,
		Count,
	};

	static BGMManager* GetInstance();

	void Play(BGMType type, bool restart = false);
	void Stop();
	void SetMasterVolume(float volume);
	void SetVolume(BGMType type, float volume);
	void Finalize();

private:
	struct BGMInfo {
		const char* filePath = nullptr;
		float volume = 1.0f;
	};

	static constexpr size_t kBGMCount = static_cast<size_t>(BGMType::Count);
	static const std::array<BGMInfo, kBGMCount> kBGMInfos;

	SoundData& Load(BGMType type);
	void ApplyVolume(BGMType type);
	static size_t ToIndex(BGMType type);

	std::array<SoundData, kBGMCount> bgmData_{};
	std::array<bool, kBGMCount> isLoaded_{};
	std::array<float, kBGMCount> volumes_{};
	float masterVolume_ = 1.0f;
	std::optional<BGMType> currentBGM_;
};
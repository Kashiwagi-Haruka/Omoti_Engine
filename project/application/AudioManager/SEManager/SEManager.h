#pragma once
#include "Audio.h"
#include <array>
#include <cstddef>

class SEManager {
public:
	enum class SEType {
		NormalAttack,
		EndAttack,
		Magic,
		Count,
	};

	static SEManager* GetInstance();

	void Play(SEType type);
	void PlayFromStart(SEType type);
	void SetMasterVolume(float volume);
	void SetVolume(SEType type, float volume);
	void Stop(SEType type);
	void Finalize();

private:
	struct SEInfo {
		const char* filePath = nullptr;
		float volume = 1.0f;
	};

	static constexpr size_t kSECount = static_cast<size_t>(SEType::Count);
	static const std::array<SEInfo, kSECount> kSEInfos;

	SoundData& Load(SEType type);
	void ApplyVolume(SEType type);
	static size_t ToIndex(SEType type);

	std::array<SoundData, kSECount> seData_{};
	std::array<bool, kSECount> isLoaded_{};
	std::array<float, kSECount> volumes_{};
	float masterVolume_ = 1.0f;
};
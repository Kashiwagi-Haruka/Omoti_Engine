#include "SEManager.h"
#include <algorithm>
#include <cassert>

const std::array<SEManager::SEInfo, SEManager::kSECount> SEManager::kSEInfos = {
    {
     {"Resources/audio/SE/Attack/normalAttack.mp3", 1.0f},
     {"Resources/audio/SE/Attack/normalAttack2.mp3", 0.4f},
     {"Resources/audio/SE/Attack/endAttack.mp3", 1.0f},
     {"Resources/audio/SE/magic.mp3", 1.0f},
     }
};

SEManager* SEManager::GetInstance() {
	static SEManager instance;
	return &instance;
}

void SEManager::Play(SEType type) { Audio::GetInstance()->SoundPlayWave(Load(type), false); }

void SEManager::PlayFromStart(SEType type) { Audio::GetInstance()->SoundPlayWaveFromStart(Load(type), false); }

void SEManager::SetMasterVolume(float volume) {
	masterVolume_ = std::clamp(volume, 0.0f, 1.0f);
	for (size_t i = 0; i < kSECount; ++i) {
		if (isLoaded_[i]) {
			ApplyVolume(static_cast<SEType>(i));
		}
	}
}

void SEManager::SetVolume(SEType type, float volume) {
	volumes_[ToIndex(type)] = std::clamp(volume, 0.0f, 1.0f);
	if (isLoaded_[ToIndex(type)]) {
		ApplyVolume(type);
	}
}

void SEManager::Stop(SEType type) {
	const size_t index = ToIndex(type);
	if (!isLoaded_[index]) {
		return;
	}
	Audio::GetInstance()->StopSound(seData_[index]);
}

void SEManager::Finalize() {
	for (size_t i = 0; i < kSECount; ++i) {
		if (isLoaded_[i]) {
			Audio::GetInstance()->SoundUnload(&seData_[i]);
			isLoaded_[i] = false;
		}
		volumes_[i] = 0.0f;
	}
	masterVolume_ = 1.0f;
}

SoundData& SEManager::Load(SEType type) {
	const size_t index = ToIndex(type);
	if (!isLoaded_[index]) {
		const SEInfo& info = kSEInfos[index];
		assert(info.filePath != nullptr);
		seData_[index] = Audio::GetInstance()->SoundLoadFile(info.filePath);
		volumes_[index] = info.volume;
		isLoaded_[index] = true;
		ApplyVolume(type);
	}
	return seData_[index];
}

void SEManager::ApplyVolume(SEType type) {
	const size_t index = ToIndex(type);
	Audio::GetInstance()->SetSoundVolume(&seData_[index], volumes_[index] * masterVolume_);
}

size_t SEManager::ToIndex(SEType type) {
	const size_t index = static_cast<size_t>(type);
	assert(index < kSECount);
	return index;
}
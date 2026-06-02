#include "BGMManager.h"
#include <algorithm>
#include <cassert>

const std::array<BGMManager::BGMInfo, BGMManager::kBGMCount> BGMManager::kBGMInfos = {
    {
     {"Resources/audio/BGM/Rendez-vous_2.mp3", 0.3f},
     {"Resources/audio/BGM/Tailshaft.mp3", 0.3f},
     {"Resources/audio/BGM/Tailshaft.mp3", 0.3f},
     {"Resources/audio/BGM/おそろい.mp3", 1.0f},
     {"Resources/audio/BGM/アンドロイドの涙.mp3", 1.0f},
     }
};

BGMManager* BGMManager::GetInstance() {
	static BGMManager instance;
	return &instance;
}

void BGMManager::Play(BGMType type, bool restart) {
	SoundData& bgm = Load(type);
	if (currentBGM_.has_value()) {
		SoundData& current = bgmData_[ToIndex(currentBGM_.value())];
		if (currentBGM_.value() == type && !restart) {
			return;
		}
		Audio::GetInstance()->StopSound(current);
	}

	ApplyVolume(type);
	Audio::GetInstance()->SoundPlayWave(bgm, true);
	currentBGM_ = type;
}

void BGMManager::Stop() {
	if (!currentBGM_.has_value()) {
		return;
	}
	Audio::GetInstance()->StopSound(bgmData_[ToIndex(currentBGM_.value())]);
	currentBGM_.reset();
}

void BGMManager::SetMasterVolume(float volume) {
	masterVolume_ = std::clamp(volume, 0.0f, 1.0f);
	for (size_t i = 0; i < kBGMCount; ++i) {
		if (isLoaded_[i]) {
			ApplyVolume(static_cast<BGMType>(i));
		}
	}
}

void BGMManager::SetVolume(BGMType type, float volume) {
	volumes_[ToIndex(type)] = std::clamp(volume, 0.0f, 1.0f);
	if (isLoaded_[ToIndex(type)]) {
		ApplyVolume(type);
	}
}

void BGMManager::Finalize() {
	Stop();
	for (size_t i = 0; i < kBGMCount; ++i) {
		if (isLoaded_[i]) {
			Audio::GetInstance()->SoundUnload(&bgmData_[i]);
			isLoaded_[i] = false;
		}
		volumes_[i] = 0.0f;
	}
	masterVolume_ = 1.0f;
}

SoundData& BGMManager::Load(BGMType type) {
	const size_t index = ToIndex(type);
	if (!isLoaded_[index]) {
		const BGMInfo& info = kBGMInfos[index];
		assert(info.filePath != nullptr);
		bgmData_[index] = Audio::GetInstance()->SoundLoadFile(info.filePath);
		volumes_[index] = info.volume;
		isLoaded_[index] = true;
		ApplyVolume(type);
	}
	return bgmData_[index];
}

void BGMManager::ApplyVolume(BGMType type) {
	const size_t index = ToIndex(type);
	Audio::GetInstance()->SetSoundVolume(&bgmData_[index], volumes_[index] * masterVolume_);
}

size_t BGMManager::ToIndex(BGMType type) {
	const size_t index = static_cast<size_t>(type);
	assert(index < kBGMCount);
	return index;
}
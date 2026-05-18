#pragma once
#include "Base/PlayableBase.h"
#include <memory>
#include <string>

class PlayableManager {
	std::unique_ptr<PlayableBase> currentPlayable_ = nullptr;
	std::unique_ptr<PlayableBase> CreatePlayable(const std::string& name) const;

public:
	void Initialize(const std::string& defaultPlayableName = "Sizuku");
	bool ChangePlayable(const std::string& name);
	PlayableBase* GetCurrentPlayable() const { return currentPlayable_.get(); }
};
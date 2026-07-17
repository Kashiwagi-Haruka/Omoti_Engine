#include "PlayableManager.h"
#include "Individual/Arte/Arte.h"
#include "Individual/Mei/Mei.h"
#include "Individual/Sizuku/Sizuku.h"
#include "Individual/Yuzuki/Yuzuki.h"
#include "Object/Characters/Base/CharacterParameterLoader.h"

void PlayableManager::Initialize(const std::string& defaultPlayableName) {
	if (!ChangePlayable(defaultPlayableName)) {
		ChangePlayable("Sizuku");
	}
}

std::unique_ptr<PlayableBase> PlayableManager::CreatePlayable(const std::string& name) const {
	if (name == "Sizuku") {
		return std::make_unique<Sizuku>();
	}
	if (name == "Mei") {
		return std::make_unique<Mei>();
	}
	if (name == "Yuzuki") {
		return std::make_unique<Yuzuki>();
	}
	if (name == "Arte") {
		return std::make_unique<Arte>();
	}
	return nullptr;
}

bool PlayableManager::ChangePlayable(const std::string& name) {
	auto nextPlayable = CreatePlayable(name);
	if (!nextPlayable) {
		return false;
	}

	nextPlayable->Initialize();

	BaseParameter baseParameter{};
	Parameter parameter{};
	int reinforcementAmount = 0;
	if (CharacterParameterLoader::LoadCurrentParameters(name, baseParameter, parameter, &reinforcementAmount)) {
		nextPlayable->SetBaseParameter(baseParameter);
		nextPlayable->SetParameter(parameter);
		nextPlayable->SetReinforcementAmount(reinforcementAmount);
	}

	currentPlayable_ = std::move(nextPlayable);
	return true;
}
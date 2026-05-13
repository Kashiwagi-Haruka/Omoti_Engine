#pragma once
#include "Base/PlayableBase.h"
class PlayableManager {

	PlayableBase* currentPlayable_ = nullptr;


	PlayableBase* ChangePlayable(std::string name);


};

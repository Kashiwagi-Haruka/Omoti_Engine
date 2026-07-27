#pragma once
#include "Object/Characters/Base/CharacterParameters.h"
#include <string>

namespace CharacterParameterLoader {

bool LoadCurrentParameters(const std::string& characterName, BaseParameter& outBaseParameter, Parameter& outParameter, int* outReinforcementAmount = nullptr);

} // namespace CharacterParameterLoader
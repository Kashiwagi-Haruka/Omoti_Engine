#pragma once

#include "Object/Characters/Base/CharacterParameters.h"
#include "Vector3.h"

#include <nlohmann/json_fwd.hpp>
#include <string>

enum class EnemyType {
	Switch,
	BrainStealer,
};

struct EnemyParameter {
	std::string modelName = "Enemy";
	Vector3 scale{3.0f, 3.0f, 3.0f};
	BaseParameter baseParameter{10.0f, 0.0f, 10.0f};

	Parameter parameter = [] {
		Parameter value{};
		value.level = 1;
		value.HP = 3000.0f;
		value.Speed = 0.1f;
		value.CriticalDamage = 1.5f;
		return value;
	}();

	float stunDuration = 0.75f;
	float damageInvincibleDuration = 0.3f;
	float backStepDistance = 5.0f;
	float backStepDuration = 0.18f;
	float deathDuration = 0.45f;
	float deathRotateZ = 1.57f;

	void Validate() const;
};

void to_json(nlohmann::json& json, const EnemyParameter& parameter);
void from_json(const nlohmann::json& json, EnemyParameter& parameter);
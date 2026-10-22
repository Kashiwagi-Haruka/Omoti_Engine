#pragma once

#include "Object/Characters/Base/CharacterParameters.h"
#include "Vector3.h"

#include <nlohmann/json_fwd.hpp>
#include <string>

enum class EnemyType {
	BrainStealer,
};

// 全種類の敵が共通して使用する設定。
// 戦闘中の残りHPやタイマーは保持しない。
struct EnemyParameter {
	std::string modelName = "Enemy";

	Vector3 scale{3.0f, 3.0f, 3.0f};

	// 現在のEnemyの基礎値を引き継ぐ。
	BaseParameter baseParameter{10.0f, 0.0f, 10.0f};

	Parameter parameter = [] {
		Parameter value{};

		value.level = 1;
		value.HP = 3000.0f;

		// 現在のゲームと同じく、1フレームあたりの移動量。
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
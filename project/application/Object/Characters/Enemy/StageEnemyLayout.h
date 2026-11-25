#pragma once

#include "Base/Parameter/EnemyParameter.h"

#include <array>
#include <vector>

inline constexpr int kStagePhaseCount = 5;
inline constexpr char kStage1EnemyPath[] = "Resources/JSON/Stage/Stage1/enemies.json";

struct StageEnemySpawn {
	EnemyType type = EnemyType::Switch;
	Vector3 position{};
};

using StageEnemyLayout = std::array<std::vector<StageEnemySpawn>, kStagePhaseCount>;

StageEnemyLayout LoadStageEnemyLayout(const char* path);
void SaveStageEnemyLayout(const char* path, const StageEnemyLayout& layout);
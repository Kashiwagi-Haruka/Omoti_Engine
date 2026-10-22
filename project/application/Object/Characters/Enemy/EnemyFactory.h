#pragma once

#include "Object/Characters/Enemy/Individual/BrainStealer/BrainStealer.h"

#include <memory>

namespace EnemyFactory {

std::unique_ptr<BaseEnemy> Create(EnemyType type, const EnemyParameter& common, const BrainStealer::SpecificParameter& specific);

} // namespace EnemyFactory
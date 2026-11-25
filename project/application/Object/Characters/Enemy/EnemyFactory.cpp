#include "EnemyFactory.h"
#include "Individual/Switch/SwitchEnemy.h"

#include <stdexcept>

std::unique_ptr<BaseEnemy> EnemyFactory::Create(EnemyType type, const EnemyParameter& common, const BrainStealer::SpecificParameter& specific) {
	switch (type) {
	case EnemyType::Switch:
		return std::make_unique<SwitchEnemy>();
	case EnemyType::BrainStealer:
		return std::make_unique<BrainStealer>(common, specific);
	}

	throw std::invalid_argument("Unknown enemy type.");
}
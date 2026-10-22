#include "EnemyParameter.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <stdexcept>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BaseParameter, HP, Attack, Deffence)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Parameter, level, HP, Attack, Deffence, Speed, CriticalRate, CriticalDamage, HealingEffect, AttributeAffinity, AttributeDamageRate, AttributeResistanceRate)

namespace {

bool IsNonNegative(float value) { return std::isfinite(value) && value >= 0.0f; }

bool IsPositive(float value) { return std::isfinite(value) && value > 0.0f; }

} // namespace

void EnemyParameter::Validate() const {
	if (modelName.empty() || modelName.size() >= 256) {
		throw std::runtime_error("Enemy model name must contain 1-255 bytes.");
	}

	if (!IsPositive(scale.x) || !IsPositive(scale.y) || !IsPositive(scale.z)) {
		throw std::runtime_error("Invalid enemy scale.");
	}

	if (!IsNonNegative(baseParameter.HP) || !IsNonNegative(baseParameter.Attack) || !IsNonNegative(baseParameter.Deffence)) {
		throw std::runtime_error("Invalid enemy base parameters.");
	}

	if (parameter.level < 1 || !std::isfinite(parameter.HP) || parameter.HP < 1.0f || parameter.HP > 999999.0f || !IsNonNegative(parameter.Attack) || !IsNonNegative(parameter.Deffence) ||
	    !IsNonNegative(parameter.Speed) || !IsNonNegative(parameter.CriticalRate) || parameter.CriticalRate > 1.0f || !IsNonNegative(parameter.CriticalDamage) ||
	    !IsNonNegative(parameter.HealingEffect) || !IsNonNegative(parameter.AttributeAffinity)) {
		throw std::runtime_error("Invalid enemy combat parameters.");
	}

	for (float value : parameter.AttributeDamageRate) {
		if (!std::isfinite(value)) {
			throw std::runtime_error("Invalid enemy attribute damage rate.");
		}
	}

	for (float value : parameter.AttributeResistanceRate) {
		if (!std::isfinite(value)) {
			throw std::runtime_error("Invalid enemy attribute resistance rate.");
		}
	}

	if (!IsNonNegative(stunDuration) || !IsNonNegative(damageInvincibleDuration) || !IsNonNegative(backStepDistance) || !IsPositive(backStepDuration) || !IsPositive(deathDuration) ||
	    !std::isfinite(deathRotateZ)) {
		throw std::runtime_error("Invalid enemy timing parameters.");
	}
}

void to_json(nlohmann::json& json, const EnemyParameter& value) {

	json = {
	    {"modelName",                value.modelName                              },
	    {"scale",	                {value.scale.x, value.scale.y, value.scale.z}},
	    {"baseParameter",            value.baseParameter                          },
	    {"parameter",                value.parameter                              },
	    {"stunDuration",             value.stunDuration                           },
	    {"damageInvincibleDuration", value.damageInvincibleDuration               },
	    {"backStepDistance",         value.backStepDistance                       },
	    {"backStepDuration",         value.backStepDuration                       },
	    {"deathDuration",            value.deathDuration                          },
	    {"deathRotateZ",             value.deathRotateZ                           }
    };
}

void from_json(const nlohmann::json& json, EnemyParameter& out) {

	EnemyParameter value;

	json.at("modelName").get_to(value.modelName);

	const auto& scale = json.at("scale");

	if (!scale.is_array() || scale.size() != 3) {
		throw std::runtime_error("Enemy scale must contain three values.");
	}

	value.scale = {scale.at(0).get<float>(), scale.at(1).get<float>(), scale.at(2).get<float>()};

	json.at("baseParameter").get_to(value.baseParameter);
	json.at("parameter").get_to(value.parameter);

	json.at("stunDuration").get_to(value.stunDuration);

	json.at("damageInvincibleDuration").get_to(value.damageInvincibleDuration);

	json.at("backStepDistance").get_to(value.backStepDistance);

	json.at("backStepDuration").get_to(value.backStepDuration);

	json.at("deathDuration").get_to(value.deathDuration);

	json.at("deathRotateZ").get_to(value.deathRotateZ);

	value.Validate();

	out = value;
}
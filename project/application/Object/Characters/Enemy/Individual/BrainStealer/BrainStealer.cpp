#define NOMINMAX
#include "BrainStealer.h"

#include "Function.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <utility>

namespace {
using Json = nlohmann::json;

void ValidateSpecificParameter(const BrainStealer::SpecificParameter& value) {
	const auto valid = [](float number) { return std::isfinite(number) && number >= 0.0f; };
	if (!valid(value.playerChaseRange) || !valid(value.attackRange) || !valid(value.attackCooldown)) {
		throw std::runtime_error("Invalid BrainStealer specific parameters.");
	}
}

void ValidateModel(const EnemyParameter& common) {
	if (common.modelName != "Brain") {
		throw std::runtime_error("BrainStealer modelName must be Brain.");
	}
}

BrainStealer::SpecificParameter SpecificParameterFromJson(const Json& json) {
	BrainStealer::SpecificParameter value;
	json.at("playerChaseRange").get_to(value.playerChaseRange);
	json.at("attackRange").get_to(value.attackRange);
	json.at("attackCooldown").get_to(value.attackCooldown);
	ValidateSpecificParameter(value);
	return value;
}

Json SpecificParameterToJson(const BrainStealer::SpecificParameter& value) {
	return {
	    {"playerChaseRange", value.playerChaseRange},
	    {"attackRange",      value.attackRange     },
	    {"attackCooldown",   value.attackCooldown  },
	};
}
} // namespace

BrainStealer::BrainStealer(const EnemyParameter& common, const SpecificParameter& specific) : BaseEnemy(common), specificParameter_(specific) {
	ValidateModel(common);
	ValidateSpecificParameter(specific);
}

const char* BrainStealer::ParameterPath() { return "Resources/JSON/Enemy/BrainStealer/parameters.json"; }

bool BrainStealer::LoadParameter(EnemyParameter& outCommon, SpecificParameter& outSpecific, std::string& error) {
	try {
		std::ifstream stream(ParameterPath());
		if (!stream) {
			throw std::runtime_error(std::string("Cannot open: ") + ParameterPath());
		}

		const Json root = Json::parse(stream);
		const auto& version = root.at("schemaVersion");
		if (!version.is_number_integer() || version != 1) {
			throw std::runtime_error("Unsupported enemy parameter version.");
		}
		if (root.at("type") != "BrainStealer") {
			throw std::runtime_error("Enemy parameter type is not BrainStealer.");
		}

		EnemyParameter common = root.at("common").get<EnemyParameter>();
		ValidateModel(common);
		const auto specific = SpecificParameterFromJson(root.at("specific"));

		outCommon = std::move(common);
		outSpecific = specific;
		error.clear();
		return true;
	} catch (const std::exception& exception) {
		error = exception.what();
		return false;
	}
}

bool BrainStealer::SaveParameter(const EnemyParameter& common, const SpecificParameter& specific, std::string& error) {
	try {
		common.Validate();
		ValidateModel(common);
		ValidateSpecificParameter(specific);

		const Json root{
		    {"schemaVersion", 1		                        },
		    {"type",          "BrainStealer"                   },
		    {"common",        common                           },
		    {"specific",      SpecificParameterToJson(specific)},
		};

		const std::filesystem::path destination(ParameterPath());
		std::filesystem::create_directories(destination.parent_path());

		auto temporary = destination;
		temporary += ".tmp";
		try {
			std::ofstream stream(temporary, std::ios::trunc);
			if (!stream) {
				throw std::runtime_error("Cannot open: " + temporary.string());
			}
			stream << root.dump(2) << '\n';
			stream.close();
			if (!stream) {
				throw std::runtime_error("Write failed: " + temporary.string());
			}
			std::filesystem::rename(temporary, destination);
		} catch (...) {
			std::error_code removeError;
			std::filesystem::remove(temporary, removeError);
			throw;
		}

		error.clear();
		return true;
	} catch (const std::exception& exception) {
		error = exception.what();
		return false;
	}
}

void BrainStealer::UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) {
	const float chaseRange = specificParameter_.playerChaseRange;
	const bool targetsPlayer = isPlayerAlive && Function::LengthSquared(playerPosition - GetPosition()) <= chaseRange * chaseRange;

	const Vector3& targetPosition = targetsPlayer ? playerPosition : housePosition;
	if (canMove) {
		MoveToward(targetPosition, GetParameter().Speed);
	}

	Vector3 toTarget = targetPosition - GetPosition();
	Vector3 toHouse = housePosition - GetPosition();
	toTarget.y = 0.0f;
	toHouse.y = 0.0f;

	const Vector3 scale = GetScale();
	const float enemyRadius = std::max({scale.x, scale.y, scale.z});
	const float houseRadius = std::max({houseScale.x, houseScale.y, houseScale.z});
	const float attackRange = specificParameter_.attackRange;
	const float targetRange = targetsPlayer ? attackRange + enemyRadius : attackRange;
	const float houseRange = attackRange + houseRadius + enemyRadius;

	if (Function::LengthSquared(toTarget) <= targetRange * targetRange || Function::LengthSquared(toHouse) <= houseRange * houseRange) {
		TryStartAttack(specificParameter_.attackCooldown);
	}
}
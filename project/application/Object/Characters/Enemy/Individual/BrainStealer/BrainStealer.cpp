#define NOMINMAX
#include "BrainStealer.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3d.h"
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

void BrainStealer::InitializeModelAnimation(Object3d& object) {
	object.SetSkinCluster(nullptr);
	currentClip_ = nullptr;
	animationTime_ = 0.0f;
	animationClips_.clear();
	skeleton_.reset();
	skinCluster_ = {};

	Model* model = ModelManager::GetInstance()->FindModel("Brain");
	if (!model) {
		return;
	}

	animationClips_ = Animation::LoadAnimationClips("Resources/3d/Enemy/Brain", "Brain");
	skeleton_ = std::make_unique<Skeleton>(Skeleton().Create(model->GetModelData().rootnode));
	skinCluster_ = CreateSkinCluster(*skeleton_, *model);

	if (!skinCluster_.mappedPalette.empty()) {
		object.SetSkinCluster(&skinCluster_);
		UpdateModelAnimation(0.0f, false);
	}
}

void BrainStealer::UpdateModelAnimation(float deltaTime, bool isMoving) {
	if (!skeleton_ || skinCluster_.mappedPalette.empty()) {
		return;
	}

	const char* clipName = isMoving ? "Move" : "Idle.001";
	if (!currentClip_ || currentClip_->name != clipName) {
		const auto clip = std::find_if(animationClips_.begin(), animationClips_.end(), [clipName](const Animation::AnimationData& candidate) { return candidate.name == clipName; });
		if (clip == animationClips_.end()) {
			return;
		}

		// 走り始めや停止時だけ、クリップの先頭に戻す。
		currentClip_ = &*clip;
		animationTime_ = 0.0f;
	} else {
		animationTime_ = Animation::AdvanceTime(*currentClip_, animationTime_, deltaTime, true);
	}

	skeleton_->ApplyAnimation(*currentClip_, animationTime_);
	skeleton_->Update();
	UpdateSkinCluster(skinCluster_, *skeleton_);
}

void BrainStealer::UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) {
	constexpr float kInwardRatio = 0.25f;
	constexpr float kAroundRatio = 0.9682458f;
	constexpr float kAttackRangeMargin = 0.01f;

	const Vector3 position = GetPosition();
	const float speed = GetParameter().Speed;
	const float chaseRange = specificParameter_.playerChaseRange;
	const bool targetsPlayer = isPlayerAlive && Function::LengthSquared(playerPosition - position) <= chaseRange * chaseRange;

	const Vector3 scale = GetScale();
	const float enemyRadius = std::max({scale.x, scale.y, scale.z});
	const float houseRadius = std::max({houseScale.x, houseScale.y, houseScale.z});
	const float attackRange = specificParameter_.attackRange;
	const float playerRange = attackRange + enemyRadius;
	const float houseRange = attackRange + houseRadius + enemyRadius;

	if (canMove && !IsAttacking()) {
		if (targetsPlayer) {
			MoveToward(playerPosition, speed);
		} else {
			const float x = position.x - housePosition.x;
			const float z = position.z - housePosition.z;
			const float radius = std::hypot(x, z);

			if (radius > houseRange && speed > 0.0f) {
				const float stopRadius = std::max(0.0f, houseRange - kAttackRangeMargin);
				const float nextRadius = std::max(stopRadius, radius - speed * kInwardRatio);
				const float averageRadius = (radius + nextRadius) * 0.5f;
				const float nextAngle = std::atan2(z, x) + speed * kAroundRatio / averageRadius;

				const Vector3 nextPosition{
				    housePosition.x + nextRadius * std::cos(nextAngle),
				    position.y,
				    housePosition.z + nextRadius * std::sin(nextAngle),
				};

				const float step = std::hypot(nextPosition.x - position.x, nextPosition.z - position.z);
				MoveToward(nextPosition, step);
			}
		}
	}

	Vector3 toPlayer = playerPosition - GetPosition();
	Vector3 toHouse = housePosition - GetPosition();
	toPlayer.y = 0.0f;
	toHouse.y = 0.0f;

	const bool playerInRange = targetsPlayer && Function::LengthSquared(toPlayer) <= playerRange * playerRange;
	const bool houseInRange = Function::LengthSquared(toHouse) <= houseRange * houseRange;

	if (canMove && !IsAttacking() && (playerInRange || houseInRange)) {
		// 既存の MoveToward は速度 0 なら向きだけを更新する。
		MoveToward(playerInRange ? playerPosition : housePosition, 0.0f);
		TryStartAttack(specificParameter_.attackCooldown);
	}
}
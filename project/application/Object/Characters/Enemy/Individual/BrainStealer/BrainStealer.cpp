#define NOMINMAX

#include "BrainStealer.h"

#include "Function.h"
#include "Object/Characters/Enemy/EnemyAttack.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace {

void ValidateSpecificParameter(const BrainStealer::SpecificParameter& value) {

	const auto valid = [](float number) { return std::isfinite(number) && number >= 0.0f; };

	if (!valid(value.playerChaseRange) || !valid(value.attackRange) || !valid(value.attackCooldown)) {

		throw std::runtime_error("Invalid BrainStealer specific parameters.");
	}
}

nlohmann::json SpecificParameterToJson(const BrainStealer::SpecificParameter& value) {

	return {
	    {"playerChaseRange", value.playerChaseRange},
        {"attackRange",      value.attackRange     },
        {"attackCooldown",   value.attackCooldown  }
    };
}

BrainStealer::SpecificParameter SpecificParameterFromJson(const nlohmann::json& json) {

	BrainStealer::SpecificParameter value;

	json.at("playerChaseRange").get_to(value.playerChaseRange);

	json.at("attackRange").get_to(value.attackRange);

	json.at("attackCooldown").get_to(value.attackCooldown);

	ValidateSpecificParameter(value);

	return value;
}

} // namespace

BrainStealer::BrainStealer(const EnemyParameter& common, const SpecificParameter& specific) : specificParameter_(specific), attack_(std::make_unique<EnemyAttack>()) {

	common.Validate();
	ValidateSpecificParameter(specific);

	enemyParameter_ = common;
}

BrainStealer::~BrainStealer() = default;

const char* BrainStealer::ParameterPath() { return "Resources/JSON/Enemy/BrainStealer/parameters.json"; }

bool BrainStealer::LoadParameter(EnemyParameter& outCommon, SpecificParameter& outSpecific, std::string& error) {

	try {
		std::ifstream stream(ParameterPath());

		if (!stream) {
			throw std::runtime_error(std::string("Cannot open: ") + ParameterPath());
		}

		nlohmann::json root;
		stream >> root;

		if (root.at("schemaVersion").get<int>() != 1) {
			throw std::runtime_error("Unsupported enemy parameter version.");
		}

		if (root.at("type").get<std::string>() != "BrainStealer") {

			throw std::runtime_error("Enemy parameter type is not BrainStealer.");
		}

		// すべて読込・検証できてから出力先へ反映する。
		EnemyParameter common = root.at("common").get<EnemyParameter>();

		SpecificParameter specific = SpecificParameterFromJson(root.at("specific"));

		outCommon = common;
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
		ValidateSpecificParameter(specific);

		const nlohmann::json root = {
		    {"schemaVersion", 1		                        },
            {"type",          "BrainStealer"                   },
            {"common",        common                           },
            {"specific",      SpecificParameterToJson(specific)}
        };

		const std::string text = root.dump(2);
		const std::filesystem::path path = ParameterPath();

		std::filesystem::create_directories(path.parent_path());

		std::ofstream stream(path);

		if (!stream) {
			throw std::runtime_error("Cannot write: " + path.string());
		}

		stream << text;
		stream.close();

		if (!stream) {
			throw std::runtime_error("Failed to save: " + path.string());
		}

		error.clear();
		return true;
	} catch (const std::exception& exception) {
		error = exception.what();
		return false;
	}
}

void BrainStealer::OnInitialize() {
	attackTimer_ = 0.0f;

	attack_->Initialize(camera_);
	attack_->Cancel();
}

void BrainStealer::UpdateTimers() {
	if (!IsAttacking()) {
		attackTimer_ += kDeltaTime;
	}
}

void BrainStealer::UpdateBehavior(const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive) {

	Vector3 targetPosition = housePos;
	bool targetingPlayer = false;

	if (isPlayerAlive) {
		const Vector3 toPlayer = playerPos - transform_.translate;

		const float chaseRange = specificParameter_.playerChaseRange;

		if (Function::LengthSquared(toPlayer) <= chaseRange * chaseRange) {

			targetPosition = playerPos;
			targetingPlayer = true;
		}
	}

	Vector3 toTarget = targetPosition - transform_.translate;

	toTarget.y = 0.0f;
	velocity_ = {};

	if (Function::LengthSquared(toTarget) > 0.0001f) {
		const Vector3 direction = Function::Normalize(toTarget);

		transform_.rotate.y = std::atan2(direction.x, direction.z);

		if (!IsAttacking()) {
			velocity_ = direction * parameter_.Speed;
		}
	}

	transform_.translate += velocity_;

	toTarget = targetPosition - transform_.translate;
	toTarget.y = 0.0f;

	Vector3 toHouse = housePos - transform_.translate;

	toHouse.y = 0.0f;

	const float enemyRadius = std::max({transform_.scale.x, transform_.scale.y, transform_.scale.z});

	const float houseRadius = std::max({houseScale.x, houseScale.y, houseScale.z});

	const float attackRange = specificParameter_.attackRange;

	const float targetAttackRange = targetingPlayer ? attackRange + enemyRadius : attackRange;

	const float houseAttackRange = attackRange + houseRadius + enemyRadius;

	const bool inAttackRange = Function::LengthSquared(toTarget) <= targetAttackRange * targetAttackRange || Function::LengthSquared(toHouse) <= houseAttackRange * houseAttackRange;

	if (!IsAttacking() && inAttackRange && attackTimer_ >= specificParameter_.attackCooldown) {

		attack_->Start(transform_);
		attackTimer_ = 0.0f;
	}
}

void BrainStealer::UpdateAction() { attack_->Update(); }

void BrainStealer::DrawAction() { attack_->Draw(); }

void BrainStealer::CancelAction() { attack_->Cancel(); }

void BrainStealer::OnCameraChanged() { attack_->SetCamera(camera_); }

bool BrainStealer::IsAttacking() const { return attack_->IsAttacking(); }

bool BrainStealer::IsAttackHitActive() const { return attack_->IsHitActive(); }

bool BrainStealer::ConsumeAttackHit() { return attack_->ConsumeHit(); }

Vector3 BrainStealer::GetAttackPosition() const { return attack_->GetPosition(); }

float BrainStealer::GetAttackHitSize() const { return attack_->GetHitSize(); }
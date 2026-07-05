#define NOMINMAX
#include "Enemy.h"
#include "Camera.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3d.h"
#include "Vector4.h"
#include <algorithm>
#include <cmath>

namespace {
const Vector4 kDamageInvincibleColor = {1.0f, 0.0f, 0.0f, 1.0f};
const Vector4 kDefaultColor = {1.0f, 1.0f, 1.0f, 1.0f};
const Vector4 kDeathColor = {1.0f, 0.2f, 0.2f, 1.0f};
} // namespace

Enemy::Enemy() {

	object_ = std::make_unique<Object3d>();
	enemyStun = std::make_unique<EnemyStun>();
	adhesion_ = std::make_unique<Adhesion>();
}
void Enemy::Initialize(Camera* camera, Vector3 translates) {
	isAlive = true;
	isStun_ = false;
	HP = 100;
	stunTime = 0;
	attackTimer_ = 0.0f;
	damageInvincibleTimer_ = 0.0f;
	lastSkillDamageId_ = -1;
	isDying_ = false;
	deathTimer_ = 0.0f;
	finalComboBackStepTimer_ = 0.0f;
	isFinalComboBackStepping_ = false;
	finalComboBackStepStart_ = translates;
	finalComboBackStepTarget_ = translates;

	object_->Initialize();
	object_->SetModel("Enemy");
	camera_ = camera;
	transform_ = {
	    .scale{3.0f, 3.0f, 3.0f},
        .rotate{0.0f, 0.0f, 0.0f},
        .translate = translates
    };
	object_->SetTransform(transform_);
	object_->SetCamera(camera_);
	object_->Update();

	enemyStun->Initialize();
	enemyAttack_ = std::make_unique<EnemyAttack>();
	enemyAttack_->Initialize(camera_);
	adhesion_->Initialize();
	adhesion_->SetCamera(camera_);
	object_->SetColor(kDefaultColor);
}
void Enemy::Update(const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive) {
	const float deltaTime = 1.0f / 60.0f;
	if (isDying_) {
		deathTimer_ += deltaTime;
		float progress = std::clamp(deathTimer_ / deathDuration_, 0.0f, 1.0f);
		transform_.rotate.z = deathTargetRotateZ_ * progress;
		object_->SetColor(kDeathColor);
		if (enemyAttack_) {
			enemyAttack_->Cancel();
			enemyAttack_->Update();
		}
		object_->SetCamera(camera_);
		object_->SetTransform(transform_);
		object_->Update();
		if (progress >= 1.0f) {
			isAlive = false;
			isDying_ = false;
		}
		return;
	}
	if (!enemyAttack_ || !enemyAttack_->IsAttacking()) {
		attackTimer_ += deltaTime;
	}
	if (damageInvincibleTimer_ > 0.0f) {
		damageInvincibleTimer_ -= deltaTime;
		if (damageInvincibleTimer_ < 0.0f) {
			damageInvincibleTimer_ = 0.0f;
		}
	}
	object_->SetColor(damageInvincibleTimer_ > 0.0f ? kDamageInvincibleColor : kDefaultColor);
	if (isFinalComboBackStepping_) {
		finalComboBackStepTimer_ += deltaTime;
		float backStepProgress = std::clamp(finalComboBackStepTimer_ / finalComboBackStepDuration_, 0.0f, 1.0f);
		transform_.translate = Function::Lerp(finalComboBackStepStart_, finalComboBackStepTarget_, backStepProgress);
		velocity_ = {0.0f, 0.0f, 0.0f};
		if (enemyAttack_) {
			enemyAttack_->Cancel();
			enemyAttack_->Update();
		}
		if (backStepProgress >= 1.0f) {
			isFinalComboBackStepping_ = false;
			finalComboBackStepTimer_ = 0.0f;
		}
		object_->SetCamera(camera_);
		object_->SetTransform(transform_);
		object_->Update();
		if (adhesion_) {
			adhesion_->SetCamera(camera_);
			adhesion_->SetTransform(transform_);
			adhesion_->Update();
		}
		return;
	}
	Vector3 targetPosition = housePos;
	bool isTargetingPlayer = false;
	if (isPlayerAlive) {
		Vector3 toPlayer = playerPos - transform_.translate;
		if (Function::LengthSquared(toPlayer) <= playerChaseRange_ * playerChaseRange_) {
			targetPosition = playerPos;
			isTargetingPlayer = true;
		}
	}

	// 敵の更新処理
	if (!isStun_) {
		Vector3 toTarget = targetPosition - transform_.translate;
		toTarget.y = 0.0f;
		if (Function::LengthSquared(toTarget) > 0.0001f) {
			Vector3 direction = Function::Normalize(toTarget);
			direction_ = direction;
			transform_.rotate.y = std::atan2(direction.x, direction.z);
			velocity_ = direction * maxSpeed_;
		} else {
			velocity_ = {0.0f, 0.0f, 0.0f};
		}
		if (enemyAttack_ && enemyAttack_->IsAttacking()) {
			velocity_ = {0.0f, 0.0f, 0.0f};
		}
	}

	if (isStun_) {
		velocity_ = {0.0f, 0.0f, 0.0f};
		stunTime++;
		enemyStun->SetCamera(camera_);
		enemyStun->SetTranslate(transform_.translate);
		enemyStun->Update();
		if (stunTime >= stunTimeMax) {
			isStun_ = false;
		}
	}
	transform_.translate += velocity_;
	object_->SetCamera(camera_);
	object_->SetTransform(transform_);
	object_->Update();
	if (adhesion_) {
		adhesion_->SetCamera(camera_);
		adhesion_->SetTransform(transform_);
		adhesion_->Update();
	}

	// 攻撃開始条件
	bool inAttackRange = false;
	if (!isStun_) {
		Vector3 toTarget = targetPosition - transform_.translate;
		toTarget.y = 0.0f;
		Vector3 toHouse = housePos - transform_.translate;
		toHouse.y = 0.0f;
		float houseRadius = std::max({houseScale.x, houseScale.y, houseScale.z});
		float enemyRadius = std::max({transform_.scale.x, transform_.scale.y, transform_.scale.z});
		float houseAttackRange = attackRange_ + houseRadius + enemyRadius;
		float playerAttackStartRange = attackRange_ + enemyRadius;
		float targetAttackRange = isTargetingPlayer ? playerAttackStartRange : attackRange_;
		inAttackRange = Function::LengthSquared(toTarget) <= targetAttackRange * targetAttackRange || Function::LengthSquared(toHouse) <= houseAttackRange * houseAttackRange;
	}
	if (!isStun_ && !IsAttacking() && inAttackRange && IsAttackReady()) {
		enemyAttack_->Start(transform_);
		ResetAttackTimer();
	}

	enemyAttack_->Update();

	if (HP <= 0 && !isDying_) {
		StartDeathAnimation();
	}
}
void Enemy::Stun() {
	if (isDying_) {
		return;
	}
	isStun_ = true;
	stunTime = 0;
	if (enemyAttack_) {
		enemyAttack_->Cancel();
	}
	if (HP <= 0 && !isDying_) {
		StartDeathAnimation();
	}
}
void Enemy::Draw() {
	// 敵の描画処理
	object_->Draw();

	if (!isDying_ && enemyAttack_) {
		enemyAttack_->Draw();
	}

	if (!isDying_ && isStun_) {
		enemyStun->Draw();
	}
	if (!isDying_ && adhesion_) {
		adhesion_->Draw();
	}
}
void Enemy::StartDeathAnimation() {
	if (isDying_) {
		return;
	}
	isDying_ = true;
	deathTimer_ = 0.0f;
	isStun_ = false;
	isFinalComboBackStepping_ = false;
	finalComboBackStepTimer_ = 0.0f;
	velocity_ = {0.0f, 0.0f, 0.0f};
	if (enemyAttack_) {
		enemyAttack_->Cancel();
	}
	object_->SetColor(kDeathColor);
}
void Enemy::BulletCollision() {}
void Enemy::SetPosition(const Vector3& position) {
	transform_.translate = position;
	if (object_) {
		object_->SetTransform(transform_);
	}
	if (adhesion_) {
		adhesion_->SetTransform(transform_);
	}
}
void Enemy::SetIsStun(bool IsStun) { isStun_ = IsStun; }

float Enemy::GetAttackHitSize() const {
	if (enemyAttack_) {
		return enemyAttack_->GetHitSize();
	}
	return attackHitSize_;
}
void Enemy::ApplyFinalComboBackStep() {
	if (isDying_ || !isAlive) {
		return;
	}
	Vector3 forward = {std::sin(transform_.rotate.y), 0.0f, std::cos(transform_.rotate.y)};
	if (Function::LengthSquared(forward) < 0.0001f) {
		forward = {0.0f, 0.0f, 1.0f};
	}
	finalComboBackStepStart_ = transform_.translate;
	finalComboBackStepTarget_ = transform_.translate - Function::Normalize(forward) * finalComboBackStepDistance_;
	finalComboBackStepTimer_ = 0.0f;
	isFinalComboBackStepping_ = true;
	velocity_ = {0.0f, 0.0f, 0.0f};
	if (enemyAttack_) {
		enemyAttack_->Cancel();
	}
}
Vector3 Enemy::GetAttackPosition() const {
	if (enemyAttack_) {
		return enemyAttack_->GetPosition();
	}
	return transform_.translate;
}

bool Enemy::IsAttackHitActive() const { return enemyAttack_ && enemyAttack_->IsHitActive(); }

bool Enemy::ConsumeAttackHit() {
	if (!enemyAttack_) {
		return false;
	}
	return enemyAttack_->ConsumeHit();
}

bool Enemy::AddAdhesionAttribute(Attribute attribute) {
	if (adhesion_) {
		return adhesion_->AddAttribute(attribute);
	}
	return false;
}
Attribute Enemy::GetLastReactionPreviousAttribute() const { return adhesion_ ? adhesion_->GetLastReactionPreviousAttribute() : Attribute::None; }

Attribute Enemy::GetLastReactionAppliedAttribute() const { return adhesion_ ? adhesion_->GetLastReactionAppliedAttribute() : Attribute::None; }
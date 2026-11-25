#define NOMINMAX
#include "BaseEnemy.h"

#include "Function.h"
#include "GameBase.h"
#include "Object3d/Object3d.h"
#include "Parameter/EnemyParameter.h"
#include "Vector4.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace {
const Vector4 kDamageInvincibleColor{1.0f, 0.0f, 0.0f, 1.0f};
const Vector4 kDefaultColor{1.0f, 1.0f, 1.0f, 1.0f};
const Vector4 kDeathColor{1.0f, 0.2f, 0.2f, 1.0f};
} // namespace

BaseEnemy::BaseEnemy(std::string modelName, const Vector3& initialScale)
    : modelName_(std::move(modelName)), initialScale_(initialScale), object_(std::make_unique<Object3d>()), enemyStun_(std::make_unique<EnemyStun>()), enemyAttack_(std::make_unique<EnemyAttack>()),
      adhesion_(std::make_unique<Adhesion>()), hpBar_(std::make_unique<EnemyHPBar>()), lockOnEnemy_(std::make_unique<LockOnEnemy>()) {}

BaseEnemy::BaseEnemy(const EnemyParameter& config) : BaseEnemy(config.modelName, config.scale) {
	config.Validate();

	baseParameter_ = config.baseParameter;
	parameter_ = config.parameter;
	maxHP_ = static_cast<int>(config.parameter.HP);
	stunFramesMax_ = 60.0f * config.stunDuration;
	damageInvincibleDuration_ = config.damageInvincibleDuration;
	backStepDistance_ = config.backStepDistance;
	backStepDuration_ = config.backStepDuration;
	deathDuration_ = config.deathDuration;
	deathTargetRotateZ_ = config.deathRotateZ;
}

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::Initialize(Camera* camera, Vector3 position) {
	camera_ = camera;
	hp_ = maxHP_;
	isAlive_ = true;
	isDying_ = false;
	isStunned_ = false;
	stunFrames_ = 0;
	attackTimer_ = 0.0f;
	damageInvincibleTimer_ = 0.0f;
	lastSkillDamageId_ = -1;
	backStepTimer_ = 0.0f;
	isBackStepping_ = false;
	backStepStart_ = position;
	backStepTarget_ = position;
	deathTimer_ = 0.0f;

	transform_ = {
	    .scale = initialScale_,
	    .rotate = {0.0f, 0.0f, 0.0f},
	    .translate = position,
	};

	object_->Initialize();
	object_->SetModel(modelName_);
	InitializeModelAnimation(*object_);
	object_->SetCamera(camera_);
	object_->SetTransform(transform_);
	object_->SetOverlayEnabled(true);
	object_->SetOverlayAmount(0.0f);
	object_->SetOutlineColor({1.0f, 1.0f, 0.0f, 1.0f});
	object_->SetOutlineWidth(0.0f);
	object_->Update();
	object_->SetColor(kDefaultColor);

	enemyStun_->Initialize();
	enemyAttack_->Initialize(camera_);
	adhesion_->Initialize();
	adhesion_->SetCamera(camera_);

	hpBar_->SetCamera(camera_);
	hpBar_->SetMaxHP(maxHP_);
	hpBar_->SetHP(hp_);
	hpBar_->SetPosition(position);
	hpBar_->Initialize();

	lockOnEnemy_->SetCamera(camera_);
	lockOnEnemy_->SetPosition(position);
	lockOnEnemy_->Initialize();
}

void BaseEnemy::SetCamera(Camera* camera) {
	camera_ = camera;
	object_->SetCamera(camera);
	object_->UpdateCameraMatrices();
	adhesion_->SetCamera(camera);
	hpBar_->SetCamera(camera);
	hpBar_->Update();
	lockOnEnemy_->SetCamera(camera);
	enemyAttack_->SetCamera(camera);
}

void BaseEnemy::SetPosition(const Vector3& position) {
	transform_.translate = position;
	object_->SetTransform(transform_);
	adhesion_->SetTransform(transform_);
	lockOnEnemy_->SetPosition(position);
}

void BaseEnemy::SetLockOn(bool lockOn) {
	lockOnEnemy_->SetActive(lockOn);
	object_->SetOutlineWidth(lockOn ? 1.0f : 0.0f);
}

void BaseEnemy::MoveToward(const Vector3& target, float speed) {
	Vector3 offset = target - transform_.translate;
	offset.y = 0.0f;
	if (Function::LengthSquared(offset) <= 0.0001f) {
		return;
	}

	const Vector3 direction = Function::Normalize(offset);
	transform_.rotate.y = std::atan2(direction.x, direction.z);
	if (!IsAttacking()) {
		transform_.translate += direction * speed;
	}
}

void BaseEnemy::TryStartAttack(float cooldown) {
	if (isStunned_ || IsAttacking() || attackTimer_ < cooldown) {
		return;
	}
	enemyAttack_->Start(transform_);
	attackTimer_ = 0.0f;
}

void BaseEnemy::UpdatePresentation(bool updateAdhesion, float deltaTime, bool isMoving) {
	UpdateModelAnimation(deltaTime, isMoving);

	object_->SetCamera(camera_);
	object_->SetTransform(transform_);
	object_->Update();

	if (updateAdhesion) {
		adhesion_->SetCamera(camera_);
		adhesion_->SetTransform(transform_);
		adhesion_->Update();
	}

	hpBar_->SetCamera(camera_);
	hpBar_->SetHP(hp_);
	hpBar_->SetPosition(transform_.translate);
	hpBar_->Update();

	lockOnEnemy_->SetCamera(camera_);
	lockOnEnemy_->SetPosition(transform_.translate);
	lockOnEnemy_->Update();
}

void BaseEnemy::Update(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive) {
	if (!isAlive_) {
		return;
	}

	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();

	if (isDying_) {
		deathTimer_ += deltaTime;
		const float progress = std::clamp(deathTimer_ / deathDuration_, 0.0f, 1.0f);
		transform_.rotate.z = deathTargetRotateZ_ * progress;
		object_->SetColor(kDeathColor);
		enemyAttack_->Cancel();
		enemyAttack_->Update();
		UpdatePresentation(false, deltaTime, false);

		if (progress >= 1.0f) {
			isAlive_ = false;
			isDying_ = false;
		}
		return;
	}

	if (!IsAttacking()) {
		attackTimer_ += deltaTime;
	}

	damageInvincibleTimer_ = std::max(0.0f, damageInvincibleTimer_ - deltaTime);
	object_->SetColor(damageInvincibleTimer_ > 0.0f ? kDamageInvincibleColor : kDefaultColor);

	if (isBackStepping_) {
		backStepTimer_ += deltaTime;
		const float progress = std::clamp(backStepTimer_ / backStepDuration_, 0.0f, 1.0f);
		transform_.translate = Function::Lerp(backStepStart_, backStepTarget_, progress);
		enemyAttack_->Cancel();
		enemyAttack_->Update();

		if (progress >= 1.0f) {
			isBackStepping_ = false;
			backStepTimer_ = 0.0f;
		}
		UpdatePresentation(true, deltaTime, false);
		return;
	}

	const bool canMove = !isStunned_;
	if (isStunned_) {
		++stunFrames_;
		enemyStun_->SetCamera(camera_);
		enemyStun_->SetTranslate(transform_.translate);
		enemyStun_->Update();

		if (static_cast<float>(stunFrames_) >= stunFramesMax_) {
			isStunned_ = false;
		}
	}

	const Vector3 positionBeforeBehavior = transform_.translate;
	UpdateBehavior(housePosition, houseScale, playerPosition, isPlayerAlive, canMove);

	const bool isMoving = transform_.translate.x != positionBeforeBehavior.x || transform_.translate.z != positionBeforeBehavior.z;
	UpdatePresentation(true, deltaTime, isMoving);

	enemyAttack_->Update();

	if (hp_ <= 0) {
		StartDeathAnimation();
	}
}

void BaseEnemy::Draw() {
	object_->Draw();
	hpBar_->Draw();
	lockOnEnemy_->Draw();

	if (!isDying_) {
		enemyAttack_->Draw();
		if (isStunned_) {
			enemyStun_->Draw();
		}
		adhesion_->Draw();
	}
}

void BaseEnemy::SetHPSubtract(int damage) {
	if (!isAlive_ || isDying_) {
		return;
	}
	hp_ -= damage;
	if (hp_ <= 0) {
		StartDeathAnimation();
	}
}

void BaseEnemy::SetIsStun(bool stunned) { isStunned_ = stunned; }

void BaseEnemy::Stun() {
	if (!isAlive_ || isDying_) {
		return;
	}

	isStunned_ = true;
	stunFrames_ = 0;
	enemyAttack_->Cancel();

	if (hp_ <= 0) {
		StartDeathAnimation();
	}
}

void BaseEnemy::ApplyFinalComboBackStep() {
	if (!isAlive_ || isDying_) {
		return;
	}

	const Vector3 forward{std::sin(transform_.rotate.y), 0.0f, std::cos(transform_.rotate.y)};
	backStepStart_ = transform_.translate;
	backStepTarget_ = backStepStart_ - forward * backStepDistance_;
	backStepTimer_ = 0.0f;
	isBackStepping_ = true;
	enemyAttack_->Cancel();
}

void BaseEnemy::StartDeathAnimation() {
	if (isDying_) {
		return;
	}
	isDying_ = true;
	deathTimer_ = 0.0f;
	isStunned_ = false;
	isBackStepping_ = false;
	backStepTimer_ = 0.0f;
	enemyAttack_->Cancel();
	object_->SetColor(kDeathColor);
}

bool BaseEnemy::IsAttacking() const { return enemyAttack_->IsAttacking(); }
bool BaseEnemy::IsAttackHitActive() const { return enemyAttack_->IsHitActive(); }
bool BaseEnemy::ConsumeAttackHit() { return enemyAttack_->ConsumeHit(); }
Vector3 BaseEnemy::GetAttackPosition() const { return enemyAttack_->GetPosition(); }
float BaseEnemy::GetAttackHitSize() const { return enemyAttack_->GetHitSize(); }

bool BaseEnemy::AddAdhesionAttribute(Attribute attribute) { return adhesion_->AddAttribute(attribute); }

Attribute BaseEnemy::GetLastReactionPreviousAttribute() const { return adhesion_->GetLastReactionPreviousAttribute(); }

Attribute BaseEnemy::GetLastReactionAppliedAttribute() const { return adhesion_->GetLastReactionAppliedAttribute(); }
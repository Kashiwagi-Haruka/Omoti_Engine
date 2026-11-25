#pragma once

#include "Object/Characters/Base/CharacterParameters.h"
#include "Object/Characters/Enemy/Adhesion/Adhesion.h"
#include "Object/Characters/Enemy/EnemyAttack.h"
#include "Object/Characters/Enemy/EnemyStun.h"
#include "Object/Characters/Enemy/HPBar/EnemyHPBar.h"
#include "Object/Characters/Enemy/LockOn/LockOnEnemy.h"
#include "Transform.h"
#include "Vector3.h"

#include <memory>
#include <string>

class Camera;
class Object3d;
struct EnemyParameter;

class BaseEnemy {
public:
	virtual ~BaseEnemy();

	void Initialize(Camera* camera, Vector3 position);
	void Update(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive);
	void Draw();

	void SetCamera(Camera* camera);
	void SetPosition(const Vector3& position);
	void SetLockOn(bool lockOn);

	void SetHPSubtract(int damage);
	void SetIsStun(bool stunned);
	void Stun();
	void ApplyFinalComboBackStep();

	int GetHP() const { return hp_; }
	bool GetIsAlive() const { return isAlive_; }
	bool IsDying() const { return isDying_; }
	bool IsStunned() const { return isStunned_; }
	Vector3 GetPosition() const { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }

	bool CanTakeDamage() const { return damageInvincibleTimer_ <= 0.0f; }
	void TriggerDamageInvincibility() { damageInvincibleTimer_ = damageInvincibleDuration_; }
	int GetLastSkillDamageId() const { return lastSkillDamageId_; }
	void SetLastSkillDamageId(int id) { lastSkillDamageId_ = id; }

	bool IsAttacking() const;
	bool IsAttackHitActive() const;
	bool ConsumeAttackHit();
	Vector3 GetAttackPosition() const;
	float GetAttackHitSize() const;

	bool AddAdhesionAttribute(Attribute attribute);
	Attribute GetLastReactionPreviousAttribute() const;
	Attribute GetLastReactionAppliedAttribute() const;

	const BaseParameter& GetBaseParameter() const { return baseParameter_; }
	const Parameter& GetParameter() const { return parameter_; }

protected:
	BaseEnemy(std::string modelName, const Vector3& initialScale);
	explicit BaseEnemy(const EnemyParameter& config);

	virtual void UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) = 0;
	virtual void InitializeModelAnimation(Object3d&) {}
	virtual void UpdateModelAnimation(float, bool) {}
	void MoveToward(const Vector3& target, float speed);
	void TryStartAttack(float cooldown);

private:
	void StartDeathAnimation();
	void UpdatePresentation(bool updateAdhesion, float deltaTime, bool isMoving);

	std::string modelName_;
	Vector3 initialScale_;
	Transform transform_{};
	Camera* camera_ = nullptr;

	int hp_ = 1000;
	int maxHP_ = 1000;
	BaseParameter baseParameter_{10.0f, 0.0f, 10.0f};
	Parameter parameter_{};
	bool isAlive_ = true;
	bool isDying_ = false;
	bool isStunned_ = false;
	int stunFrames_ = 0;
	float stunFramesMax_ = 45.0f;

	float attackTimer_ = 0.0f;
	float damageInvincibleTimer_ = 0.0f;
	float damageInvincibleDuration_ = 0.3f;
	int lastSkillDamageId_ = -1;

	float backStepDistance_ = 5.0f;
	float backStepDuration_ = 0.18f;
	float backStepTimer_ = 0.0f;
	bool isBackStepping_ = false;
	Vector3 backStepStart_{};
	Vector3 backStepTarget_{};

	float deathTimer_ = 0.0f;
	float deathDuration_ = 0.45f;
	float deathTargetRotateZ_ = 1.57f;

	std::unique_ptr<Object3d> object_;
	std::unique_ptr<EnemyStun> enemyStun_;
	std::unique_ptr<EnemyAttack> enemyAttack_;
	std::unique_ptr<Adhesion> adhesion_;
	std::unique_ptr<EnemyHPBar> hpBar_;
	std::unique_ptr<LockOnEnemy> lockOnEnemy_;
};
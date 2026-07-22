#pragma once
#include "Object3d/Object3d.h"
#include "PlayerParameters.h"
#include "Weapon/PlayerSword.h"
#include "Attack/Skill/PlayerSkill.h"
#include "Object/Characters/Base/Attribute.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>
#include "Audio.h"
#include "PlayerModels.h"
#include "PlayerKey/PlayerKey.h"
#include "Attack/PlayerAttack.h"
class Camera;
class PlayerBullet;
class Field;
class Enemy;

class Player {

	enum class State {
		kIdle,
		kRunning,
		kJumping,
		kFalling,
		kAttacking

	};
	State state_;


	Parameters parameters_;
	std::unique_ptr<PlayerModels> models_;
	std::unique_ptr<PlayerAttack> attack_;
	struct Select {};

	float jumpTimer = 0.0f;
	int hp_; // プレイヤーHP

	bool isAlive;
	bool isInvincible_ = false;
	float invincibleTimer_ = 0.0f;
	bool damageTrigger_ = false;
	int pendingDamage_ = 0;

	bool isDash = false;
	bool isJump = false;
	bool isfalling = false;
	bool isAttack = false;
	Vector3 velocity_;
	Vector3 bulletVelocity_;

	Transform transform_;

	Camera* camera_;

	Field* map_ = nullptr;

	bool isSelect_;
	bool isLevelUP;
	float rotateTimer = 0.0f;

	bool usedAirAttack = false;

	// ダッシュ押したとき前に進む距離
	float dashDistance_ = 5.0f;

	// 最大ダッシュゲージ
	float dashGaugeMax_ = 240.0f;

	// 現在のダッシュゲージ
	float dashGauge_ = dashGaugeMax_;

	// ダッシュゲージの減少速度
	float dashGaugeDecrease_ = 1.0f;

	// ダッシュゲージの回復速度
	float dashGaugeRecovery_ = 0.5f;

	// ダッシュゲージが0になったか
	bool isDashGaugeRecovery_ = false;

Vector3 movementLimitCenter_{0.0f, 2.5f, 0.0f};
	float movementLimitRadius_ = 50.0f;
	bool attackApproachActive_ = false;
	Vector3 attackApproachTarget_{};
	float attackApproachStopDistance_ = 5.0f;
	float attackApproachRange_ = 10.0f;
	float attackApproachSpeed_ = 0.35f;
	Enemy* lockOnTarget_ = nullptr;
	// 攻撃中のみロックオン対象へ体の正面を維持する。
	void FaceLockOnTarget();

public:
	Player();
	~Player();
	void Initialize(Camera* camera);
	void Move();
	void Update();
	void Draw(bool drawOutline = true);
	void SetAttackApproachTarget(const Vector3& target);
	void SetLockOnTarget(Enemy* target) { lockOnTarget_ = target; }
	void ClearLockOnTarget() { lockOnTarget_ = nullptr; }
	Enemy* GetLockOnTarget() const { return lockOnTarget_; }
	void Jump();
	void Falling();
	PlayerSkill* GetSkill() { return attack_->GetSkill(); }

	void SetCamera(Camera* camera);
	void SetMap(Field* map) { map_ = map; }
	Vector3 GetPosition() { return transform_.translate; }
	Vector3 GetVelocity() { return velocity_; }
	bool GetIsAlive() { return isAlive; }
	bool IsDashing() const { return isDash; }
	bool GetIsSkillAttack() { return attack_->isSkillAttacking(); }
	Vector3 GetSkillPosition() { return attack_->GetSkillDamagePosition(); }
	Parameters GetParameters() { return parameters_; }
	void SetParameters(const Parameters& p) { parameters_ = p; }
	Vector3 GetRotate() { return transform_.rotate; }
	Vector3 GetScale() { return transform_.scale; }
	void Damage(int amount) {
		if (!isInvincible_) {
			pendingDamage_ = amount;
			isInvincible_ = true;
			invincibleTimer_ = 1.0f; // 1秒無敵
			damageTrigger_ = true;
		}
	}
	bool ConsumeDamageTrigger() {
		const bool triggered = damageTrigger_;
		damageTrigger_ = false;
		return triggered;
	}
	int ConsumePendingDamage() {
		const int damage = pendingDamage_;
		pendingDamage_ = 0;
		return damage;
	}
	bool GetSelect() { return isSelect_; };
	int GetHP() const { return hp_; }
	int GetHPMax() const { return parameters_.hpMax_; }
	float GetDashGauge() const { return dashGauge_; }
	float GetDashGaugeMax() const { return dashGaugeMax_; }
	Vector3 GetMovementLimitCenter() const { return movementLimitCenter_; }
	float GetMovementLimitRadius() const { return movementLimitRadius_; }
	void IsLevelUp(bool lv) { isLevelUP = lv; }
	bool GetLv() { return isLevelUP; }
	void EXPMath();
	PlayerSword* GetSword() { return attack_->GetSword(); }
	int GetComboStep() const { return attack_->GetComboStep(); }           // コンボ段階取得用
	bool IsFallingAttack() const { return attack_->IsFallingAttacking(); } // 落下攻撃中か
	Object3d* GetCharacterObject3d() { return models_ ? models_->GetCharacterObject3d() : nullptr; }
	void SetCharacterType(const std::string& characterName);
	Attribute GetCurrentAttribute() const;
	const BaseParameter& GetCurrentBaseParameter() const;
	const Parameter& GetCurrentCombatParameter() const;
};
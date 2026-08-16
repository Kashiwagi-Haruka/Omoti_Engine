#pragma once
#include "Audio.h"
#include "Camera.h"
#include "Object/Player/PlayerKey/PlayerKey.h"
#include "Object/Player/PlayerModels.h"
#include "Object/Player/Weapon/PlayerSword.h"
#include "Skill/PlayerSkill.h"
#include "Special/PlayerSpecialAttack.h"
#include <string>

class PlayerAttack {

	bool isfalling = false;
	bool isJump = false;
	enum class AttackState {
		kNone,          // 攻撃していない
		kWeakAttack1,   // 弱攻撃1
		kWeakAttack2,   // 弱攻撃2
		kWeakAttack3,   // 弱攻撃3
		kWeakAttack4,   // 弱攻撃4
		kStrongAttack,  // 重撃
		kSkillAttack,   // スキル技
		kSpecialAttack, // 必殺技
		kFallingAttack  // 落下攻撃
	};
	AttackState attackState_;

	// コンボ攻撃用
	int comboStep_ = 0;           // 現在のコンボ段階 (0〜4)
	float comboTimer_ = 0.0f;     // コンボ受付タイマー
	float comboWindow_ = 0.8f;    // コンボ受付時間（秒）
	bool isAttacking_ = false;    // 攻撃中フラグ
	bool canCombo_ = false;       // 次のコンボ入力可能フラグ
	bool nextComboInput_ = false; // 次のコンボ入力があったか

	// 重撃・落下攻撃用
	float attackHoldTimer_ = 0.0f;      // 長押し時間
	float heavyAttackThreshold_ = 0.3f; // 重撃判定時間（秒）
	bool isFallingAttack_ = false;      // 落下攻撃中フラグ
	bool nextHeavyAttackInput_ = false; // 重撃入力があったか

	// スキル攻撃用
	bool isSkillAttack = false;
	bool isSpecialAttack = false;

	// プレイヤーが移動できるか
	bool canMove_ = true;

	std::unique_ptr<PlayerSword> sword_;
	std::unique_ptr<PlayerSkill> skill_;
	std::unique_ptr<PlayerSpecialAttack> special_;
	std::unique_ptr<PlayerKey> key_;
	Camera* camera_ = nullptr;
	PlayerModels* models_ = nullptr;

	Transform playerTransform_;

public:
	PlayerAttack();
	~PlayerAttack();
	void Initialize();
	void SetAttackName(std::string AttackName);
	void Update();
	void UpdateAttachments();
	void Draw();
	void EndAttack();
	void ResetNormalAttackState();

	bool IsAttacking() const { return isAttacking_; }            // 攻撃中かどうかを返す関数
	bool IsFallingAttacking() const { return isFallingAttack_; } // 落下攻撃中かどうかを返す関数
	bool isSkillAttacking() const { return isSkillAttack; }      // スキル攻撃中かどうかを返す関数
	bool isSpecialAttacking() const { return isSpecialAttack; }  // 必殺技攻撃中かどうかを返す関数
	bool IsSpecialAnimationPlaying() const { return isSpecialAttack && !special_->IsAnimationFinished(); }
	bool IsAnyAttackActive() const { return isAttacking_ || isFallingAttack_ || isSkillAttack || IsSpecialAnimationPlaying(); }
	bool IsCanMove() const { return !isAttacking_ && !isFallingAttack_ && !isSkillAttack && !IsSpecialAnimationPlaying(); }
	// プレイヤーが移動できるかどうかを返す関数

	void SetIsFallingAttack(bool isFalling) { isFallingAttack_ = isFalling; }
	void SetAirState(bool isJumping, bool isFalling) {
		isJump = isJumping;
		isfalling = isFalling;
	}
	void SetAttacking(bool isAttacking) { isAttacking_ = isAttacking; }
	void SetCamera(Camera* camera);
	void SetTransform(const Transform& transform) { playerTransform_ = transform; }
	void SetModels(PlayerModels* models) { models_ = models; }

	int GetComboStep() const { return comboStep_; }
	PlayerSword* GetSword() { return sword_.get(); }
	PlayerSkill* GetSkill() { return skill_.get(); }
	PlayerSpecialAttack* GetSpecial() { return special_.get(); }
	Vector3 GetSkillDamagePosition() const { return skill_->GetDamagePosition(); }
};
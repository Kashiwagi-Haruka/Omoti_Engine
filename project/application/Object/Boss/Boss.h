#pragma once
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinCluster.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>
#include <vector>

class Camera;
class Object3d;

/// <summary>
/// ボス
/// </summary>
class Boss {
	/// <summary>
	/// アクション状態
	/// </summary>
	enum class ActionState {
		Idle,
		Charging,
		Attacking,
	};
	// HP
	int hp_ = 50; 
	// 最大HP
	int maxHp_ = 50;
	// 生存フラグ
	bool isAlive_ = true;
	// ダメージ無敵時間
	float damageInvincibleTimer_ = 0.0f; 
	// ダメージ無敵時間の長さ
	float damageInvincibleDuration_ = 0.5f; 
	// 最後にダメージを与えたスキルのID
	int lastSkillDamageId_ = -1;            

	// 変換情報
	Transform transform_{}; 
	// 基本位置
	Vector3 basePosition_{}; 
	// 基本スケール
	Vector3 baseScale_{};    
	// 速度
	Vector3 velocity_{};     
	// 最大速度
	float maxSpeed_ = 0.12f; 
	// プレイヤー追跡範囲
	float playerChaseRange_ = 10.0f; 

	// 攻撃タイマー
	float attackTimer_ = 0.0f; 
	// 攻撃クールダウン時間
	float attackCooldown_ = 2.0f; 
	// 攻撃アクティブタイマー
	float attackActiveTimer_ = 0.0f; 
	// 攻撃アクティブ時間
	float attackActiveDuration_ = 0.4f; 
	// 攻撃範囲
	float attackRange_ = 2.5f;          
	// 攻撃ヒットサイズ
	float attackHitSize_ = 1.2f;        
	// 攻撃ヒット前方オフセット
	float attackHitForwardOffset_ = 0.8f; 
	// チャージ攻撃ヒット幅
	float chargeAttackHitWidth_ = 1.8f;   
	// チャージ攻撃ヒット高さ
	float chargeAttackHitHeight_ = 1.4f;  
	// チャージ攻撃ヒット奥行き
	float chargeAttackHitDepth_ = 1.8f;   
	// 攻撃ダメージ
	int attackDamage_ = 1;                
	// チャージ攻撃ダメージ
	int chargeAttackDamage_ = 2;          
	// 攻撃ヒット消費フラグ
	bool attackHitConsumed_ = false;      
	// チャージタイマー
	float chargeTimer_ = 0.0f;            
	// チャージ時間
	float chargeDuration_ = 0.8f;         
	// 攻撃アニメーションタイマー
	float attackAnimationTimer_ = 0.0f;   
	// 攻撃アニメーション時間
	float attackAnimationDuration_ = 0.0f; 
	// チャージスピンタイマー
	float chargeSpinTimer_ = 0.0f;         
	// チャージスピン時間
	float chargeSpinDuration_ = 2.0f;      
	// チャージスピン速度
	float chargeSpinSpeed_ = 6.0f;         
	// 基本ヨー角
	float baseYaw_ = 0.0f;                 
	// チャージ攻撃中フラグ
	bool isChargeAttack_ = false;          
	// アクション状態
	ActionState actionState_ = ActionState::Idle; 

	// アニメーションタイマー
	float animationTimer_ = 0.0f; 
	// アニメーション時間
	float animationTime_ = 0.0f;  
	// 出現タイマー
	float appearTimer_ = 0.0f;    
	// 出現時間
	float appearDuration_ = 2.0f; 

	// アニメーションクリップ
	std::vector<Animation::AnimationData> animationClips_{}; 
	// 現在のアニメーションインデックス
	size_t currentAnimationIndex_ = 0;                       
	// アニメーションループフラグ
	bool animationLoop_ = true;                              

	// 3Dオブジェクト
	std::unique_ptr<Object3d> object_; 
	// スケルトン
	std::unique_ptr<Skeleton> skeleton_; 
	// スキンクラスター
	SkinCluster skinCluster_{};          
#ifdef _DEBUG
	// デバッグ用のボックスオブジェクト
	std::unique_ptr<Object3d> debugBox_; 
#endif // _DEBUG
	// カメラ
	Camera* camera_ = nullptr; 

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Boss();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Boss() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera"> カメラ </param>
	/// <param name="position"> 位置 </param>
	void Initialize(Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="housePos"> 家の位置 </param>
	/// <param name="playerPos"> プレイヤーの位置 </param>
	/// <param name="isPlayerAlive"> プレイヤーが生存しているかどうか </param>
	void Update(const Vector3& housePos, const Vector3& playerPos, bool isPlayerAlive);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// カメラを設定する
	/// </summary>
	/// <param name="camera"> カメラ </param>
	void SetCamera(Camera* camera) { camera_ = camera; }

	/// <summary>
	/// 生存状態を取得する
	/// </summary>
	/// <returns></returns>
	bool GetIsAlive() const { return isAlive_; }

	/// <summary>
	/// HPを取得する
	/// </summary>
	/// <returns></returns>
	int GetHP() const { return hp_; }

	/// <summary>
	/// 最大HPを取得する
	/// </summary>
	/// <returns></returns>
	int GetMaxHP() const { return maxHp_; }

	/// <summary>
	/// 位置を取得する
	/// </summary>
	/// <returns></returns>
	Vector3 GetPosition() const { return transform_.translate; }

	/// <summary>
	/// スケールを取得する
	/// </summary>
	/// <returns></returns>
	Vector3 GetScale() const { return transform_.scale; }

	/// <summary>
	/// 位置を設定する
	/// </summary>
	/// <param name="position"> 位置 </param>
	void SetPosition(const Vector3& position) { transform_.translate = position; }

	/// <summary>
	/// HPを減らす
	/// </summary>
	/// <param name="hp"> 減らすHP </param>
	void SetHPSubtract(int hp);

	/// <summary>
	/// ダメージを受け取れるかどうかを判定する
	/// </summary>
	/// <returns></returns>
	bool CanTakeDamage() const { return damageInvincibleTimer_ <= 0.0f; }

	/// <summary>
	/// ダメージ無敵状態を開始する
	/// </summary>
	void TriggerDamageInvincibility() { damageInvincibleTimer_ = damageInvincibleDuration_; }

	/// <summary>
	/// 最後のスキルダメージIDを取得する
	/// </summary>
	/// <returns></returns>
	int GetLastSkillDamageId() const { return lastSkillDamageId_; }

	/// <summary>
	/// 最後のスキルダメージIDを設定する
	/// </summary>
	/// <param name="skillDamageId"> スキルのID </param>
	void SetLastSkillDamageId(int skillDamageId) { lastSkillDamageId_ = skillDamageId; }

	/// <summary>
	/// 攻撃ヒットサイズを取得する
	/// </summary>
	/// <returns></returns>
	float GetAttackHitSize() const { return attackHitSize_; }

	/// <summary>
	/// 攻撃位置を取得する
	/// </summary>
	/// <returns></returns>
	Vector3 GetAttackPosition() const;

	/// <summary>
	/// 突進攻撃ヒットサイズを取得する
	/// </summary>
	/// <returns></returns>
	Vector3 GetChargeAttackHitSize() const { return {chargeAttackHitWidth_, chargeAttackHitHeight_, chargeAttackHitDepth_}; }

	/// <summary>
	/// 突進攻撃がアクティブかどうかを判定する
	/// </summary>
	/// <returns></returns>
	bool IsChargeAttackHitActive() const { return isChargeAttack_ && IsAttackHitActive(); }

	/// <summary>
	/// 攻撃のダメージ量を取得する
	/// </summary>
	/// <returns></returns>
	int GetAttackDamage() const { return isChargeAttack_ ? chargeAttackDamage_ : attackDamage_; }
	bool IsAttackHitActive() const { return attackActiveTimer_ > 0.0f; }
	bool ConsumeAttackHit();
};
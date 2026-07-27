#pragma once
#include "Engine/math/RigidBody.h"
#include "Vector3.h"
#include <string>
#include <vector>
class Enemy;
class EnemyManager;
class House;
class Player;
class Boss;

/// <summary>
/// 衝突判定の管理クラス
/// </summary>
class CollisionManager {
public:
	/// <summary>
	/// コライダーを追加する
	/// </summary>
	/// <param name="name"> コライダーの名前 </param>
	/// <param name="aabb"> AABBox </param>
	void AddColider(std::string name, AABB aabb);

	/// <summary>
	///	vectorコライダーを追加する
	/// </summary>
	/// <param name="name"> コライダーの名前 </param>
	/// <param name="aabb"> AABBox </param>
	void AddColider(std::string name, std::vector<AABB> aabb);

	/// <summary>
	/// 衝突判定を行う
	/// </summary>
	/// <param name="name"> コライダーの名前 </param>
	/// <param name="name2"> コライダーの名前 </param>
	/// <returns> 衝突している = true </returns>
	bool CheckCollision(std::string name, std::string name2);

	/// <summary>
	/// ゲームシーンの衝突判定を処理する
	/// </summary>
	/// <param name="player"> プレイヤー </param>
	/// <param name="enemyManager"> 敵マネージャー </param>
	/// <param name="house"> 家 </param>
	/// <param name="boss"> ボス </param>
	/// <param name="outHitEnemyPos"> 衝突した敵の位置 </param>
	/// <param name="outDidPlayerAttackHitEnemy"> プレイヤーの攻撃が敵に当たったかどうか </param>
	/// <param name="outNormalAttackHitEnemy"> 通常攻撃が命中した敵 </param>
	/// <returns></returns>
	bool HandleGameSceneCollisions(
	    Player& player, EnemyManager& enemyManager, House& house, Boss* boss, Vector3* outHitEnemyPos = nullptr, bool* outDidPlayerAttackHitEnemy = nullptr, Enemy** outNormalAttackHitEnemy = nullptr);
};
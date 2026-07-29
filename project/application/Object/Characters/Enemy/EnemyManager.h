#pragma once
#include "Enemy.h"
#include "EnemyHitEffect.h"
#include "Object/Damage/Damage.h"
#include <cstddef>
#include <memory>
#include <vector>
class EnemyManager {

public:
	// ウェーブの状態
	enum class WaveState {
		kWaiting,  // 次のウェーブ待機中
		kSpawning, // 敵生成中
		kActive,   // ウェーブ進行中
		kComplete  // ウェーブクリア
	};

private:
	struct HitEffectEntry {
		Enemy* enemy;
		std::unique_ptr<EnemyHitEffect> effect;
	};
	struct DamageTextEntry {
		Enemy* enemy;
		std::unique_ptr<Damage> damageText;
		Vector3 targetOffset;
	};
	std::vector<std::unique_ptr<Enemy>> enemies;
	std::vector<HitEffectEntry> hitEffects;
	std::vector<DamageTextEntry> damageTexts;
	Camera* camera_ = nullptr;
	int nextDamageTextOffsetIndex_ = 0;

	// ウェーブシステム
	int currentWave_ = 0;        // 現在のウェーブ番号
	WaveState waveState_;        // ウェーブの状態
	float waveTimer_ = 0.0f;     // ウェーブタイマー
	float waveDelay_ = 3.0f;     // ウェーブ間の待機時間（秒）
	int totalEnemiesKilled_ = 0; // 倒した敵の総数
	bool allWavesComplete_ = false;
	int maxWave_ = 5;
	std::vector<Vector3> pendingSpawnPositions_;
	std::size_t nextSpawnIndex_ = 0;

public:
	EnemyManager() {}
	~EnemyManager() = default;

	void Initialize(Camera* camera);
	void AddEnemy(Camera* camera, const Vector3& pos);
	void Update(Camera* camera, const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive, bool isMovementPaused = false);
	void SetCamera(Camera* camera);	
	void Draw();
	void Clear();
	void OnEnemyDamaged(Enemy* enemy, int damage = 1, Attribute attribute = Attribute::None, bool isCritical = false, Attribute reactionPreviousAttribute = Attribute::None);
	// ウェーブシステム関連
	void StartNextWave();        // 次のウェーブを開始
	void SpawnWaveEnemies();     // ウェーブに応じた敵の生成位置を準備
	void SpawnNextQueuedEnemy(); // 準備済みの敵を1フレームに1体生成
	void CheckWaveComplete();    // ウェーブクリア判定
	void ResolveOverlaps(const Vector3& playerPos, bool isPlayerAlive);
	void ResolveDamageTextOverlaps();

	// ゲッター
	int GetCurrentWave() const { return currentWave_; }
	WaveState GetWaveState() const { return waveState_; }
	int GetAliveEnemyCount() const; // 生存している敵の数
	int GetTotalEnemiesKilled() const { return totalEnemiesKilled_; }
	bool IsWaveActive() const;     // ウェーブ進行中か
	bool IsWaveComplete() const;   // ウェーブクリアしたか
	float GetWaveProgress() const; // ウェーブの進行度（0.0～1.0）
	bool AreAllWavesComplete() const { return allWavesComplete_; }
	int GetMaxWave() const { return maxWave_; }

	// セッター
	void SetWaveDelay(float delay) { waveDelay_ = delay; }
	void ForceStartWave(int waveNumber);
	void ForceBossPhase();

	// 敵リストへのアクセス（当たり判定用など）
	std::vector<std::unique_ptr<Enemy>>& GetEnemies() { return enemies; }
};
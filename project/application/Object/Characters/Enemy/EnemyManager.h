#pragma once

#include "EnemyFactory.h"
#include "EnemyHitEffect.h"
#include "Object/Damage/Damage.h"
#include "StageEnemyLayout.h"

#include <cstddef>
#include <memory>
#include <vector>

class EnemyManager {
public:
	enum class WaveState {
		kWaiting,
		kSpawning,
		kActive,
		kComplete,
	};

	EnemyManager() = default;
	~EnemyManager() = default;

	void Initialize(Camera* camera);
	void AddEnemy(Camera* camera, const StageEnemySpawn& spawn);
	void Update(Camera* camera, const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive, bool isMovementPaused = false);
	void SetCamera(Camera* camera);
	void Draw();
	void Clear();

	void OnEnemyDamaged(BaseEnemy* enemy, int damage = 1, Attribute attribute = Attribute::None, bool isCritical = false, Attribute reactionPreviousAttribute = Attribute::None);

	void StartNextWave();
	void SpawnWaveEnemies();
	void SpawnNextQueuedEnemy();
	void CheckWaveComplete();
	void ResolveOverlaps(const Vector3& playerPos, bool isPlayerAlive);
	void ResolveDamageTextOverlaps();

	int GetCurrentWave() const { return currentWave_; }
	WaveState GetWaveState() const { return waveState_; }
	int GetAliveEnemyCount() const;
	int GetTotalEnemiesKilled() const { return totalEnemiesKilled_; }
	bool IsWaveActive() const;
	bool IsWaveComplete() const;
	float GetWaveProgress() const;
	bool AreAllWavesComplete() const { return allWavesComplete_; }
	int GetMaxWave() const { return maxWave_; }

	void SetWaveDelay(float delay) { waveDelay_ = delay; }
	void ForceStartWave(int waveNumber);
	void ForceBossPhase();

	std::vector<std::unique_ptr<BaseEnemy>>& GetEnemies() { return enemies; }

private:
	struct HitEffectEntry {
		BaseEnemy* enemy;
		std::unique_ptr<EnemyHitEffect> effect;
	};

	struct DamageTextEntry {
		BaseEnemy* enemy;
		std::unique_ptr<Damage> damageText;
		Vector3 targetOffset;
	};

	std::vector<std::unique_ptr<BaseEnemy>> enemies;
	std::vector<HitEffectEntry> hitEffects;
	std::vector<DamageTextEntry> damageTexts;
	Camera* camera_ = nullptr;
	int nextDamageTextOffsetIndex_ = 0;

	int currentWave_ = 0;
	WaveState waveState_ = WaveState::kWaiting;
	float waveTimer_ = 0.0f;
	float waveDelay_ = 3.0f;
	int totalEnemiesKilled_ = 0;
	bool allWavesComplete_ = false;
	static constexpr int maxWave_ = kStagePhaseCount;

	StageEnemyLayout stageLayout_{};
	EnemyParameter brainParameters_{};
	BrainStealer::SpecificParameter brainSpecificParameters_{};
	std::size_t nextSpawnIndex_ = 0;
};
#define NOMINMAX
#include "EnemyManager.h"
#include "Function.h"
#include "Object3d/Object3dCommon.h"
#include "Model/ModelManager.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>

namespace {
constexpr int kFinalWave = 5;
constexpr float kEnemySeparationRadius = 1.0f;
constexpr float kPlayerSeparationRadius = 1.0f;
constexpr float kDamageTextSeparationRadius = 0.45f;
constexpr float kDamageTextBaseScaleRate = 0.18f;
constexpr float kDamageTextMinScale = 0.45f;
constexpr float kDamageTextUpOffsetRate = 0.75f;
constexpr float kDamageTextUpOffsetExtra = -1.0f;
constexpr int kDamageTextTargetOffsetCount = 12;
constexpr std::array<Vector3, kDamageTextTargetOffsetCount> kDamageTextTargetOffsets = {
    Vector3{0.0f,   0.0f,  0.0f  },
    Vector3{0.32f,  0.18f, 0.0f  },
    Vector3{-0.32f, 0.36f, 0.0f  },
    Vector3{0.0f,   0.54f, 0.32f },
    Vector3{0.0f,   0.24f, -0.32f},
    Vector3{0.36f,  0.48f, 0.36f },
    Vector3{-0.36f, 0.12f, -0.36f},
    Vector3{0.48f,  0.66f, -0.24f},
    Vector3{-0.48f, 0.30f, 0.24f },
    Vector3{0.20f,  0.78f, 0.48f },
    Vector3{-0.20f, 0.06f, -0.48f},
    Vector3{0.0f,   0.90f, 0.0f  },
};
constexpr float kSeparationEpsilon = 0.0001f;
constexpr int kSeparationIterations = 4;

float LengthSquaredXZ(const Vector3& vector) { return vector.x * vector.x + vector.z * vector.z; }

Vector3 MakeDamageTextPosition(const Enemy& enemy, const Vector3& targetOffset) {
	Vector3 position = enemy.GetPosition();
	const Vector3 scale = enemy.GetScale();
	const float horizontalScale = std::max(scale.x, scale.z);
	position.x += targetOffset.x * horizontalScale;
	position.y += scale.y * kDamageTextUpOffsetRate + kDamageTextUpOffsetExtra + targetOffset.y * scale.y;
	position.z += targetOffset.z * horizontalScale;
	return position;
}

Vector3 MakeDamageTextScale(const Enemy& enemy) {
	const Vector3 targetScale = enemy.GetScale();
	const float textScale = std::max(kDamageTextMinScale, std::max({targetScale.x, targetScale.y, targetScale.z}) * kDamageTextBaseScaleRate);
	return {textScale, textScale, textScale};
}

void PushAwayXZ(Vector3& position, const Vector3& otherPosition, float minDistance, const Vector3& fallbackDirection) {
	Vector3 delta = position - otherPosition;
	delta.y = 0.0f;
	float distanceSq = LengthSquaredXZ(delta);

	if (distanceSq >= minDistance * minDistance) {
		return;
	}

	if (distanceSq <= kSeparationEpsilon) {
		delta = fallbackDirection;
		delta.y = 0.0f;
		distanceSq = LengthSquaredXZ(delta);
	}

	if (distanceSq <= kSeparationEpsilon) {
		delta = {1.0f, 0.0f, 0.0f};
		distanceSq = 1.0f;
	}

	const float distance = std::sqrt(distanceSq);
	const float pushDistance = minDistance - distance;
	position.x += (delta.x / distance) * pushDistance;
	position.z += (delta.z / distance) * pushDistance;
}
} // namespace
void EnemyManager::Clear() {
	enemies.clear(); // unique_ptr が自動削除
	hitEffects.clear();
	damageTexts.clear();
	pendingSpawnPositions_.clear();
	nextSpawnIndex_ = 0;
	nextDamageTextOffsetIndex_ = 0;
}
void EnemyManager::Initialize(Camera* camera) {
	Clear();
	camera_ = camera;

	// ウェーブシステムの初期化
	currentWave_ = 0;
	waveState_ = WaveState::kWaiting;
	waveTimer_ = 0.0f;
	waveDelay_ = 3.0f; // ウェーブ間の待機時間（秒）
	totalEnemiesKilled_ = 0;
	allWavesComplete_ = false;
	ModelManager::GetInstance()->LoadModel("Resources/3d", "EnemyStun");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "EnemyAttack"); // 鎌モデル
	// 最初のウェーブを開始
	StartNextWave();
}

void EnemyManager::StartNextWave() {
	if (allWavesComplete_) {
		return;
	}
	currentWave_++;
	waveState_ = WaveState::kSpawning;
	waveTimer_ = 0.0f;

	// 現在のウェーブに基づいて敵の生成位置を準備
	SpawnWaveEnemies();
}

void EnemyManager::SpawnWaveEnemies() {

	// ウェーブごとの設定
	struct WaveConfig {
		int enemyCount;    // 敵の数
		float startX;      // 開始X位置
		float endX;        // 終了X位置
		float minY;        // 最小Y位置
		float minZ;        // Z最小
		float maxZ;        // Z最大
	};

	WaveConfig config;

		switch (currentWave_) {
	case 1: // ウェーブ1: 少数、低い位置、広い間隔5
		config = {5, -40.0f, 0.0f, 1.5f,-60.0f, -20.0f};
		break;

	case 2: // ウェーブ2: 中数、やや高い位置8
		config = {8, -40.0f, 0.0f, 1.5f,-60.0f, -20.0f};
		break;

	case 3: // ウェーブ3: 多数、バラバラの高さ12
		config = {12, -40.0f, 0.0f, 1.5f, -60.0f, -20.0f};
		break;

	case 4: // ウェーブ4: 密集、高低差大15
		config = {15, -40.0f, 0.0f, 1.5f,-60.0f, -20.0f};
		break;

	case 5: // ウェーブ5: 大量、ランダム配置20
		config = {20, -40.0f, 0.0f, 1.5f, -60.0f, -20.0f};
		break;

	default: // ウェーブ6以降: どんどん増える
		config = {
		    15 + (currentWave_ - 5) * 3, // 徐々に増加
		    10.0f,
		    0.0f + (currentWave_ - 5) * 5.0f,
		    1.0f,
		    -60.0f,
		    -20.0f};
		break;
	}

	// 敵の生成位置を準備
	float spacing = (config.endX - config.startX) / config.enemyCount;
	const float minDistance = 2.0f;
	const float minDistanceSq = minDistance * minDistance;
	const int maxAttempts = 40;
	pendingSpawnPositions_.clear();
	nextSpawnIndex_ = 0;
	std::vector<Vector3>& spawnPositions = pendingSpawnPositions_;
	spawnPositions.reserve(config.enemyCount);

	for (int i = 0; i < config.enemyCount; i++) {
		Vector3 pos = {};
		bool placed = false;
		const float baseX = config.startX + i * spacing;

		for (int attempt = 0; attempt < maxAttempts; ++attempt) {
			float x = baseX;
			if (attempt > 0) {
				x = config.startX + ((float)rand() / RAND_MAX) * (config.endX - config.startX);
			}

			float y = config.minY;

			// ランダムなZ位置のバリエーション
			float z = config.minZ + ((float)rand() / RAND_MAX) * (config.maxZ - config.minZ);

			Vector3 candidate = {x, y, z};
			bool overlaps = false;
			for (const auto& existing : spawnPositions) {
				Vector3 delta = candidate - existing;
				float distanceSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
				if (distanceSq < minDistanceSq) {
					overlaps = true;
					break;
				}
			}

			if (!overlaps) {
				pos = candidate;
				placed = true;
				break;
			}
		}

		if (!placed) {
			pos = {baseX, config.minY, (config.minZ + config.maxZ) * 0.5f};
		}

		spawnPositions.push_back(pos);
	}
}

void EnemyManager::SpawnNextQueuedEnemy() {
	if (nextSpawnIndex_ >= pendingSpawnPositions_.size()) {
		waveState_ = WaveState::kActive;
		return;
	}

	AddEnemy(camera_, pendingSpawnPositions_[nextSpawnIndex_]);
	++nextSpawnIndex_;
	if (nextSpawnIndex_ >= pendingSpawnPositions_.size()) {
		waveState_ = WaveState::kActive;
	}
}

void EnemyManager::AddEnemy(Camera* camera, const Vector3& pos) {
	auto e = std::make_unique<Enemy>();
	e->Initialize(camera, pos);
	e->SetCamera(camera);
	Enemy* enemyPtr = e.get();
	enemies.push_back(std::move(e));

	auto hitEffect = std::make_unique<EnemyHitEffect>();
	hitEffect->SetCamera(camera);
	hitEffect->Initialize();
	hitEffects.push_back({enemyPtr, std::move(hitEffect)});
}
void EnemyManager::Update(Camera* camera, const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive) {

	// ウェーブの状態管理
	switch (waveState_) {
	case WaveState::kWaiting:
		// 待機中（次のウェーブまでの猶予時間）
		waveTimer_ += 1.0f / 60.0f;
		if (waveTimer_ >= waveDelay_) {
			StartNextWave();
		}
		break;

	case WaveState::kSpawning:
		// 敵生成中：1フレームにつき1体だけ生成
		SpawnNextQueuedEnemy();
		break;

	case WaveState::kActive:
		// ウェーブ進行中：全滅チェック
		CheckWaveComplete();
		break;

	case WaveState::kComplete:
		// ウェーブクリア：次のウェーブへ
		if (!allWavesComplete_) {
			waveState_ = WaveState::kWaiting;
			waveTimer_ = 0.0f;
		}
		break;

	default:
		break;
	}

		// 敵の更新
	for (auto& e : enemies) {
		if (e->GetIsAlive() || e->IsDying()) {
			e->SetCamera(camera);
			e->Update(housePos, houseScale, playerPos, isPlayerAlive);
		}
	}
	ResolveOverlaps(playerPos, isPlayerAlive);

	for (auto& entry : hitEffects) {
		if (entry.enemy && entry.enemy->GetIsAlive()) {
			entry.effect->SetPosition(entry.enemy->GetPosition());
		}
		entry.effect->Update();
	}

	for (auto& entry : damageTexts) {
		if (entry.enemy) {
			entry.damageText->SetPosition(MakeDamageTextPosition(*entry.enemy, entry.targetOffset));
		}
	}
	ResolveDamageTextOverlaps();
	for (auto& entry : damageTexts) {
		entry.damageText->Update();
	}
	damageTexts.erase(std::remove_if(damageTexts.begin(), damageTexts.end(), [](const DamageTextEntry& entry) { return !entry.damageText || !entry.damageText->IsVisible(); }), damageTexts.end());
}
void EnemyManager::SetCamera(Camera* camera) {
	camera_ = camera;
	for (const auto& enemy : enemies) {
		if (enemy) {
			enemy->SetCamera(camera_);
		}
	}
}
void EnemyManager::ResolveDamageTextOverlaps() {
	for (size_t i = 0; i < damageTexts.size(); ++i) {
		Damage* damageA = damageTexts[i].damageText.get();
		if (!damageA || !damageA->IsVisible()) {
			continue;
		}

		Vector3 positionA = damageA->GetPosition();
		for (size_t j = i + 1; j < damageTexts.size(); ++j) {
			Damage* damageB = damageTexts[j].damageText.get();
			if (!damageB || !damageB->IsVisible()) {
				continue;
			}

			Vector3 positionB = damageB->GetPosition();
			Vector3 delta = positionB - positionA;
			const float distanceSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
			if (distanceSq >= kDamageTextSeparationRadius * kDamageTextSeparationRadius) {
				continue;
			}

			float distance = std::sqrt(distanceSq);
			if (distanceSq <= kSeparationEpsilon) {
				const float angle = static_cast<float>(i + j + 1) * 2.399963f;
				delta = {std::cos(angle), 0.0f, std::sin(angle)};
				distance = 0.0f;
			}

			const float pushDistance = (kDamageTextSeparationRadius - distance) * 0.5f;
			const float invDistance = distance > kSeparationEpsilon ? 1.0f / distance : 1.0f;
			const Vector3 push = {delta.x * invDistance * pushDistance, delta.y * invDistance * pushDistance, delta.z * invDistance * pushDistance};
			positionA -= push;
			positionB += push;
			damageB->SetPosition(positionB);
		}
		damageA->SetPosition(positionA);
	}
}

void EnemyManager::ResolveOverlaps(const Vector3& playerPos, bool isPlayerAlive) {
	const float enemyMinDistance = kEnemySeparationRadius * 2.0f;
	const float playerMinDistance = kEnemySeparationRadius + kPlayerSeparationRadius;

	for (int iteration = 0; iteration < kSeparationIterations; ++iteration) {
		for (size_t i = 0; i < enemies.size(); ++i) {
			Enemy* enemyA = enemies[i].get();
			if (!enemyA || !enemyA->GetIsAlive() || enemyA->IsDying()) {
				continue;
			}

			Vector3 enemyAPosition = enemyA->GetPosition();

			if (isPlayerAlive) {
				PushAwayXZ(enemyAPosition, playerPos, playerMinDistance, enemyAPosition - playerPos);
			}

			for (size_t j = i + 1; j < enemies.size(); ++j) {
				Enemy* enemyB = enemies[j].get();
				if (!enemyB || !enemyB->GetIsAlive() || enemyB->IsDying()) {
					continue;
				}

				Vector3 enemyBPosition = enemyB->GetPosition();
				Vector3 delta = enemyAPosition - enemyBPosition;
				delta.y = 0.0f;
				float distanceSq = LengthSquaredXZ(delta);

				if (distanceSq >= enemyMinDistance * enemyMinDistance) {
					continue;
				}

				if (distanceSq <= kSeparationEpsilon) {
					float angle = static_cast<float>(i + j + 1) * 2.399963f;
					delta = {std::cos(angle), 0.0f, std::sin(angle)};
					distanceSq = 1.0f;
				}

				const float distance = std::sqrt(distanceSq);
				const float pushDistance = (enemyMinDistance - distance) * 0.5f;
				const Vector3 push = {delta.x / distance * pushDistance, 0.0f, delta.z / distance * pushDistance};
				enemyAPosition += push;
				enemyBPosition -= push;

				if (isPlayerAlive) {
					PushAwayXZ(enemyBPosition, playerPos, playerMinDistance, enemyBPosition - playerPos);
				}

				enemyB->SetPosition(enemyBPosition);
			}

			if (isPlayerAlive) {
				PushAwayXZ(enemyAPosition, playerPos, playerMinDistance, enemyAPosition - playerPos);
			}
			enemyA->SetPosition(enemyAPosition);
		}
	}
}

void EnemyManager::CheckWaveComplete() {
	// 生存している敵の数をカウント
	int aliveCount = 0;
	for (auto& e : enemies) {
		if (e->GetIsAlive()) {
			aliveCount++;
		}
	}

	// 全滅したらウェーブクリア
	if (aliveCount == 0 && !enemies.empty()) {
		waveState_ = WaveState::kComplete;
		totalEnemiesKilled_ += static_cast<int>(enemies.size());
		if (currentWave_ >= kFinalWave) {
			allWavesComplete_ = true;
		}

		// 倒した敵をクリア
		hitEffects.clear();
		for (auto& entry : damageTexts) {
			entry.enemy = nullptr;
		}
		enemies.clear();
	}
}

void EnemyManager::Draw() {
	for (auto& e : enemies) {
		if (e->GetIsAlive() || e->IsDying()) {
			e->Draw();
		}
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	for (auto& entry : hitEffects) {
		entry.effect->Draw();
	}
	for (auto& entry : damageTexts) {
		entry.damageText->Draw();
	}
	Object3dCommon::GetInstance()->DrawCommon();
}

void EnemyManager::OnEnemyDamaged(Enemy* enemy, int damage, Attribute attribute, bool isCritical, Attribute reactionPreviousAttribute) {
	for (auto& entry : hitEffects) {
		if (entry.enemy == enemy) {
			entry.effect->Activate(enemy->GetPosition());
			break;
		}
	}

	auto damageText = std::make_unique<Damage>();
	damageText->Initialize(camera_);
	const Vector3 targetOffset = kDamageTextTargetOffsets[nextDamageTextOffsetIndex_ % kDamageTextTargetOffsets.size()];
	nextDamageTextOffsetIndex_ = (nextDamageTextOffsetIndex_ + 1) % static_cast<int>(kDamageTextTargetOffsets.size());
	damageText->SetPosition(MakeDamageTextPosition(*enemy, targetOffset));
	damageText->SetScale(MakeDamageTextScale(*enemy));
	if (reactionPreviousAttribute != Attribute::None && reactionPreviousAttribute != attribute) {
		damageText->SetReactionAttribute(reactionPreviousAttribute, attribute);
	} else {
		damageText->SetAttribute(attribute);
	}
	damageText->SetIsCritical(isCritical);
	damageText->SetDamageValue(damage);
	damageTexts.push_back({enemy, std::move(damageText), targetOffset});
}
void EnemyManager::ForceStartWave(int waveNumber) {
	Clear();
	allWavesComplete_ = false;
	waveTimer_ = 0.0f;
	currentWave_ = std::clamp(waveNumber, 1, maxWave_);
	waveState_ = WaveState::kSpawning;
	SpawnWaveEnemies();
}

void EnemyManager::ForceBossPhase() {
	Clear();
	allWavesComplete_ = true;
	waveTimer_ = 0.0f;
	currentWave_ = maxWave_;
	waveState_ = WaveState::kComplete;
}

int EnemyManager::GetAliveEnemyCount() const {
	int count = 0;
	for (const auto& e : enemies) {
		if (e->GetIsAlive()) {
			count++;
		}
	}
	return count;
}

bool EnemyManager::IsWaveActive() const { return waveState_ == WaveState::kActive; }

bool EnemyManager::IsWaveComplete() const { return waveState_ == WaveState::kComplete; }

float EnemyManager::GetWaveProgress() const {
	if (enemies.empty())
		return 1.0f;

	int aliveCount = GetAliveEnemyCount();
	int totalCount = static_cast<int>(enemies.size());

	return 1.0f - ((float)aliveCount / totalCount);
}
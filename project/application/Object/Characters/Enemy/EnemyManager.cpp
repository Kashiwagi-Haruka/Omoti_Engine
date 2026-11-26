#define NOMINMAX
#include "EnemyManager.h"
#include "GameBase.h"
#include "Function.h "
#include "Object3d/Object3dCommon.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
constexpr float kEnemySeparationRadius = 3.0f;
constexpr float kPlayerSeparationRadius = 1.0f;
constexpr float kDamageTextSeparationRadius = 0.45f;
constexpr float kDamageTextBaseScaleRate = 0.18f;
constexpr float kDamageTextMinScale = 0.45f;
constexpr float kDamageTextUpOffsetRate = 0.75f;
constexpr float kDamageTextUpOffsetExtra = -1.0f;
constexpr float kSeparationEpsilon = 0.0001f;
constexpr int kSeparationIterations = 4;

constexpr std::array<Vector3, 12> kDamageTextTargetOffsets{
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
    Vector3{-0.20f, 0.06f, -0.32f},
    Vector3{0.0f,   0.90f, 0.0f  },
};

float LengthSquaredXZ(const Vector3& value) { return value.x * value.x + value.z * value.z; }

Vector3 MakeDamageTextPosition(const BaseEnemy& enemy, const Vector3& targetOffset) {
	Vector3 position = enemy.GetPosition();
	const Vector3 scale = enemy.GetScale();
	const float horizontalScale = std::max(scale.x, scale.z);

	position.x += targetOffset.x * horizontalScale;
	position.y += scale.y * kDamageTextUpOffsetRate + kDamageTextUpOffsetExtra + targetOffset.y * scale.y;
	position.z += targetOffset.z * horizontalScale;
	return position;
}

Vector3 MakeDamageTextScale(const BaseEnemy& enemy) {
	const Vector3 targetScale = enemy.GetScale() * 2.0f;
	const float largestDimension = std::max({
	    targetScale.x,
	    targetScale.y,
	    targetScale.z,
	});
	const float textScale = std::max(kDamageTextMinScale, largestDimension * kDamageTextBaseScaleRate);
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
	hitEffects.clear();
	damageTexts.clear();
	enemies.clear();
	nextSpawnIndex_ = 0;
	nextDamageTextOffsetIndex_ = 0;
}

void EnemyManager::Initialize(Camera* camera) {
	auto layout = LoadStageEnemyLayout(kStage1EnemyPath);
	const bool needsBrain = std::any_of(
	    layout.begin(), layout.end(), [](const auto& phase) { return std::any_of(phase.begin(), phase.end(), [](const StageEnemySpawn& spawn) { return spawn.type == EnemyType::BrainStealer; }); });

	EnemyParameter common;
	BrainStealer::SpecificParameter specific;
	if (needsBrain) {
		std::string error;
		if (!BrainStealer::LoadParameter(common, specific, error)) {
			throw std::runtime_error(error);
		}
	}

	Clear();
	camera_ = camera;
	stageLayout_ = std::move(layout);
	brainParameters_ = std::move(common);
	brainSpecificParameters_ = specific;
	currentWave_ = 0;
	waveState_ = WaveState::kWaiting;
	waveTimer_ = 0.0f;
	waveDelay_ = 3.0f;
	totalEnemiesKilled_ = 0;
	allWavesComplete_ = false;
	StartNextWave();
}

void EnemyManager::StartNextWave() {
	if (allWavesComplete_) {
		return;
	}

	++currentWave_;
	waveState_ = WaveState::kSpawning;
	waveTimer_ = 0.0f;
	SpawnWaveEnemies();
}

void EnemyManager::SpawnWaveEnemies() { nextSpawnIndex_ = 0; }

void EnemyManager::SpawnNextQueuedEnemy() {
	const auto& phase = stageLayout_.at(static_cast<std::size_t>(currentWave_ - 1));

	if (nextSpawnIndex_ < phase.size()) {
		AddEnemy(camera_, phase[nextSpawnIndex_]);
		++nextSpawnIndex_;
	}

	if (nextSpawnIndex_ == phase.size()) {
		waveState_ = WaveState::kActive;
	}
}

void EnemyManager::AddEnemy(Camera* camera, const StageEnemySpawn& spawn) {
	auto enemy = EnemyFactory::Create(spawn.type, brainParameters_, brainSpecificParameters_);
	enemy->Initialize(camera, spawn.position);
	enemy->SetCamera(camera);

	auto hitEffect = std::make_unique<EnemyHitEffect>();
	hitEffect->SetCamera(camera);
	hitEffect->Initialize();

	BaseEnemy* enemyPtr = enemy.get();
	enemies.push_back(std::move(enemy));

	try {
		hitEffects.push_back({enemyPtr, std::move(hitEffect)});
	} catch (...) {
		enemies.pop_back();
		throw;
	}
}

void EnemyManager::Update(Camera* camera, const Vector3& housePos, const Vector3& houseScale, const Vector3& playerPos, bool isPlayerAlive, bool isMovementPaused) {
	switch (waveState_) {
	case WaveState::kWaiting:
		waveTimer_ += GameBase::GetInstance()->GetDeltaTime();
		if (waveTimer_ >= waveDelay_) {
			StartNextWave();
		}
		break;

	case WaveState::kSpawning:
		SpawnNextQueuedEnemy();
		break;

	case WaveState::kActive:
		CheckWaveComplete();
		break;

	case WaveState::kComplete:
		if (!allWavesComplete_) {
			waveState_ = WaveState::kWaiting;
			waveTimer_ = 0.0f;
		}
		break;
	}

	if (!isMovementPaused) {
		for (const auto& enemy : enemies) {
			if (enemy->GetIsAlive() || enemy->IsDying()) {
				enemy->SetCamera(camera);
				enemy->Update(housePos, houseScale, playerPos, isPlayerAlive);
			}
		}
		ResolveOverlaps(playerPos, isPlayerAlive);
	}

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

	for (const auto& entry : hitEffects) {
		if (entry.effect) {
			entry.effect->SetCamera(camera_);
		}
	}

	for (const auto& entry : damageTexts) {
		if (entry.damageText) {
			entry.damageText->SetCamera(camera_);
		}
	}
}

void EnemyManager::ResolveDamageTextOverlaps() {
	const float separationRadiusSq = kDamageTextSeparationRadius * kDamageTextSeparationRadius;

	for (std::size_t i = 0; i < damageTexts.size(); ++i) {
		Damage* damageA = damageTexts[i].damageText.get();
		if (!damageA || !damageA->IsVisible()) {
			continue;
		}

		Vector3 positionA = damageA->GetPosition();

		for (std::size_t j = i + 1; j < damageTexts.size(); ++j) {
			Damage* damageB = damageTexts[j].damageText.get();
			if (!damageB || !damageB->IsVisible()) {
				continue;
			}

			Vector3 positionB = damageB->GetPosition();
			Vector3 delta = positionB - positionA;
			const float distanceSq = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
			if (distanceSq >= separationRadiusSq) {
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
			const Vector3 push{
			    delta.x * invDistance * pushDistance,
			    delta.y * invDistance * pushDistance,
			    delta.z * invDistance * pushDistance,
			};

			positionA -= push;
			positionB += push;
			damageB->SetPosition(positionB);
		}

		damageA->SetPosition(positionA);
	}
}

void EnemyManager::ResolveOverlaps(const Vector3& playerPos, bool isPlayerAlive) {
	const float enemyMinDistance = kEnemySeparationRadius * 2.0f;
	const float enemyMinDistanceSq = enemyMinDistance * enemyMinDistance;
	const float playerMinDistance = kEnemySeparationRadius + kPlayerSeparationRadius;

	for (int iteration = 0; iteration < kSeparationIterations; ++iteration) {
		for (std::size_t i = 0; i < enemies.size(); ++i) {
			BaseEnemy* enemyA = enemies[i].get();
			if (!enemyA || !enemyA->GetIsAlive() || enemyA->IsDying()) {
				continue;
			}

			Vector3 positionA = enemyA->GetPosition();
			if (isPlayerAlive) {
				PushAwayXZ(positionA, playerPos, playerMinDistance, positionA - playerPos);
			}

			for (std::size_t j = i + 1; j < enemies.size(); ++j) {
				BaseEnemy* enemyB = enemies[j].get();
				if (!enemyB || !enemyB->GetIsAlive() || enemyB->IsDying()) {
					continue;
				}

				Vector3 positionB = enemyB->GetPosition();
				Vector3 delta = positionA - positionB;
				delta.y = 0.0f;
				float distanceSq = LengthSquaredXZ(delta);

				if (distanceSq >= enemyMinDistanceSq) {
					continue;
				}

				if (distanceSq <= kSeparationEpsilon) {
					const float angle = static_cast<float>(i + j + 1) * 2.399963f;
					delta = {std::cos(angle), 0.0f, std::sin(angle)};
					distanceSq = 1.0f;
				}

				const float distance = std::sqrt(distanceSq);
				const float pushDistance = (enemyMinDistance - distance) * 0.5f;
				const Vector3 push{
				    delta.x / distance * pushDistance,
				    0.0f,
				    delta.z / distance * pushDistance,
				};
				positionA += push;
				positionB -= push;

				if (isPlayerAlive) {
					PushAwayXZ(positionB, playerPos, playerMinDistance, positionB - playerPos);
				}

				enemyB->SetPosition(positionB);
			}

			if (isPlayerAlive) {
				PushAwayXZ(positionA, playerPos, playerMinDistance, positionA - playerPos);
			}
			enemyA->SetPosition(positionA);
		}
	}
}

void EnemyManager::CheckWaveComplete() {
	if (GetAliveEnemyCount() != 0) {
		return;
	}

	waveState_ = WaveState::kComplete;
	totalEnemiesKilled_ += static_cast<int>(enemies.size());
	allWavesComplete_ = currentWave_ >= maxWave_;

	hitEffects.clear();
	for (auto& entry : damageTexts) {
		entry.enemy = nullptr;
	}

	enemies.clear();
}

void EnemyManager::Draw() {
	for (const auto& enemy : enemies) {
		if (enemy->GetIsAlive() || enemy->IsDying()) {
			enemy->Draw();
		}
	}

	auto* common = Object3dCommon::GetInstance();
	common->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);

	for (auto& entry : hitEffects) {
		entry.effect->Draw();
	}
	for (auto& entry : damageTexts) {
		entry.damageText->Draw();
	}

	common->DrawCommon();
}

void EnemyManager::OnEnemyDamaged(BaseEnemy* enemy, int damage, Attribute attribute, bool isCritical, Attribute reactionPreviousAttribute) {
	for (auto& entry : hitEffects) {
		if (entry.enemy == enemy) {
			entry.effect->Activate(enemy->GetPosition());
			break;
		}
	}

	auto damageText = std::make_unique<Damage>();
	damageText->Initialize(camera_);

	const Vector3 targetOffset = kDamageTextTargetOffsets[static_cast<std::size_t>(nextDamageTextOffsetIndex_)];
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
	for (const auto& enemy : enemies) {
		if (enemy->GetIsAlive()) {
			++count;
		}
	}
	return count;
}

bool EnemyManager::IsWaveActive() const { return waveState_ == WaveState::kActive; }

bool EnemyManager::IsWaveComplete() const { return waveState_ == WaveState::kComplete; }

float EnemyManager::GetWaveProgress() const {
	if (enemies.empty()) {
		return 1.0f;
	}

	const float aliveCount = static_cast<float>(GetAliveEnemyCount());
	const float totalCount = static_cast<float>(enemies.size());
	return 1.0f - aliveCount / totalCount;
}
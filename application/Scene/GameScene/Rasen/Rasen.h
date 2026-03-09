#pragma once

#include "Object/House/House.h"
#include "Sprite.h"
#include "Vector2.h"
#include <memory>

class EnemyManager;
class ExpCubeManager;
class Boss;
class Player;
class Camera;

class Rasen {
public:
	Rasen();
	~Rasen() = default;

	void Initialize(Camera* camera);
	void Update(Camera* camera, Player* player, Boss* boss);
	void Draw(Boss* boss);
	void Finalize();
	void DebugImGui(Boss* boss, Camera* camera);

	House* GetHouse() const { return house.get(); }
	EnemyManager* GetEnemyManager() const { return enemyManager.get(); }
	ExpCubeManager* GetExpCubeManager() const { return expCubeManager.get(); }
	bool IsBossActive() const { return isBossActive_; }
	bool IsGoalActive() const { return goalActive; }
	bool IsLevelSelecting() const { return isLevelSelecting; }

private:
	std::unique_ptr<House> house;
	std::unique_ptr<EnemyManager> enemyManager;
	std::unique_ptr<ExpCubeManager> expCubeManager;
	std::unique_ptr<Sprite> levelupIcons[4];
	std::unique_ptr<Sprite> phaseSprites_[5];
	std::unique_ptr<Sprite> bossHpBarSprite_;
	std::unique_ptr<Sprite> bossHpBarBackSprite_;
	std::unique_ptr<Sprite> warningSprite_;
	int currentPhaseSpriteIndex_ = 0;
	int lastWave_ = 0;
	bool isPhaseSpriteActive_ = false;
	bool isPhaseSpritePaused_ = false;
	float phaseSpriteX_ = 0.0f;
	float phaseSpriteStopTimer_ = 0.0f;
	Vector2 phaseSpriteSize_ = {400.0f, 120.0f};
	float phaseSpriteY_ = 80.0f;
	Vector2 bossHpBarSize_ = {400.0f, 120.0f};
	Vector2 warningSpriteBaseScale_ = {600.0f, 200.0f};
	bool isWarningActive_ = false;
	float warningTimer_ = 0.0f;
	float warningDuration_ = 2.5f;
	bool isBossActive_ = false;
	bool goalActive = false;
	bool isLevelSelecting = false;
	int selectChoices[2]{};
	int cursorIndex = 0;
	int debugPhaseSelection_ = 0;

	void ApplyPhaseSelection(int selectionIndex, Camera* camera, Boss* boss);
};
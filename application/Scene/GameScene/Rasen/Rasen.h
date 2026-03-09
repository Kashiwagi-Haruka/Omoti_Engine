#pragma once
#include <memory>
#include "Object/House/House.h"
#include "Sprite.h"
class Enemy;
class EnemyManager;
class ExpCubeManager;
class Boss;

class Rasen {

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

	std::unique_ptr<House> house;
	std::unique_ptr<EnemyManager> enemyManager;
	std::unique_ptr<ExpCubeManager> expCubeManager;
	// レベルアップ選択専用スプライト
	std::unique_ptr<Sprite> levelupIcons[4]; // 0:Atk, 1:Speed, 2:HP, 3:Allow
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

	bool goalActive = false; // 敵全滅後に true になる
	// =====================
	// レベルアップ選択用
	// =====================
	bool isLevelSelecting = false;
	int selectChoices[2]; // 0:Atk, 1:Speed, 2:HP, 3:Allow
	int cursorIndex = 0;  // 0 or 1
	int debugPhaseSelection_ = 0;

	void ApplyPhaseSelection(int selectionIndex);
};

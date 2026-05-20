#pragma once
#include <memory>
#include "Vector2.h"
#include "Text/Text.h"
#include <cstdint>

class Sprite;
class HPBarUI {
	struct SpriteData {
		// 描画用スプライト本体。
		std::unique_ptr<Sprite> sprite;
		// 使用するテクスチャハンドル。
		uint32_t handle = 0;
		// スプライトサイズ（スケール）。
		Vector2 size = {100, 100};
		// 回転量（未使用の場合は0）。
		Vector2 rotate = {0, 0};
		// 描画位置。
		Vector2 translate = {0, 0};
	};

	// プレイヤーHPの数値テキスト
	Text playerHpText;

	// プレイヤーHPバー用スプライト。
	SpriteData playerHpSPData;
	// プレイヤーHPフレーム用スプライト。
	SpriteData playerHPFlameSPData;

		// 現在のプレイヤーHP。
	int playerHP=1;
	// プレイヤーHP最大値。
	int playerHPMax=1;
	// HPバー最大サイズ。
	Vector2 playerHPMaxSize = {400, 100};
	// HPバーの現在表示幅。
	float playerHPWidth = 1200.0f;
	// HPバーの最大表示幅。
	float playerHPWidthMax = 1200.0f;

public:
	HPBarUI();
	void Initialize();
	void Update();
	void Draw();
	void SetPlayerHP(int HP) { playerHP = HP; }
	void SetPlayerHPMax(int HPMax) { playerHPMax = HPMax; }
};

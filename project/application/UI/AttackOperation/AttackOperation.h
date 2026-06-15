#pragma once
#include "Vector2.h"
#include <cstdint>
#include <memory>
class Sprite;
class AttackOperation {
	// スプライトに必要なリソースと変換情報。
	struct SpriteData {
		// 描画用スプライト本体。
		std::unique_ptr<Sprite> sprite;
		// スプライトサイズ（スケール）。
		Vector2 size = {100, 100};
		// 回転量（未使用の場合は0）。
		Vector2 rotate = {0, 0};
		// 描画位置。
		Vector2 translate = {0, 0};
	};

	// ジャンプのスプライト
	SpriteData jumpSPData_;
	// 通常攻撃のスプライトデータ。
	SpriteData NormalAttackSPData;
	// スキルアイコン表示用スプライト。
	SpriteData SkillIconSPData;
	

	public:
	AttackOperation();
	void Initialize();
	void Update();
	void Draw();

};

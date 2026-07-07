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

	// ダッシュのスプライトデータ。
	SpriteData dashSPData_;
	// ジャンプのスプライト
	SpriteData jumpSPData_;
	// 通常攻撃のスプライトデータ。
	SpriteData normalAttackSPData_;
	// スキルアイコン表示用スプライト。
	SpriteData skillIconSPData_;
	// スペシャル攻撃のスプライトデータ。
	SpriteData specialAttackSPData_;
	// キーボードスプライト
	SpriteData keyboardDashSPData_;
	SpriteData keyboardJumpSPData_;
	SpriteData keyboardNormalAttackSPData_;
	SpriteData keyboardSkillIconSPData_;
	SpriteData keyboardSpecialAttackSPData_;

	SpriteData padDashSPData_;
	SpriteData padJumpSPData_;
	SpriteData padNormalAttackSPData_;
	SpriteData padSkillIconSPData_;
	SpriteData padSpecialAttackSPData_;

	public:
	AttackOperation();
	void Initialize();
	void Update();
	void Draw();

private:
	void UpdateOperationSprite(SpriteData& spriteData, bool isPressed);
	void UpdateKeyboardSprite(SpriteData& keyboardSpriteData, const SpriteData& iconSpriteData);
	void SetOperationSpriteBaseSize(SpriteData& spriteData, const Vector2& size);
};

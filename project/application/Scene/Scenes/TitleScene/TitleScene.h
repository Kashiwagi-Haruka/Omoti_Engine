#pragma once
#include "Audio.h"
#include "BaseScene.h"
#include "SceneTransition/SceneTransition.h"
#include "Sprite.h"
#include <memory>
class GameBase;

class TitleScene : public BaseScene {
	// スプライトデータ構造体
	struct SpriteData {
		std::unique_ptr<Sprite> sprite = nullptr;
		uint32_t handle = 0;
		Vector2 size = {100, 100};
		float rotate = {0};
		Vector2 translate = {0, 0};
	};
	// 背景のスプライト
	SpriteData BackGroundSpriteData_;
	// ロゴのスプライト
	SpriteData logoSpriteData_;
	// ルールのスプライト
	SpriteData ruleSpriteData_;
	// 公式サイトのスプライト
	SpriteData OfficialWebsiteSpriteData_;
	// スペースキーのスプライト
	std::unique_ptr<Sprite> pressSpaceSprite = nullptr;
	// スペースキーのテクスチャハンドル
	uint32_t pressSpaceHandle = 0;
	// スペースキーの位置とサイズ
	Vector2 pressSpacePos = {640, 420};
	Vector2 pressSpaceSize = {300, 300};
	// 背景の回転速度
	float backgroundRotationSpeed = 0.01f;
	// シーン遷移閉じか
	bool isTransitionIn = false;
	// シーン遷移明けか
	bool isTransitionOut = false;
	// シーン遷移
	std::unique_ptr<SceneTransition> transition = nullptr;

public:
	// コンストラクタ
	TitleScene();
	// デストラクタ	
	~TitleScene() override = default;
	// 初期化
	void Initialize() override;
	// 更新
	void Update() override;
	// 描画
	void Draw() override;
	// 終了
	void Finalize() override;
};

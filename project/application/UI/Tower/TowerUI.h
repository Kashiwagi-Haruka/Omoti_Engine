#pragma once
#include "Vector2.h"
#include <memory>
class Sprite;
class TowerUI {

	// スプライトに必要なリソースと変換情報。
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

	// 家HP数字表示用スプライト群。
	SpriteData houseHpNumberSPData[3];
	// 家HPの%表示用スプライト。
	SpriteData houseHpPercentSPData;
	// 家HPラベル用スプライト。
	SpriteData houseHpStringSPData;

	// 数字テクスチャの1桁あたりサイズ。
	Vector2 numbersTextureSize = {300, 300};
	// 家HP数字テクスチャの1桁あたりサイズ。
	Vector2 houseHpNumbersTextureSize = {400, 400};
	// 家HPの%表示基準位置。
	Vector2 houseHpPercentBasePosition = {980, 520};
	// 家HPラベルのオフセット。
	Vector2 houseHpStringOffset = {0, -40};
	// 家HPの桁表示開始インデックス。
	int houseHpDigitStartIndex = 0;

	public:

	// 生成時にUIリソースを読み込む。
	TowerUI();
	// 破棄時にリソースを解放する。
	~TowerUI();
	// UI表示の初期化を行う。
	void Initialize();
	// UI状態を更新する。
	void Update();
	// UIを描画する。
	void Draw();
};

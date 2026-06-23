#pragma once
#include "Text/Text.h"
#include "Vector2.h"
#include <cstdint>

class TowerUI {
	// 家HP表示用テキスト。
	Text houseHpText_;
	// 家HPラベル用テキスト。
	Text houseHpLabelText_;
	// 家HP表示に使うフォントハンドル。
	uint32_t houseHpFontHandle_ = 0;

	// 家HPの%表示基準位置。
	Vector2 houseHpPercentBasePosition = {980, 520};
	// 家HPラベルのオフセット。
	Vector2 houseHpStringOffset = {0, -40};

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
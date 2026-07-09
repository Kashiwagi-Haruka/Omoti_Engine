#pragma once
#include "Text/Text.h"
#include "Menu/CharacterDisplay/CharacterDisplayMenuType.h"
class CharacterDisplayMenuText {
	// ステータステキスト
	Text statusText_;
	// 武器テキスト
	Text weaponText_;
	// 装備テキスト
	Text equipText_;
	// スキルツリーテキスト
	Text skilltree_;
	// 強化テキスト
	Text reinforcement_;
	// プロフィールテキスト
	Text profileText_;

	CharacterDisplayMenuType selectMenutype_;

public:
	CharacterDisplayMenuText() = default;
	~CharacterDisplayMenuText() = default;
	void Initialize();
	void Update();
	void Draw();
	void SetMenuType(CharacterDisplayMenuType menuType);

};

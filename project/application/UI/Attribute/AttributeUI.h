#pragma once
#include "Object/Characters/Base/Attribute.h"
#include "Sprite.h"
#include <array>
#include <memory>

class Player;
class AttributeUI {

private:
	const float kAttributeIconDistance = 150.0f;
	const float kAttributeIconSize = 64.0f;
	const float kAttributeStickDeadZone = 0.45f;
	const std::array<const char*, 6> kAttributeTexturePaths = {"Resources/2d/Attribute/Fire.png",    "Resources/2d/Attribute/Ice.png",       "Resources/2d/Attribute/Wind.png",
	                                                               "Resources/2d/Attribute/Thunder.png", "Resources/2d/Attribute/Imaginary.png", "Resources/2d/Attribute/Quantum.png"};
	const std::array<Attribute, 6> kAttributes = {Attribute::Fire, Attribute::Ice, Attribute::Wind, Attribute::Thunder, Attribute::Imaginary, Attribute::Quantum};
	
	// それぞれの属性のスプライト
	std::array<std::unique_ptr<Sprite>, 6> attributeSprites_{};
	// 今選んでる属性
	int selectedAttributeIndex_ = 1;
	// 属性メニュー開いてるか
	bool isAttributeMenuOpen_ = false;

	// プレイヤー
	Player* player_;

public:

	AttributeUI();
	void Initialize();
	void Update();
	void Draw();
	void SetPlayer(Player* player);
	bool GetIsAttributeMenuOpen() const;
};

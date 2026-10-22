#include "AttributeUI.h"
#include "TextureManager.h"
#include "ScreenSize.h"
#include "Input.h"
#include "Object/Player/Player.h"
#include <numbers>
AttributeUI::AttributeUI() {
	for (auto& sprite : attributeSprites_) {
		sprite = std::make_unique<Sprite>();
	}

}
void AttributeUI::Initialize() {
	for (size_t i = 0; i < attributeSprites_.size(); ++i) {
		attributeSprites_[i]->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath(kAttributeTexturePaths[i]));
		const float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(attributeSprites_.size());
		attributeSprites_[i]->SetPosition({SCREEN_SIZE::HALF_WIDTH + std::sin(angle) * kAttributeIconDistance, SCREEN_SIZE::HALF_HEIGHT - std::cos(angle) * kAttributeIconDistance});
		attributeSprites_[i]->SetAnchorPoint({0.5f, 0.5f});
	}
}
void AttributeUI::Update() {
	isAttributeMenuOpen_ = player_ && player_->IsSizuku() && (Input::GetInstance()->PushLeftTrigger() || Input::GetInstance()->PushKey(DIK_Y));
	if (isAttributeMenuOpen_) {
		Input::GetInstance()->SetIsCursorStability(false);
		bool hasSelectionInput = false;
		const Vector2 stick = Input::GetInstance()->GetJoyStickLXY();
		if (stick.x * stick.x + stick.y * stick.y >= kAttributeStickDeadZone * kAttributeStickDeadZone) {
			float angle = std::atan2(stick.x, stick.y);
			if (angle < 0.0f) {
				angle += 2.0f * std::numbers::pi_v<float>;
			}
			const float sector = 2.0f * std::numbers::pi_v<float> / static_cast<float>(attributeSprites_.size());
			selectedAttributeIndex_ = static_cast<int>((angle + sector * 0.5f) / sector) % static_cast<int>(attributeSprites_.size());
			hasSelectionInput = true;
		}

		const Vector2 mousePosition = {Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()};
		for (size_t i = 0; i < attributeSprites_.size(); ++i) {
			const Vector2 iconPosition = attributeSprites_[i]->GetPosition();
			const float halfIconSize = kAttributeIconSize * 0.5f;
			if (std::abs(mousePosition.x - iconPosition.x) <= halfIconSize && std::abs(mousePosition.y - iconPosition.y) <= halfIconSize) {
				selectedAttributeIndex_ = static_cast<int>(i);
				hasSelectionInput = true;
				break;
			}
		}

		if (hasSelectionInput) {
			player_->SetCurrentAttribute(kAttributes[selectedAttributeIndex_]);
		}
	}
	for (size_t i = 0; i < attributeSprites_.size(); ++i) {
		const float size = static_cast<int>(i) == selectedAttributeIndex_ ? kAttributeIconSize * 1.25f : kAttributeIconSize;
		attributeSprites_[i]->SetScale({size, size});
		attributeSprites_[i]->SetColor(static_cast<int>(i) == selectedAttributeIndex_ ? Vector4{1.0f, 1.0f, 1.0f, 1.0f} : Vector4{0.65f, 0.65f, 0.65f, 0.85f});
		attributeSprites_[i]->Update();
	}
}

void AttributeUI::Draw() {
	if (isAttributeMenuOpen_) {
		for (const auto& sprite : attributeSprites_) {
			sprite->Draw();
		}
	}
}

void AttributeUI::SetPlayer(Player* player) { player_ = player; }
bool AttributeUI::GetIsAttributeMenuOpen() const { return isAttributeMenuOpen_; }
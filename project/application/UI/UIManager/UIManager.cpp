#define NOMINMAX
#include "UIManager.h"
#include "Input.h"
#include "Object/Player/Player.h"
#include "PlayCommand/PlayCommand.h"
#include "ScreenSize.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include <array>
#include <cmath>
#include <numbers>

namespace {
constexpr float kAttributeIconDistance = 150.0f;
constexpr float kAttributeIconSize = 64.0f;
constexpr float kAttributeStickDeadZone = 0.45f;
constexpr std::array<const char*, 6> kAttributeTexturePaths = {"Resources/2d/Attribute/Fire.png",   "Resources/2d/Attribute/Ice.png",       "Resources/2d/Attribute/Wind.png",
                                                               "Resources/2d/Attribute/Thunder.png", "Resources/2d/Attribute/Imaginary.png", "Resources/2d/Attribute/Quantum.png"};
constexpr std::array<Attribute, 6> kAttributes = {Attribute::Fire, Attribute::Ice, Attribute::Wind, Attribute::Thunder, Attribute::Imaginary, Attribute::Quantum};
} // namespace

UIManager::UIManager() {
	cursolSprite_ = std::make_unique<Sprite>();
	controllerSprite_ = std::make_unique<Sprite>();
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	dashGaugeUI_ = std::make_unique<DashGauge>();
	towerUI_ = std::make_unique<TowerUI>();
	menuUI_ = std::make_unique<Menu>();
	teamUI_ = std::make_unique<TeamUI>();
	padMenuUI_ = std::make_unique<PadMenu>();
	for (auto& sprite : attributeSprites_) {
		sprite = std::make_unique<Sprite>();
	}
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	cursolSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Cursor.png"));
	cursolSprite_->SetScale({30.0f, 30.0f});

	controllerSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/controller.png"));
	controllerSprite_->SetScale({70.0f, 70.0f});
	controllerSprite_->SetPosition({10.0f, SCREEN_SIZE::HEIGHT - 20.0f});
	controllerSprite_->SetAnchorPoint({0.0f, 1.0f});
	controllerSprite_->Update();

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	dashGaugeUI_->Initialize();

	towerUI_->Initialize();

	menuUI_->Initialize();

	padMenuUI_->Initialize();
	const Vector2 center = {SCREEN_SIZE::WIDTH / 2.0f, SCREEN_SIZE::HEIGHT / 2.0f};
	for (size_t i = 0; i < attributeSprites_.size(); ++i) {
		attributeSprites_[i]->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath(kAttributeTexturePaths[i]));
		const float angle = 2.0f * std::numbers::pi_v<float> * static_cast<float>(i) / static_cast<float>(attributeSprites_.size());
		attributeSprites_[i]->SetPosition({center.x + std::sin(angle) * kAttributeIconDistance, center.y - std::cos(angle) * kAttributeIconDistance});
		attributeSprites_[i]->SetAnchorPoint({0.5f, 0.5f});
	}
	if (team_) {
		teamUI_->Initialize(*team_);
	}
}

void UIManager::Update() {

	cursolSprite_->SetPosition({Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()});
	cursolSprite_->Update();

	hpBarUI_->Update();

	attackOperationUI_->Update();

	dashGaugeUI_->Update();

	towerUI_->Update();

	menuUI_->Update();

	padMenuUI_->Update();
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
	if (team_) {
		teamUI_->Update(*team_);
	}
}


void UIManager::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	hpBarUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	attackOperationUI_->Draw();

	dashGaugeUI_->Draw();

	towerUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	menuUI_->Draw();
	if (team_) {
		teamUI_->Draw();
	}
	padMenuUI_->Draw();
	if (isAttributeMenuOpen_) {
		for (const auto& sprite : attributeSprites_) {
			sprite->Draw();
		}
	}
	controllerSprite_->Draw();
	if (PlayCommand::GetCURSOR_DISPLAY() || isAttributeMenuOpen_) {
		if (cursolSprite_) {
			cursolSprite_->Draw();
		}
	}
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
void UIManager::SetTeam(Team* team) { team_ = team; }
void UIManager::SetPlayerDashGauge(float dashGauge, float dashGaugeMax, bool isDashView) {
	const float gaugeRate = dashGaugeMax > 0.0f ? dashGauge / dashGaugeMax : 0.0f;
	dashGaugeUI_->SetGaugeRate(gaugeRate);
	dashGaugeUI_->SetDamageUIView(isDashView);
}
void UIManager::SetSpecialAttackCooldown(float remainingSeconds) { attackOperationUI_->SetSpecialCooldownRemaining(remainingSeconds); }
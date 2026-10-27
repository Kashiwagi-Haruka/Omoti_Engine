#define NOMINMAX
#include "UIManager.h"
#include "Input.h"
#include "Object/Player/Player.h"
#include "PlayCommand/PlayCommand.h"
#include "GameBase.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"

UIManager::UIManager() {
	//生成
	cursolSprite_ = std::make_unique<Sprite>();
	controllerSprite_ = std::make_unique<Sprite>();
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	dashGaugeUI_ = std::make_unique<DashGauge>();
	towerUI_ = std::make_unique<TowerUI>();
	menuUI_ = std::make_unique<Menu>();
	teamUI_ = std::make_unique<TeamUI>();
	padMenuUI_ = std::make_unique<PadMenu>();
	attributeUI_ = std::make_unique<AttributeUI>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {
	// 初期化
	cursolSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Cursor.png"));
	cursolSprite_->SetScale({30.0f, 30.0f});

	controllerSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/controller.png"));
	controllerSprite_->SetScale({70.0f, 70.0f});
	controllerSprite_->SetPosition({10.0f, GameBase::GetInstance()->GetScreenHeight() - 20.0f});
	controllerSprite_->SetAnchorPoint({0.0f, 1.0f});
	controllerSprite_->Update();

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	dashGaugeUI_->Initialize();

	towerUI_->Initialize();

	menuUI_->Initialize();

	padMenuUI_->Initialize();
	if (team_) {
		teamUI_->Initialize(*team_);
	}
	attributeUI_->Initialize();
}

void UIManager::Update() {

	cursolSprite_->SetPosition({Input::GetInstance()->GetMouseX(), Input::GetInstance()->GetMouseY()});
	cursolSprite_->Update();

	hpBarUI_->Update();
	if (player_) {
		attackOperationUI_->SetCurrentAttribute(player_->GetCurrentAttribute());
	}

	attackOperationUI_->Update();

	dashGaugeUI_->Update();

	towerUI_->Update();

	menuUI_->Update();

	padMenuUI_->Update();

	if (team_) {
		teamUI_->Update(*team_);
	}
	attributeUI_->SetPlayer(player_);
	attributeUI_->Update();
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

	controllerSprite_->Draw();
	if (PlayCommand::GetInstance()->GetCURSOR_DISPLAY() || attributeUI_->GetIsAttributeMenuOpen()) {
		if (cursolSprite_) {
			cursolSprite_->Draw();
		}
	}
	attributeUI_->Draw();
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
void UIManager::SetPlayer(Player* player) { player_ = player; }
void UIManager::SetSpecialAttackCooldown(float remainingSeconds) { attackOperationUI_->SetSpecialCooldownRemaining(remainingSeconds); }
#define NOMINMAX
#include "UIManager.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"

UIManager::UIManager() {
	cursolSprite_ = std::make_unique<Sprite>();
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	dashGaugeUI_ = std::make_unique<DashGauge>();
	towerUI_ = std::make_unique<TowerUI>();
	menuUI_ = std::make_unique<Menu>();
	teamUI_ = std::make_unique<TeamUI>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	cursolSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Cursor.png"));
	cursolSprite_->SetScale({30.0f, 30.0f});

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	dashGaugeUI_->Initialize();

	towerUI_->Initialize();

	menuUI_->Initialize();
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
	if (PlayCommand::GetCURSOR_DISPLAY()) {
		if (cursolSprite_) {
			cursolSprite_->Draw();
		}
	}
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
void UIManager::SetTeam(Team* team) { team_ = team; }
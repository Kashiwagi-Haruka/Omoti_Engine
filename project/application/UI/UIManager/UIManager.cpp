#define NOMINMAX
#include "UIManager.h"
#include "Sprite/SpriteCommon.h"

UIManager::UIManager() {
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	towerUI_ = std::make_unique<TowerUI>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	towerUI_->Initialize();
}

void UIManager::Update() {

	hpBarUI_->Update();

	attackOperationUI_->Update();

	towerUI_->Update();
}

void UIManager::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	hpBarUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	attackOperationUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	towerUI_->Draw();
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
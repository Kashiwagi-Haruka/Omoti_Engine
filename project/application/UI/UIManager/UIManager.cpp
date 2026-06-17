#define NOMINMAX
#include "UIManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"

UIManager::UIManager() {
	cursolSprite_ = std::make_unique<Sprite>();
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	towerUI_ = std::make_unique<TowerUI>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	cursolSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/cursol.png"));

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	towerUI_->Initialize();
}

void UIManager::Update() {

	cursolSprite_->Update();

	hpBarUI_->Update();

	attackOperationUI_->Update();

	towerUI_->Update();
}

void UIManager::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	if (cursolSprite_) {
		cursolSprite_->Draw();
	}
	hpBarUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	attackOperationUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	towerUI_->Draw();
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
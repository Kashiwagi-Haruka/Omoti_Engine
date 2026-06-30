#define NOMINMAX
#include "UIManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"

UIManager::UIManager() {
	cursolSprite_ = std::make_unique<Sprite>();
	hpBarUI_ = std::make_unique<HPBarUI>();
	attackOperationUI_ = std::make_unique<AttackOperation>();
	towerUI_ = std::make_unique<TowerUI>();
	menuUI_ = std::make_unique<Menu>();
}

UIManager::~UIManager() {}

void UIManager::Initialize() {

	cursolSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Cursor.png"));
	cursolSprite_->SetScale({30.0f, 30.0f});

	hpBarUI_->Initialize();

	attackOperationUI_->Initialize();

	towerUI_->Initialize();

	menuUI_->Initialize();
}

void UIManager::Update() {

	cursolSprite_->SetPosition({
	    Input::GetInstance()->GetMouseX(),Input::GetInstance()->GetMouseY()});
	cursolSprite_->Update();

	hpBarUI_->Update();

	attackOperationUI_->Update();

	towerUI_->Update();

	menuUI_->Update();
}

void UIManager::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	hpBarUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	attackOperationUI_->Draw();
	towerUI_->Draw();
	SpriteCommon::GetInstance()->DrawCommon();
	/*menuUI_->Draw();*/
	if (PlayCommand::GetCURSOR_DISPLAY()) {
		if (cursolSprite_) {
			cursolSprite_->Draw();
		}
	}
}

void UIManager::SetPlayerHP(int HP) { hpBarUI_->SetPlayerHP(HP); }
void UIManager::SetPlayerHPMax(int HPMax) { hpBarUI_->SetPlayerHPMax(HPMax); }
void UIManager::SetPlayerParameters(Parameters parameters) { parameters_ = parameters; }
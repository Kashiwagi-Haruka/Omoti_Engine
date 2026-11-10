#include "GameOverScene.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "AudioManager/BGMManager/BGMManager.h"
GameOverScene::GameOverScene() {

	logoSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/over.png");
	logoSpriteData_.sprite = std::make_unique<Sprite>();
	logoSpriteData_.sprite->Initialize(logoSpriteData_.handle);
	transition = std::make_unique<SceneTransition>();
}

void GameOverScene::Finalize() {}

void GameOverScene::Initialize() {

	logoSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	logoSpriteData_.size = {1280, 720};
	logoSpriteData_.translate = {640, 360};
	logoSpriteData_.sprite->SetScale(logoSpriteData_.size);
	logoSpriteData_.sprite->SetPosition(logoSpriteData_.translate);
	logoSpriteData_.sprite->Update();

	pressSpaceHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/SPACE.png");

	pressSpaceSprite = std::make_unique<Sprite>();
	pressSpaceSprite->Initialize(pressSpaceHandle);
	pressSpaceSprite->SetAnchorPoint({0.5f, 0.5f});
	pressSpaceSprite->SetScale(pressSpaceSize);
	pressSpaceSprite->SetPosition(pressSpacePos);
	pressSpaceSprite->SetTextureRange({0, 0}, {768, 768});
	pressSpaceSprite->Update();
	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
	transition->Initialize(false);
	isTransitionIn = true;
	isTransitionOut = false;
}
void GameOverScene::Update() {
	BGMManager::GetInstance()->Play(BGMManager::BGMType::GameOver);
	if (PlayCommand::GetInstance()->GetDESIDE() && !isTransitionOut) {
		transition->Initialize(true);
		isTransitionOut = true;
	}
	if (isTransitionIn || isTransitionOut) {
		transition->Update();
		if (transition->IsEnd() && isTransitionIn) {
			isTransitionIn = false;
		}
		if (transition->IsEnd() && isTransitionOut) {
			SceneManager::GetInstance()->ChangeScene("Title");
		}
	}
#ifdef USE_IMGUI
	ImGui::Begin("resultScene");
	ImGui::End();
#endif // USE_IMGUI
}

void GameOverScene::Draw() {
	SpriteCommon::GetInstance()->DrawCommon();
	logoSpriteData_.sprite->Draw();
	pressSpaceSprite->Draw();
	if (isTransitionIn || isTransitionOut) {
		transition->Draw();
	}
}
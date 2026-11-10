#include "TitleScene.h"
#include "GameBase.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "AudioManager/BGMManager/BGMManager.h"
#include "Object/Load/GameLoad.h"

TitleScene::TitleScene() {
	BackGroundSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/title.png");
	BackGroundSpriteData_.sprite = std::make_unique<Sprite>();
	BackGroundSpriteData_.sprite->Initialize(BackGroundSpriteData_.handle);
	logoSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/logo.png");
	logoSpriteData_.sprite = std::make_unique<Sprite>();
	logoSpriteData_.sprite->Initialize(logoSpriteData_.handle);
	ruleSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/rule.png");
	ruleSpriteData_.sprite = std::make_unique<Sprite>();
	ruleSpriteData_.sprite->Initialize(ruleSpriteData_.handle);
	OfficialWebsiteSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Title/OfficialWebsite.png");
	OfficialWebsiteSpriteData_.sprite = std::make_unique<Sprite>();
	OfficialWebsiteSpriteData_.sprite->Initialize(OfficialWebsiteSpriteData_.handle);
	transition = std::make_unique<SceneTransition>();
}

void TitleScene::Finalize() {}

void TitleScene::Initialize() {

	BackGroundSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	BackGroundSpriteData_.size = {2000, 2000};
	BackGroundSpriteData_.translate = {640, 0};
	BackGroundSpriteData_.sprite->SetScale(BackGroundSpriteData_.size);
	BackGroundSpriteData_.sprite->SetPosition(BackGroundSpriteData_.translate);
	BackGroundSpriteData_.sprite->Update();
	logoSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	logoSpriteData_.sprite->SetPosition({640, 300});
	logoSpriteData_.sprite->SetScale({500, 500});
	logoSpriteData_.sprite->Update();
	ruleSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	ruleSpriteData_.sprite->SetScale({640, 360});
	ruleSpriteData_.sprite->SetPosition({640, 300});
	ruleSpriteData_.sprite->Update();
	OfficialWebsiteSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	OfficialWebsiteSpriteData_.sprite->SetPosition({1000, 80});
	OfficialWebsiteSpriteData_.sprite->SetScale({300, 60});
	OfficialWebsiteSpriteData_.sprite->Update();
	pressSpaceHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/SPACE.png");

	pressSpaceSprite = std::make_unique<Sprite>();
	pressSpaceSprite->Initialize(pressSpaceHandle);
	pressSpaceSprite->SetAnchorPoint({0.5f, 0.5f});
	pressSpaceSprite->SetTextureRange({0, 0}, {768, 768});
	pressSpaceSprite->SetScale(pressSpaceSize);
	pressSpaceSprite->SetPosition(pressSpacePos);
	pressSpaceSprite->Update();
	isTransitionIn = true;
	isTransitionOut = false;
	transition->Initialize(false);
	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);

	GameLoad::GetInstance()->InitializeLoadSprites();
}

void TitleScene::Update() {
	BGMManager::GetInstance()->Play(BGMManager::BGMType::Title);
	
	GameLoad::GetInstance()->Update();

	BackGroundSpriteData_.rotate += 0.01f;

	BackGroundSpriteData_.sprite->SetRotation(BackGroundSpriteData_.rotate);
	BackGroundSpriteData_.sprite->Update();
	OfficialWebsiteSpriteData_.sprite->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	if (Input::GetInstance()->GetMouseX()>( - OfficialWebsiteSpriteData_.sprite->GetScale().x * 0.5f) + OfficialWebsiteSpriteData_.sprite->GetPosition().x &&
	    Input::GetInstance()->GetMouseX() < OfficialWebsiteSpriteData_.sprite->GetScale().x * 0.5f + OfficialWebsiteSpriteData_.sprite->GetPosition().x) {
		if (Input::GetInstance()->GetMouseY() > -OfficialWebsiteSpriteData_.sprite->GetScale().y * 0.5f + OfficialWebsiteSpriteData_.sprite->GetPosition().y &&
		    Input::GetInstance()->GetMouseY() < OfficialWebsiteSpriteData_.sprite->GetScale().y * 0.5f + OfficialWebsiteSpriteData_.sprite->GetPosition().y) {
			OfficialWebsiteSpriteData_.sprite->SetColor({1.0f, 1.0f, 0.0f, 1.0f});
			if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft)) {
				GameBase::GetInstance()->OpenHttps(L"https://iceflowergame.github.io/OfficialWebsite/");
			}
		} else {
			
		}
	}

	if (PlayCommand::GetInstance()->GetDESIDE() && !isTransitionOut) {
		transition->Initialize(true);
		isTransitionOut = true;
		GameLoad::GetInstance()->GameSceneLoad();
	}

	if (isTransitionIn || isTransitionOut) {
		transition->Update();
		if (transition->IsEnd() && isTransitionIn) {
			isTransitionIn = false;
		}
		if (transition->IsEnd() && isTransitionOut && GameLoad::GetInstance()->IsLoadComplete()) {
			SceneManager::GetInstance()->ChangeScene("StageSelect");
			/*SceneManager::GetInstance()->ChangeScene("Game");*/
		}
	}
#ifdef USE_IMGUI
	ImGui::Begin("titleScene");
	ImGui::End();
	if (ImGui::Begin("Scene")) {
		if (ImGui::Button("Sample")) {
			SceneManager::GetInstance()->ChangeScene("Sample");
		}
		if (ImGui::Button("Tutorial")) {
			SceneManager::GetInstance()->ChangeScene("Tutorial");
		}
	}
	ImGui::End();
#endif // USE_IMGUI
}
void TitleScene::Draw() {

	SpriteCommon::GetInstance()->DrawCommon();
	BackGroundSpriteData_.sprite->Draw();
	logoSpriteData_.sprite->Draw();
	pressSpaceSprite->Draw();
	OfficialWebsiteSpriteData_.sprite->Draw();
	if (isTransitionIn || isTransitionOut) {
		transition->Draw();
	}

	if (isTransitionOut) {
		ruleSpriteData_.sprite->Draw();
	}
	if (isTransitionOut) {
		GameLoad::GetInstance()->DrawLoadProgress();
		
	}
}
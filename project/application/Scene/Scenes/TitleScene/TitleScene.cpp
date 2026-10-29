#include "TitleScene.h"
#include "GameBase.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "AudioManager/BGMManager/BGMManager.h"
#include "Object/Load/GameLoad.h"
#include <imgui.h>
TitleScene::TitleScene() {
	BGSP_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/title.png");
	BGSP_.sprite = std::make_unique<Sprite>();
	BGSP_.sprite->Initialize(BGSP_.handle);
	logoSP_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/logo.png");
	logoSP_.sprite = std::make_unique<Sprite>();
	logoSP_.sprite->Initialize(logoSP_.handle);
	ruleSP_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/rule.png");
	ruleSP_.sprite = std::make_unique<Sprite>();
	ruleSP_.sprite->Initialize(ruleSP_.handle);
	OfficialWebsiteSP_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Title/OfficialWebsite.png");
	OfficialWebsiteSP_.sprite = std::make_unique<Sprite>();
	OfficialWebsiteSP_.sprite->Initialize(OfficialWebsiteSP_.handle);
	transition = std::make_unique<SceneTransition>();
}

void TitleScene::Finalize() {}

void TitleScene::Initialize() {

	BGSP_.sprite->SetAnchorPoint({0.5f, 0.5f});
	BGSP_.size = {2000, 2000};
	BGSP_.translate = {640, 0};
	BGSP_.sprite->SetScale(BGSP_.size);
	BGSP_.sprite->SetPosition(BGSP_.translate);
	BGSP_.sprite->Update();
	logoSP_.sprite->SetAnchorPoint({0.5f, 0.5f});
	logoSP_.sprite->SetPosition({640, 300});
	logoSP_.sprite->SetScale({500, 500});
	logoSP_.sprite->Update();
	ruleSP_.sprite->SetAnchorPoint({0.5f, 0.5f});
	ruleSP_.sprite->SetScale({640, 360});
	ruleSP_.sprite->SetPosition({640, 300});
	ruleSP_.sprite->Update();
	OfficialWebsiteSP_.sprite->SetAnchorPoint({0.5f, 0.5f});
	OfficialWebsiteSP_.sprite->SetPosition({1000, 80});
	OfficialWebsiteSP_.sprite->SetScale({300, 60});
	OfficialWebsiteSP_.sprite->Update();
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

	BGSP_.rotate += 0.01f;

	BGSP_.sprite->SetRotation(BGSP_.rotate);
	BGSP_.sprite->Update();
	OfficialWebsiteSP_.sprite->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	if (Input::GetInstance()->GetMouseX()>( - OfficialWebsiteSP_.sprite->GetScale().x * 0.5f) + OfficialWebsiteSP_.sprite->GetPosition().x &&
	    Input::GetInstance()->GetMouseX() < OfficialWebsiteSP_.sprite->GetScale().x * 0.5f + OfficialWebsiteSP_.sprite->GetPosition().x) {
		if (Input::GetInstance()->GetMouseY() > -OfficialWebsiteSP_.sprite->GetScale().y * 0.5f + OfficialWebsiteSP_.sprite->GetPosition().y &&
		    Input::GetInstance()->GetMouseY() < OfficialWebsiteSP_.sprite->GetScale().y * 0.5f + OfficialWebsiteSP_.sprite->GetPosition().y) {
			OfficialWebsiteSP_.sprite->SetColor({1.0f, 1.0f, 0.0f, 1.0f});
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
	BGSP_.sprite->Draw();
	logoSP_.sprite->Draw();
	pressSpaceSprite->Draw();
	OfficialWebsiteSP_.sprite->Draw();
	if (isTransitionIn || isTransitionOut) {
		transition->Draw();
	}

	if (isTransitionOut) {
		ruleSP_.sprite->Draw();
	}
	if (isTransitionOut) {
		GameLoad::GetInstance()->DrawLoadProgress();
		
	}
}
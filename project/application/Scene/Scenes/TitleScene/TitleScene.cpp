#include "TitleScene.h"
#include "GameBase.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"
#include "SceneManager.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include "AudioManager/BGMManager/BGMManager.h"
#include "Object/Load/GameLoad.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif
// タイトルシーンのコンストラクタ
TitleScene::TitleScene() {
	// 背景スプライトデータの生成・初期化
	BackGroundSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/title.png");
	BackGroundSpriteData_.sprite = std::make_unique<Sprite>();
	BackGroundSpriteData_.sprite->Initialize(BackGroundSpriteData_.handle);
	// ロゴスプライトデータの生成・初期化
	logoSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/logo.png");
	logoSpriteData_.sprite = std::make_unique<Sprite>();
	logoSpriteData_.sprite->Initialize(logoSpriteData_.handle);
	// ルールスプライトデータの生成・初期化
	ruleSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/rule.png");
	ruleSpriteData_.sprite = std::make_unique<Sprite>();
	ruleSpriteData_.sprite->Initialize(ruleSpriteData_.handle);
	// 公式サイトスプライトデータの生成・初期化
	OfficialWebsiteSpriteData_.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Title/OfficialWebsite.png");
	OfficialWebsiteSpriteData_.sprite = std::make_unique<Sprite>();
	OfficialWebsiteSpriteData_.sprite->Initialize(OfficialWebsiteSpriteData_.handle);
	transition = std::make_unique<SceneTransition>();
}

void TitleScene::Finalize() {
	// 終了処理
}

void TitleScene::Initialize() {
	// 初期化処理
	BackGroundSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	BackGroundSpriteData_.size = {2000, 2000};
	BackGroundSpriteData_.translate = {640, 0};
	BackGroundSpriteData_.sprite->SetScale(BackGroundSpriteData_.size);
	BackGroundSpriteData_.sprite->SetPosition(BackGroundSpriteData_.translate);
	BackGroundSpriteData_.sprite->Update();
	// ロゴスプライトの初期化
	logoSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	logoSpriteData_.sprite->SetPosition({640, 300});
	logoSpriteData_.sprite->SetScale({500, 500});
	logoSpriteData_.sprite->Update();
	// ルールスプライトの初期化
	ruleSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	ruleSpriteData_.sprite->SetScale({640, 360});
	ruleSpriteData_.sprite->SetPosition({640, 300});
	ruleSpriteData_.sprite->Update();
	// 公式サイトスプライトの初期化
	OfficialWebsiteSpriteData_.sprite->SetAnchorPoint({0.5f, 0.5f});
	OfficialWebsiteSpriteData_.sprite->SetPosition({1000, 80});
	OfficialWebsiteSpriteData_.sprite->SetScale({300, 60});
	OfficialWebsiteSpriteData_.sprite->Update();
	// スペースキーのスプライトの初期化
	pressSpaceHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/SPACE.png");
	pressSpaceSprite = std::make_unique<Sprite>();
	pressSpaceSprite->Initialize(pressSpaceHandle);
	pressSpaceSprite->SetAnchorPoint({0.5f, 0.5f});
	pressSpaceSprite->SetTextureRange({0, 0}, {768, 768});
	pressSpaceSprite->SetScale(pressSpaceSize);
	pressSpaceSprite->SetPosition(pressSpacePos);
	pressSpaceSprite->Update();
	// シーン遷移の初期化
	isTransitionIn = true;
	isTransitionOut = false;
	transition->Initialize(false);
	// カーソルの可視化・移動可否
	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
	// ロードスプライトの初期化
	GameLoad::GetInstance()->InitializeLoadSprites();
}

void TitleScene::Update() {
	// 更新処理
	// BGMの再生
	BGMManager::GetInstance()->Play(BGMManager::BGMType::Title);
	
	// ゲームのロード進行状況を更新
	GameLoad::GetInstance()->Update();

	// 背景スプライトの回転
	BackGroundSpriteData_.rotate += backgroundRotationSpeed;
	BackGroundSpriteData_.sprite->SetRotation(BackGroundSpriteData_.rotate);
	BackGroundSpriteData_.sprite->Update();

	// 公式サイトスプライトの色変更
	OfficialWebsiteSpriteData_.sprite->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	// 公式サイトスプライトのクリック判定
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

	// スペースキーが押された場合、シーン遷移を開始
	if (PlayCommand::GetInstance()->GetDESIDE() && !isTransitionOut) {
		transition->Initialize(true);
		isTransitionOut = true;
		GameLoad::GetInstance()->GameSceneLoad();
	}

	// シーン遷移の更新
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

	// 描画処理
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
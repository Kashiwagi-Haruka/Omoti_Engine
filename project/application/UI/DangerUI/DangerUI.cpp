#include "DangerUI.h"
#include "TextureManager.h"
#include "Function.h"
#include "GameBase.h"

DangerUI::DangerUI() {}
DangerUI::~DangerUI() {}

void DangerUI::Initialize() { 
	dangerSprite_.emplace_back(std::make_unique<Sprite>());
	uint32_t handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/DangerUI/Pink.png");
	dangerSprite_.back()->Initialize(handle);
	dangerSprite_.emplace_back(std::make_unique<Sprite>());
	uint32_t handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/DangerUI/SkyBlue.png");
	dangerSprite_.back()->Initialize(handle);
	dangerSprite_.emplace_back(std::make_unique<Sprite>());
	uint32_t handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/DangerUI/NavyBlue.png");
	dangerSprite_.back()->Initialize(handle);
}
void DangerUI::Update() {
	dangerTimer_ += GameBase::GetInstance()->GetDeltaTime();
	switch (dangerState_) {
	case DangerUI::DangerState::kNone:
		dangerTimer_ = 0.0f;
		break;
	case DangerUI::DangerState::kStart:
		for (int i = 0; i < dangerSprite_.size(); i++) {
			dangerSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, Function::Lerp(dangerSprite_[i]->GetColor().w, 1.0f, dangerTimer_)});
		}
		if (dangerTimer_ >= 1.0f) {
			dangerState_ = DangerState::kActive;
			dangerTimer_ = 0.0f;
		}

		break;
	case DangerUI::DangerState::kActive:
		break;
	case DangerUI::DangerState::kEnd:

		for (int i = 0; i < dangerSprite_.size(); i++) {
			dangerSprite_[i]->SetColor({1.0f, 1.0f, 1.0f, Function::Lerp(dangerSprite_[i]->GetColor().w, 0.0f, dangerTimer_)});
		}
		if (dangerTimer_ >= 1.0f) {
			dangerState_ = DangerState::kNone;
			dangerTimer_ = 0.0f;
		}
		break;
	default:
		break;
	}

}
void DangerUI::Draw() { 
	if (dangerState_ == DangerState::kNone) {
		return;
	}
	for (int i = 0; i < dangerSprite_.size(); i++) {
		dangerSprite_[i]->Draw();
	}
}

void DangerUI::Start() { 
	dangerState_ = DangerState::kStart;
	dangerTimer_ = 0.0f;
}
void DangerUI::End() { 
	dangerState_ = DangerState::kEnd; 
	dangerTimer_ = 0.0f;
}
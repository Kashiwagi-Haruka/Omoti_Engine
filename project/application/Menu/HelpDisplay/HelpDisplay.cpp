#include "HelpDisplay.h"
#include "TextureManager.h"

HelpDisplay::HelpDisplay() { backgroundSprite_ = std::make_unique<Sprite>(); }

void HelpDisplay::Initialize() {

	uint32_t backgroundHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	backgroundSprite_->Initialize(backgroundHandle);
	backgroundSprite_->SetScale({1280, 720});
	backgroundSprite_->SetPosition({640, 360});
	backgroundSprite_->Update();
}
void HelpDisplay::Update() { 
	
}
void HelpDisplay::Draw() { backgroundSprite_->Draw(); }
void HelpDisplay::Open() { 
	

}
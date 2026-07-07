#include "PadMenu.h"
#include "TextureManager.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"
#include <numbers>
#include "WinApp.h"
namespace{
float padMenuSize = 500.0f;
Vector2 padMenuPos = {WinApp::kClientWidth/2.0f, WinApp::kClientHeight/2.0f};
}
PadMenu::PadMenu() { 
	backgroundSprite_ = std::make_unique<Sprite>(); 
	selectSprite_ = std::make_unique<Sprite>();
	flameSprite_ = std::make_unique<Sprite>();
}
PadMenu::~PadMenu(){};
void PadMenu::Initialize() {
	uint32_t backgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/Background.png");
	uint32_t selectTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/Select.png");
	uint32_t circleTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/CircleFlame.png");
	backgroundSprite_->Initialize(backgroundTextureHandle);
	backgroundSprite_->SetScale({padMenuSize,padMenuSize});
	backgroundSprite_->SetPosition(padMenuPos);
	backgroundSprite_->SetColor({0.3f, 0.3f, 0.3f, 0.5f});
	backgroundSprite_->SetAnchorPoint({0.5f, 0.5f});
	backgroundSprite_->Update();
	selectSprite_->Initialize(selectTextureHandle);
	selectSprite_->SetScale({padMenuSize, padMenuSize});
	selectSprite_->SetPosition(padMenuPos);
	selectSprite_->SetColor({1.0f, 1.0f, 1.0f, 0.5f});
	selectSprite_->SetAnchorPoint({0.5f, 0.5f});
	selectSprite_->Update();
	flameSprite_->Initialize(circleTextureHandle);
	flameSprite_->SetScale({padMenuSize,padMenuSize});
	flameSprite_->SetPosition(padMenuPos);
	flameSprite_->SetAnchorPoint({0.5f, 0.5});
	flameSprite_->Update();

}
void PadMenu::Update(){
	if (!PlayCommand::GetPadMenuOpen()) {
		return;
	}

	if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::UPRIGHT)) {
		selectRotation_ = std::numbers::pi_v<float> / 4.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::RIGHT)) {
		selectRotation_ = std::numbers::pi_v<float> / 2.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::DOWNRIGHT)) {
		selectRotation_ = std::numbers::pi_v<float>*3.0f / 4.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::DOWN)) {
		selectRotation_ = std::numbers::pi_v<float>;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::DOWNLEFT)) {
		selectRotation_ = std::numbers::pi_v<float>*3.0f / -4.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::LEFT)) {
		selectRotation_ = std::numbers::pi_v<float> / -2.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::UPLEFT)) {
		selectRotation_ = std::numbers::pi_v<float> / -4.0f;
	} else if (Input::GetInstance()->IsJoyStickSelectDirectionL(Input::JoyconStickDirection::UP)) {
		selectRotation_ = 0.0f;
	}

	selectSprite_->SetRotation(selectRotation_);
	selectSprite_->Update();
}
void PadMenu::Draw(){
	if (!PlayCommand::GetPadMenuOpen()) {
		return;
	}
	backgroundSprite_->Draw();
	selectSprite_->Draw();
	flameSprite_->Draw();
}
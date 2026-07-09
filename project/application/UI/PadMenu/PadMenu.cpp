#include "PadMenu.h"
#include "TextureManager.h"
#include "PlayCommand/PlayCommand.h"
#include "Input.h"
#include <numbers>
#include "WinApp.h"
#include <cmath>
namespace{
float padMenuSize = 500.0f;
Vector2 padMenuPos = {WinApp::kClientWidth/2.0f, WinApp::kClientHeight/2.0f};
float iconSize = 60.0f;
float iconDistance = 150.0f;
}
PadMenu::PadMenu() { 
	backgroundSprite_ = std::make_unique<Sprite>(); 
	selectSprite_ = std::make_unique<Sprite>();
	flameSprite_ = std::make_unique<Sprite>();

	teamDisplayIcon_ = std::make_unique<Sprite>();
	characterDisplayIcon_ = std::make_unique<Sprite>();
	helpIcon_ = std::make_unique<Sprite>();
}
PadMenu::~PadMenu(){};
void PadMenu::Initialize() {
	uint32_t backgroundTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/Background.png");
	uint32_t selectTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/Select.png");
	uint32_t circleTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/PadMenu/CircleFlame.png");
	uint32_t teamDisplayIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Team.png");
	uint32_t characterDisplayIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/CharacterDisplay.png");
	uint32_t helpIconHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Help.png");

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

	teamDisplayIcon_->Initialize(teamDisplayIconHandle);
	teamDisplayIcon_->SetScale({iconSize, iconSize});
	teamDisplayIcon_->SetPosition({iconDistance * sinf(0)+padMenuPos.x, iconDistance * -cosf(0)+padMenuPos.y});
	teamDisplayIcon_->SetAnchorPoint({0.5f, 0.5f});
	teamDisplayIcon_->Update();

	characterDisplayIcon_->Initialize(characterDisplayIconHandle);
	characterDisplayIcon_->SetScale({iconSize, iconSize});
	characterDisplayIcon_->SetPosition({iconDistance * sinf(std::numbers::pi_v<float> / 4.0f) + padMenuPos.x, iconDistance * -cosf(std::numbers::pi_v<float> / 4.0f) + padMenuPos.y});
	characterDisplayIcon_->SetAnchorPoint({0.5f, 0.5f});
	characterDisplayIcon_->Update();

	helpIcon_->Initialize(helpIconHandle);
	helpIcon_->SetScale({iconSize, iconSize});
	helpIcon_->SetPosition({iconDistance * sinf(std::numbers::pi_v<float> / 2.0f) + padMenuPos.x, iconDistance * -cosf(std::numbers::pi_v<float> / 2.0f) + padMenuPos.y});
	helpIcon_->SetAnchorPoint({0.5f, 0.5f});
	helpIcon_->Update();
}
void PadMenu::Update(){
	if (!PlayCommand::GetPadMenuOpen()) {
		return;
	}
	isMenuSelect = true;
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
	} else {
		isMenuSelect = false;
	}

	selectSprite_->SetRotation(selectRotation_);
	selectSprite_->Update();
}
void PadMenu::Draw(){
	if (!PlayCommand::GetPadMenuOpen()) {
		return;
	}
	backgroundSprite_->Draw();
	if (isMenuSelect) {
	selectSprite_->Draw();
	}
	teamDisplayIcon_->Draw();
	characterDisplayIcon_->Draw();
	helpIcon_->Draw();
	flameSprite_->Draw();
}
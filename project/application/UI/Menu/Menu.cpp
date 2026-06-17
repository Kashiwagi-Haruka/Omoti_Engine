#include "Menu.h"
#include "TextureManager.h"

namespace{
constexpr Vector2 kIconMargin = { 20.0f, 20.0f };
constexpr Vector2 kIconSize = {100.0f, 100.0f};

constexpr Vector2 kKeyIconSize = {100.0f, 40.0f};
}

Menu::Menu() { 
	pauseSprite_ = std::make_unique<Sprite>();
	characterDisplaySprite_ = std::make_unique<Sprite>();
	teamSelectSprite_ = std::make_unique<Sprite>();
	helpSprite_ = std::make_unique<Sprite>();

	pauseKeyboardSprite_ = std::make_unique<Sprite>();
	characterDisplayKeyboardSprite_ = std::make_unique<Sprite>();
	teamSelectKeyboardSprite_ = std::make_unique<Sprite>();
	helpKeyboardSprite_ = std::make_unique<Sprite>();
}
Menu::~Menu() {}
void Menu::Initialize() { 
	pauseSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Pause.png")); 
	characterDisplaySprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/CharacterDisplay.png"));
	teamSelectSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Team.png"));
	helpSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Help.png"));

	pauseKeyboardSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Keyboard/PauseKey.png"));
	characterDisplayKeyboardSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Keyboard/CharacterDisplayKey.png"));
	teamSelectKeyboardSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Keyboard/TeamKey.png"));
	helpKeyboardSprite_->Initialize(TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Menu/Keyboard/HelpKey.png"));

	pauseSprite_->SetScale(kIconSize);
	characterDisplaySprite_->SetScale(kIconSize);
	teamSelectSprite_->SetScale(kIconSize);
	helpSprite_->SetScale(kIconSize);

	pauseKeyboardSprite_->SetScale(kKeyIconSize);
	characterDisplayKeyboardSprite_->SetScale(kKeyIconSize);
	teamSelectKeyboardSprite_->SetScale(kKeyIconSize);
	helpKeyboardSprite_->SetScale(kKeyIconSize);
}
void Menu::Update() {}
void Menu::Draw() { 
	if (pauseSprite_){
		pauseSprite_->Draw(); 
	}
	if (characterDisplaySprite_) {
		characterDisplaySprite_->Draw();
	}
	if (teamSelectSprite_) {
		teamSelectSprite_->Draw();
	}
	if (helpKeyboardSprite_) {
		helpKeyboardSprite_->Draw();
	}
	if (pauseKeyboardSprite_) {
		pauseKeyboardSprite_->Draw();
	}
	if (characterDisplayKeyboardSprite_) {
		characterDisplayKeyboardSprite_->Draw();
	}
	if (teamSelectKeyboardSprite_) {
		teamSelectKeyboardSprite_->Draw();
	}
	if (helpKeyboardSprite_) {
		helpKeyboardSprite_->Draw();
	}
}
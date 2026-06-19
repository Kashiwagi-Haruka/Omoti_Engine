#include "Menu.h"
#include "TextureManager.h"

namespace{
constexpr Vector2 kIconMargin = { 20.0f, 20.0f };
constexpr Vector2 kIconSize = {100.0f, 100.0f};

constexpr Vector2 kKeyIconSize = {100.0f, 40.0f};
constexpr float kKeyMarginY = 20.0f;
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
	pauseSprite_->SetPosition({kIconMargin.x, kIconMargin.y});
	characterDisplaySprite_->SetScale(kIconSize);
	characterDisplaySprite_->SetPosition({1280.0f - kIconMargin.x, kIconMargin.y});
	teamSelectSprite_->SetScale(kIconSize);
	teamSelectSprite_->SetPosition({characterDisplaySprite_->GetPosition().x - (kIconSize.x + kIconMargin.x),kIconMargin.y});
	helpSprite_->SetScale(kIconSize);
	helpSprite_->SetPosition({teamSelectSprite_->GetPosition().x - (kIconSize.x + kIconMargin.x), kIconMargin.y});

	pauseKeyboardSprite_->SetScale(kKeyIconSize);
	pauseKeyboardSprite_->SetPosition({pauseSprite_->GetPosition().x, pauseSprite_->GetPosition().y + kKeyMarginY});
	characterDisplayKeyboardSprite_->SetScale(kKeyIconSize);
	characterDisplayKeyboardSprite_->SetPosition({characterDisplaySprite_->GetPosition().x, characterDisplaySprite_->GetPosition().y + kKeyMarginY});
	teamSelectKeyboardSprite_->SetScale(kKeyIconSize);
	teamSelectKeyboardSprite_->SetPosition({teamSelectSprite_->GetPosition().x, teamSelectSprite_->GetPosition().y + kKeyMarginY});
	helpKeyboardSprite_->SetScale(kKeyIconSize);
	helpKeyboardSprite_->SetPosition({helpKeyboardSprite_->GetPosition().x, helpKeyboardSprite_->GetPosition().y + kKeyMarginY});

	pauseSprite_->Update();
	characterDisplaySprite_->Update();
	teamSelectSprite_->Update();
	helpSprite_->Update();
	pauseKeyboardSprite_->Update();
	characterDisplayKeyboardSprite_->Update();
	teamSelectKeyboardSprite_->Update();
	helpKeyboardSprite_->Update();
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
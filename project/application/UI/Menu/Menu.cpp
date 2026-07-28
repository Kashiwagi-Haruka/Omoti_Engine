#include "Menu.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
namespace{
constexpr Vector2 kIconMargin = { 20.0f, 20.0f };
constexpr Vector2 kIconSize = {40.0f, 40.0f};
constexpr Vector2 kSelectIconSize = {45.0f, 45.0f};

constexpr Vector2 kKeyIconSize = {60.0f, 32.0f};
constexpr float kKeyMarginY = 20.0f;
constexpr Vector2 kKeyAncor = {1.0f, 0.0f};
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

	characterDisplaySprite_->SetScale(kIconSize);
	characterDisplaySprite_->SetPosition({WinApp::kClientWidth - (kIconMargin.x), kIconMargin.y});
	characterDisplaySprite_->SetAnchorPoint({1.0f, 0.0f});

	teamSelectSprite_->SetScale(kIconSize);
	teamSelectSprite_->SetPosition({characterDisplaySprite_->GetPosition().x - (kIconSize.x + kIconMargin.x),kIconMargin.y});
	teamSelectSprite_->SetAnchorPoint({1.0f, 0.0f});

	helpSprite_->SetScale(kIconSize);
	helpSprite_->SetPosition({teamSelectSprite_->GetPosition().x - (kIconSize.x + kIconMargin.x), kIconMargin.y});
	helpSprite_->SetAnchorPoint({1.0f, 0.0f});

	pauseSprite_->SetScale(kIconSize);
	pauseSprite_->SetPosition({helpSprite_->GetPosition().x - (kIconSize.x + kIconMargin.x), kIconMargin.y});
	pauseSprite_->SetAnchorPoint({0.0f, 0.0f});

	pauseKeyboardSprite_->SetScale(kKeyIconSize);
	pauseKeyboardSprite_->SetPosition({pauseSprite_->GetPosition().x, pauseSprite_->GetPosition().y + kKeyMarginY + (kIconSize.y / 2.0f)});
	pauseKeyboardSprite_->SetAnchorPoint({0.0f,0.0f});
	
	characterDisplayKeyboardSprite_->SetScale(kKeyIconSize);
	characterDisplayKeyboardSprite_->SetPosition({characterDisplaySprite_->GetPosition().x, characterDisplaySprite_->GetPosition().y + kKeyMarginY + (kIconSize.y / 2.0f)});
	characterDisplayKeyboardSprite_->SetAnchorPoint(kKeyAncor);

	teamSelectKeyboardSprite_->SetScale(kKeyIconSize);
	teamSelectKeyboardSprite_->SetPosition({teamSelectSprite_->GetPosition().x, teamSelectSprite_->GetPosition().y + kKeyMarginY + (kIconSize.y / 2.0f)});
	teamSelectKeyboardSprite_->SetAnchorPoint(kKeyAncor);

	helpKeyboardSprite_->SetScale(kKeyIconSize);
	helpKeyboardSprite_->SetPosition({helpSprite_->GetPosition().x, helpSprite_->GetPosition().y + kKeyMarginY + (kIconSize.y / 2.0f)});
	helpKeyboardSprite_->SetAnchorPoint(kKeyAncor);

	pauseSprite_->Update();
	characterDisplaySprite_->Update();
	teamSelectSprite_->Update();
	helpSprite_->Update();
	pauseKeyboardSprite_->Update();
	characterDisplayKeyboardSprite_->Update();
	teamSelectKeyboardSprite_->Update();
	helpKeyboardSprite_->Update();
}
void Menu::Update() {

	UpdateIcon(pauseSprite_.get(),pauseKeyboardSprite_.get()); 
	UpdateIcon(characterDisplaySprite_.get(),characterDisplayKeyboardSprite_.get()); 
	UpdateIcon(teamSelectSprite_.get(),teamSelectKeyboardSprite_.get()); 
	UpdateIcon(helpSprite_.get(),helpKeyboardSprite_.get()); 

}
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
	if (helpSprite_) {
		helpSprite_->Draw();
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
void Menu::UpdateIcon(Sprite* sprite,Sprite* keySprite){ 
	if (sprite->GetPosition().x -(kIconSize.x*sprite->GetAnchorPoint().x) <= Input::GetInstance()->GetMouseX()) {
		if (sprite->GetPosition().x + (kIconSize.x * (1.0f-sprite->GetAnchorPoint().x)) >= Input::GetInstance()->GetMouseX()) {
			if (sprite->GetPosition().y - (kIconSize.y * sprite->GetAnchorPoint().y) <= Input::GetInstance()->GetMouseY()) {
				if (sprite->GetPosition().y + (kIconSize.y * (1.0f-sprite->GetAnchorPoint().y)) >= Input::GetInstance()->GetMouseY()) {
					sprite->SetScale(kSelectIconSize);
					sprite->Update();
					if (PlayCommand::GetDESIDE()) {
						keySprite->SetColor({1.0f, 1.0f, 0.0f, 1.0f});
						keySprite->Update();
					}
					return;
				}
			}
		}
	}
	sprite->SetScale(kIconSize);
	sprite->Update();
	keySprite->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	keySprite->Update();
	return;
}
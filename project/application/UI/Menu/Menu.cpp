#include "Menu.h"
#include "TextureManager.h"
Menu::Menu() { 
	pauseSprite_ = std::make_unique<Sprite>();
	characterDisplaySprite_ = std::make_unique<Sprite>();
	teamSelectSprite_ = std::make_unique<Sprite>();
}
Menu::~Menu() {}
void Menu::Initialize() {
	
}
void Menu::Update() {}
void Menu::Draw() {}
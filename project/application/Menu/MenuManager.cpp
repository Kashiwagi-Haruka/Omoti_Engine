#include "MenuManager.h"
#include "PlayCommand/PlayCommand.h"

MenuManager::MenuManager() { 
	characterDisplay_ = std::make_unique<CharacterDisplay>(); 
	pause_ = std::make_unique<Pause>();
}
void MenuManager::SetCharacter(Object3d* obj, Attribute attribute) { 
	pause_->SetCurrentAttribute(attribute);
	pause_->SetCurrentCharacterObj(obj);
}
void MenuManager::LoadTeamDisplay() {
	if (teamDisplay_) {
		return;
	}
	teamDisplay_ = std::make_unique<TeamDisplay>();
	teamDisplay_->Initialize(*team_);
}

void MenuManager::UnloadTeamDisplay() {
	if (!teamDisplay_) {
		return;
	}
	teamDisplay_->Unload();
	teamDisplay_.reset();
}
void MenuManager::Initialize(const Team &team) {

	characterDisplay_->Initialize(team);
	pause_->Initialize();


}

void MenuManager::Update() {
	switch (menuType_) {
	case MenuManager::Menu::NONE:
		if (PlayCommand::GetCharacterDisplay()) {
			menuType_ = Menu::CharacterDisplay;
			Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight_);
		}
		if (PlayCommand::GetPause()) {
			menuType_ = Menu::Pause;
		}
		if (PlayCommand::GetTeamSelectDisplay()) {
			menuType_ = Menu::TeamDisplay;
		}
		return;
		break;
	case MenuManager::Menu::Pause:
		pause_->Update();
		if (PlayCommand::GetESCAPE()) {
			menuType_ = Menu::NONE;
		}
		
		break;
	case MenuManager::Menu::CharacterDisplay:
		characterDisplay_->Update();
		if (PlayCommand::GetESCAPE()) {
			menuType_ = Menu::NONE;
		}
		break;
	case MenuManager::Menu::TeamDisplay:
		
		if (PlayCommand::GetESCAPE()) {
			menuType_ = Menu::NONE;
		}

		break;
	default:
		break;
	}
}

void MenuManager::Draw() {
	switch (menuType_) {
	case MenuManager::Menu::NONE:
		return;
		break;
	case MenuManager::Menu::Pause:

		break;
	case MenuManager::Menu::CharacterDisplay:
		break;
	default:
		break;
	}
}
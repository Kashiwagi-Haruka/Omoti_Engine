#include "MenuManager.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "SpriteCommon.h"

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
void MenuManager::Initialize(Team& team) {
	team_ = &team;
	menuType_ = Menu::NONE;
	characterDisplay_->Initialize(team);
	pause_->Initialize();
}

void MenuManager::Update() {
	switch (menuType_) {
	case MenuManager::Menu::NONE:
		if (PlayCommand::GetInstance()->GetCharacterDisplay()) {
			menuType_ = Menu::CharacterDisplay;
		} else if (PlayCommand::GetInstance()->GetPause()) {
			menuType_ = Menu::Pause;
			pause_->Start();
		} else if (PlayCommand::GetInstance()->GetTeamSelectDisplay()) {
			menuType_ = Menu::TeamDisplay;
			LoadTeamDisplay();
			Input::GetInstance()->SetIsCursorStability(false);
			Input::GetInstance()->SetIsCursorVisible(true);
		} else {
			pause_->Update();
		}
		break;
	case MenuManager::Menu::Pause:
		pause_->Update();
		if (!pause_->IsPause()) {
			menuType_ = Menu::NONE;
		}
		break;
	case MenuManager::Menu::CharacterDisplay:
		characterDisplay_->Update();
		if (PlayCommand::GetInstance()->GetESCAPE() || PlayCommand::GetInstance()->GetCharacterDisplay()) {
			menuType_ = Menu::NONE;
		}
		break;
	case MenuManager::Menu::TeamDisplay:
		teamDisplay_->Update(*team_);
		if ((!PlayCommand::GetInstance()->GetTeamSelectDisplay() && (PlayCommand::GetInstance()->GetESCAPE() || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonB))) ||
		    PlayCommand::GetInstance()->GetTeamSelectDisplay()) {
			Close();
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
		pause_->Draw();
		break;
	case MenuManager::Menu::CharacterDisplay:
		characterDisplay_->Draw();
		break;
	case MenuManager::Menu::TeamDisplay:
		SpriteCommon::GetInstance()->DrawCommon();
		teamDisplay_->Draw(*team_);
		break;
	default:
		break;
	}
}

void MenuManager::Close() {
	if (menuType_ == Menu::TeamDisplay) {
		UnloadTeamDisplay();
	}
	menuType_ = Menu::NONE;
}

bool MenuManager::IsTitleBack() const { return pause_->IsTitleBack(); }
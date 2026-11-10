#include "MenuManager.h"

#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "SpriteCommon.h"

MenuManager::MenuManager() {
	characterDisplay_ = std::make_unique<CharacterDisplay>();
	pause_ = std::make_unique<Pause>();
	helpDisplay_ = std::make_unique<HelpDisplay>();
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

	// HelpDisplayは初めて開くときに初期化する。
	// 前回提示したHelpDisplay::Open()がInitialize()を呼ぶ。
}

void MenuManager::OpenHelpDisplay() {
	// ほかのメニューを表示中なら開かない。
	if (menuType_ != Menu::NONE) {
		return;
	}

	// 初期化前の呼び出しは受け付けない。
	if (!team_) {
		return;
	}

	helpDisplay_->Open();
	menuType_ = Menu::HelpDisplay;

	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
}

void MenuManager::Update() {
	auto* input = Input::GetInstance();
	auto* command = PlayCommand::GetInstance();

	switch (menuType_) {
	case Menu::NONE:
		if (command->GetCharacterDisplay()) {
			menuType_ = Menu::CharacterDisplay;
		} else if (command->GetPause()) {
			menuType_ = Menu::Pause;
			pause_->Start();
		} else if (command->GetTeamSelectDisplay()) {
			menuType_ = Menu::TeamDisplay;

			LoadTeamDisplay();

			input->SetIsCursorStability(false);
			input->SetIsCursorVisible(true);
		} else if (input->TriggerKey(DIK_H)) {
			OpenHelpDisplay();
		} else {
			pause_->Update();
		}
		break;

	case Menu::Pause:
		pause_->Update();

		if (!pause_->IsPause()) {
			menuType_ = Menu::NONE;
		}
		break;

	case Menu::CharacterDisplay:
		characterDisplay_->Update();

		if (command->GetESCAPE() || command->GetCharacterDisplay()) {
			menuType_ = Menu::NONE;
		}
		break;

	case Menu::TeamDisplay:
		teamDisplay_->Update(*team_);

		if ((!command->GetTeamSelectDisplay() && (command->GetESCAPE() || input->TriggerButton(Input::PadButton::kButtonB))) || command->GetTeamSelectDisplay()) {
			Close();
		}
		break;

	case Menu::HelpDisplay:
		// GameScene側でも毎フレームカーソル状態を設定しているため、
		// ヒント表示中はここでも表示・固定解除を要求する。
		input->SetIsCursorStability(false);
		input->SetIsCursorVisible(true);

		if (input->TriggerKey(DIK_H) || command->GetESCAPE()) {
			Close();
			break;
		}

		helpDisplay_->Update();

		// HelpDisplay内の「戻る」から閉じた場合も、
		// MenuManagerの状態を解除する。
		if (!helpDisplay_->IsOpen()) {
			Close();
		}
		break;

	default:
		break;
	}
}

void MenuManager::Draw() {
	switch (menuType_) {
	case Menu::NONE:
		return;

	case Menu::Pause:
		pause_->Draw();
		break;

	case Menu::CharacterDisplay:
		characterDisplay_->Draw();
		break;

	case Menu::TeamDisplay:
		SpriteCommon::GetInstance()->DrawCommon();
		teamDisplay_->Draw(*team_);
		break;

	case Menu::HelpDisplay:
		SpriteCommon::GetInstance()->DrawCommon();
		helpDisplay_->Draw();
		break;

	default:
		break;
	}
}

void MenuManager::Close() {
	if (menuType_ == Menu::TeamDisplay) {
		UnloadTeamDisplay();
	}

	if (menuType_ == Menu::HelpDisplay) {
		helpDisplay_->Close();

		// ゲーム画面に戻る。
		// Altを押している場合はカーソル表示を維持する。
		auto* input = Input::GetInstance();

		const bool altHeld = input->PushKey(DIK_LMENU) || input->PushKey(DIK_RMENU);

		input->SetIsCursorStability(!altHeld);
		input->SetIsCursorVisible(altHeld);
	}

	menuType_ = Menu::NONE;
}

bool MenuManager::IsTitleBack() const { return pause_->IsTitleBack(); }
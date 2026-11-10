#pragma once

#include "CharacterDisplay/CharacterDisplay.h"
#include "HelpDisplay/HelpDisplay.h"
#include "Pause/Pause.h"
#include "TeamDisplay/TeamDisplay.h"

#include <memory>

class MenuManager {
private:
	std::unique_ptr<CharacterDisplay> characterDisplay_;
	std::unique_ptr<Pause> pause_;
	std::unique_ptr<TeamDisplay> teamDisplay_;
	std::unique_ptr<HelpDisplay> helpDisplay_;

	Team* team_ = nullptr;

	enum class Menu { NONE, Pause, CharacterDisplay, TeamDisplay, HelpDisplay };

	Menu menuType_ = Menu::NONE;

	void LoadTeamDisplay();
	void UnloadTeamDisplay();

public:
	MenuManager();

	void Initialize(Team& team);
	void Update();
	void Draw();

	void SetCharacter(Object3d* obj, Attribute attribute);

	void OpenHelpDisplay();
	void Close();

	bool IsActive() const { return menuType_ != Menu::NONE; }

	bool IsTitleBack() const;
};
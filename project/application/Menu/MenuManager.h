#pragma once
#include "CharacterDisplay/CharacterDisplay.h"
#include "Pause/Pause.h"
#include "TeamDisplay/TeamDisplay.h"
#include <memory>

class MenuManager {

	std::unique_ptr<CharacterDisplay> characterDisplay_;
	std::unique_ptr<Pause> pause_;
	std::unique_ptr<TeamDisplay> teamDisplay_;
	Team* team_ = nullptr;

	enum class Menu {
		// メニュー状態じゃない
		NONE,
		// ポーズ
		Pause,
		// キャラクターディスプレイ
		CharacterDisplay,
		// チームディスプレイ
		TeamDisplay
	};

	Menu menuType_ = Menu::NONE;
	Pause::Action action_ = Pause::Action::kNone;

	void LoadTeamDisplay();
	void UnloadTeamDisplay();

public:
	MenuManager();
	void Initialize(Team& team);
	void Update();
	void Draw();
	void SetCharacter(Object3d* obj, Attribute attribute);
	void Close();
	bool IsActive() const { return menuType_ != Menu::NONE; }
	Pause::Action ConsumeAction();
};
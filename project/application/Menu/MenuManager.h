#pragma once
#include <memory>
#include "CharacterDisplay/CharacterDisplay.h"
#include "Pause/Pause.h"
#include "TeamDisplay/TeamDisplay.h"

class MenuManager {

	std::unique_ptr<CharacterDisplay> characterDisplay_;
	std::unique_ptr<Pause> pause_;
	std::unique_ptr<TeamDisplay> teamDisplay_;

	enum class Menu{
		// メニュー状態じゃない
		NONE,
		// ポーズ
		Pause,
		// キャラクターディスプレイ
		CharacterDisplay,
		// チームディスプレイ
		TeamDisplay
	};

	Menu menuType_;


	void LoadTeamDisplay();
	void UnloadTeamDisplay();

public:
	MenuManager();
	void Initialize(const Team& team);
	void Update();
	void Draw();
	void SetCharacter(Object3d* obj, Attribute attribute);
};

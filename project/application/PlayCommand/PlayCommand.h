#pragma once
class PlayCommand {

	enum Command{
		DESIDE,//決定
		JUMP,   // ジャンプ
		DASH,   // ダッシュ
		NORMAL_ATTACK, // 通常攻撃
		SKILL_ATTACK,  // スキル攻撃
		SPECIAL_ATTACK, // 必殺技
	};

	bool GetInput(Command& command);



};

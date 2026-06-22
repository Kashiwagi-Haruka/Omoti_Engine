#pragma once
class PlayCommand {

	public:
	static bool GetMOVE_FRONT(); // 前移動
	static bool GetMOVE_BACK();   // 後移動
	static bool GetMOVE_LEFT();   // 左移動
	static bool GetMOVE_RIGHT();   // 右移動
	static bool GetJUMP();         // ジャンプ
	static bool GetDASH();          // ダッシュ
	static bool GetNORMAL_ATTACK_PUSH(); // 通常攻撃長押し
	static bool GetNORMAL_ATTACK_TRIGGER(); // 通常攻撃単押し
	static bool GetNORMAL_ATTACK_RELEASE(); // 通常攻撃離したとき
	static bool GetSKILL_ATTACK();   // スキル攻撃
	static bool GetSPECIAL_ATTACK(); // 必殺攻撃
	static bool GetDESIDE();          // 決定
	static bool GetESCAPE();          // キャンセル
	static bool GetCURSOR_DISPLAY();  // カーソル表示

};

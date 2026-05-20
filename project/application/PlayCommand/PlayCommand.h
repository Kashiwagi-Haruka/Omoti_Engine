#pragma once
class PlayCommand {

	bool GetMOVE_FRONT(); // 前移動
	bool GetMOVE_BACK();  // 後移動
	bool GetMOVE_LEFT();  // 左移動
	bool GetMOVE_RIGHT(); // 右移動
	bool GetJUMP();       // ジャンプ
	bool GetDASH();       // ダッシュ
	bool GetNORMAL_ATTACK(); // 通常攻撃
	bool GetSKILL_ATTACK();  // スキル攻撃
	bool GetSPECIAL_ATTACK(); // 必殺攻撃
	bool GetDESIDE();         // 決定
	bool GetESCAPE();         // キャンセル


};

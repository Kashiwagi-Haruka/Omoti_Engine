#pragma once
class PlayCommand {

	public:
	/// <summary>
	/// 前移動
	/// </summary>
	/// <returns></returns>
	static bool GetMOVE_FRONT();

	/// <summary>
	/// 後移動
	/// </summary>
	/// <returns></returns>
	static bool GetMOVE_BACK();

	/// <summary>
	/// 左移動
	/// </summary>
	/// <returns></returns>
	static bool GetMOVE_LEFT();

	/// <summary>
	/// 右移動
	/// </summary>
	/// <returns></returns>
	static bool GetMOVE_RIGHT();

	/// <summary>
	/// ジャンプ
	/// </summary>
	/// <returns></returns>
	static bool GetJUMP();  

	/// <summary>
	/// ダッシュ
	/// </summary>
	/// <returns></returns>
	static bool GetDASH();

	/// <summary>
	/// 通常攻撃長押し
	/// </summary>
	/// <returns></returns>
	static bool GetNORMAL_ATTACK_PUSH();

	/// <summary>
	/// 通常攻撃単押し
	/// </summary>
	/// <returns></returns>
	static bool GetNORMAL_ATTACK_TRIGGER();

	/// <summary>
	/// 通常攻撃離したとき
	/// </summary>
	/// <returns></returns>
	static bool GetNORMAL_ATTACK_RELEASE();

	/// <summary>
	/// スキル攻撃
	/// </summary>
	/// <returns></returns>
	static bool GetSKILL_ATTACK();   

	/// <summary>
	/// 必殺攻撃
	/// </summary>
	/// <returns></returns>
	static bool GetSPECIAL_ATTACK();

	/// <summary>
	/// 決定
	/// </summary>
	/// <returns></returns>
	static bool GetDESIDE();

	/// <summary>
	/// キャンセル
	/// </summary>
	/// <returns></returns>
	static bool GetESCAPE();          

	/// <summary>
	/// キャンセル
	/// </summary>
	/// <returns></returns>
	static bool GetPause();

	/// <summary>
	/// カーソル表示
	/// </summary>
	/// <returns></returns>
	static bool GetCURSOR_DISPLAY();

	/// <summary>
	/// キャラクターチェンジ1
	/// </summary>
	/// <returns></returns>
	static bool GetCharacterChange1();

	/// <summary>
	/// キャラクターチェンジ2
	/// </summary>
	/// <returns></returns>
	static bool GetCharacterChange2();

	/// <summary>
	/// キャラクターチェンジ3
	/// </summary>
	/// <returns></returns>
	static bool GetCharacterChange3();

	/// <summary>
	/// キャラクターチェンジ4
	/// </summary>
	/// <returns></returns>
	static bool GetCharacterChange4();
};

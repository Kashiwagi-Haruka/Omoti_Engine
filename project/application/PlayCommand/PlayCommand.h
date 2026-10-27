#pragma once
#include <memory>
class PlayCommand {

	// シングルトン本体
	static std::unique_ptr<PlayCommand> instance_;

	// ポーズ中か
	bool isPause_ = false;


	public:
	// シングルトン取得
	static PlayCommand* GetInstance();
	/// <summary>
	/// 前移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_FRONT();

	/// <summary>
	/// 後移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_BACK();

	/// <summary>
	/// 左移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_LEFT();

	/// <summary>
	/// 右移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_RIGHT();

	/// <summary>
	/// ジャンプ
	/// </summary>
	/// <returns></returns>
	bool GetJUMP();  

	/// <summary>
	/// ダッシュ
	/// </summary>
	/// <returns></returns>
	bool GetDASH();

	/// <summary>
	/// ダッシュトリガー
	/// </summary>
	/// <returns></returns>
	bool GetDASH_TRIGGER();

	/// <summary>
	/// 通常攻撃長押し
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_PUSH();

	/// <summary>
	/// 通常攻撃単押し
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_TRIGGER();

	/// <summary>
	/// 通常攻撃離したとき
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_RELEASE();

	/// <summary>
	/// スキル攻撃
	/// </summary>
	/// <returns></returns>
	bool GetSKILL_ATTACK();   

	/// <summary>
	/// 必殺攻撃
	/// </summary>
	/// <returns></returns>
	bool GetSPECIAL_ATTACK();

	/// <summary>
	/// 決定
	/// </summary>
	/// <returns></returns>
	bool GetDESIDE();

	/// <summary>
	/// キャンセル
	/// </summary>
	/// <returns></returns>
	bool GetESCAPE();          

	/// <summary>
	/// ポーズ切り替え
	/// </summary>
	/// <returns></returns>
	bool GetPause();

	/// <summary>
	/// カーソル表示
	/// </summary>
	/// <returns></returns>
	bool GetCURSOR_DISPLAY();

	/// <summary>
	/// キャラクターチェンジ1
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange1();

	/// <summary>
	/// キャラクターチェンジ2
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange2();

	/// <summary>
	/// キャラクターチェンジ3
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange3();

	/// <summary>
	/// キャラクターチェンジ4
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange4();

	/// <summary>
	/// チーム画面切り替え
	/// </summary>
	/// <returns></returns>
	bool GetTeamSelectDisplay();

	/// <summary>
	/// キャラクター画面切り替え
	/// </summary>
	/// <returns></returns>
	bool GetCharacterDisplay();

	/// <summary>
	/// パッドのメニュー開いているか
	/// </summary>
	/// <returns></returns>
	bool GetPadMenuOpen();

	/// <summary>
	/// 上入力したかUI
	/// </summary>
	/// <returns></returns>
	bool GetUpUI();
	
	/// <summary>
	/// 下入力したかUI
	/// </summary>
	/// <returns></returns>
	bool GetDownUI();
};

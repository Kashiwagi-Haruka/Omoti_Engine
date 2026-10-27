#pragma once
#include <memory>
class PlayCommand {

	// シングルトン本体
	static std::unique_ptr<PlayCommand> instance_;

	// ポーズ中か
	bool isPause_ = false;


	public:

	void Finalize();
	// シングルトン取得
	static PlayCommand* GetInstance();
	/// <summary>
	/// 前移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_FRONT()const;

	/// <summary>
	/// 後移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_BACK()const;

	/// <summary>
	/// 左移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_LEFT()const;

	/// <summary>
	/// 右移動
	/// </summary>
	/// <returns></returns>
	bool GetMOVE_RIGHT()const;

	/// <summary>
	/// ジャンプ
	/// </summary>
	/// <returns></returns>
	bool GetJUMP()const;  

	/// <summary>
	/// ダッシュ
	/// </summary>
	/// <returns></returns>
	bool GetDASH()const;

	/// <summary>
	/// ダッシュトリガー
	/// </summary>
	/// <returns></returns>
	bool GetDASH_TRIGGER()const;

	/// <summary>
	/// 通常攻撃長押し
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_PUSH()const;

	/// <summary>
	/// 通常攻撃単押し
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_TRIGGER()const;

	/// <summary>
	/// 通常攻撃離したとき
	/// </summary>
	/// <returns></returns>
	bool GetNORMAL_ATTACK_RELEASE()const;

	/// <summary>
	/// スキル攻撃
	/// </summary>
	/// <returns></returns>
	bool GetSKILL_ATTACK()const;   

	/// <summary>
	/// 必殺攻撃
	/// </summary>
	/// <returns></returns>
	bool GetSPECIAL_ATTACK()const;

	/// <summary>
	/// 決定
	/// </summary>
	/// <returns></returns>
	bool GetDESIDE()const;

	/// <summary>
	/// キャンセル
	/// </summary>
	/// <returns></returns>
	bool GetESCAPE()const;          

	/// <summary>
	/// ポーズ切り替え
	/// </summary>
	/// <returns></returns>
	bool GetPause()const;

	/// <summary>
	/// カーソル表示
	/// </summary>
	/// <returns></returns>
	bool GetCURSOR_DISPLAY()const;

	/// <summary>
	/// キャラクターチェンジ1
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange1()const;

	/// <summary>
	/// キャラクターチェンジ2
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange2()const;

	/// <summary>
	/// キャラクターチェンジ3
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange3()const;

	/// <summary>
	/// キャラクターチェンジ4
	/// </summary>
	/// <returns></returns>
	bool GetCharacterChange4()const;

	/// <summary>
	/// チーム画面切り替え
	/// </summary>
	/// <returns></returns>
	bool GetTeamSelectDisplay()const;

	/// <summary>
	/// キャラクター画面切り替え
	/// </summary>
	/// <returns></returns>
	bool GetCharacterDisplay()const;

	/// <summary>
	/// パッドのメニュー開いているか
	/// </summary>
	/// <returns></returns>
	bool GetPadMenuOpen()const;

	/// <summary>
	/// 上入力したかUI
	/// </summary>
	/// <returns></returns>
	bool GetUpUI()const;
	
	/// <summary>
	/// 下入力したかUI
	/// </summary>
	/// <returns></returns>
	bool GetDownUI()const;
};

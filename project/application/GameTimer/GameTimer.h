#pragma once
#include <memory>
class GameTimer {

	// ゲーム内の経過時間(秒)を保持するタイマー。
	float timer_ = 0.0f;
	// シングルトンの実体。
	static std::unique_ptr<GameTimer> instance;

public:
	/// <summary>
	/// インスタンス取得。
	/// </summary>
	/// <returns></returns>
	static GameTimer* GetInstance();

	/// <summary>
	/// タイマーリセット
	/// </summary>
	void Reset();

	/// <summary>
	/// 1フレーム分の時間を加算する。
	/// </summary>
	void Update();

	/// <summary>
	/// 現在のタイマー値(秒)を取得する。
	/// </summary>
	/// <returns></returns>
	float GetTimer() { return timer_; };
};
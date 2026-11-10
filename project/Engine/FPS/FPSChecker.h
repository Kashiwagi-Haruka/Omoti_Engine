#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <thread>

#include "Engine/Texture/Text/Text.h"

class FPSChecker final {
public:
	FPSChecker() = default;
	~FPSChecker();

	FPSChecker(const FPSChecker&) = delete;
	FPSChecker& operator=(const FPSChecker&) = delete;

	// GameBaseの初期化後、メインスレッドから呼ぶ。
	void Initialize();

	// 終了時に集計スレッドを停止する。
	void Finalize();

	// EndFlame後、完成したフレームごとに1回呼ぶ。
	void CountFrame();

	// メインスレッドから呼ぶ。
	void Draw();

	void SetVisible(bool visible) { visible_ = visible; }
	void ToggleVisible() { visible_ = !visible_; }
	bool IsVisible() const { return visible_; }

	// 初回集計前は-1。
	double GetAverageFPS() const { return averageFPS_.load(std::memory_order_relaxed); }

private:
	using Clock = std::chrono::steady_clock;

	void MeasureLoop();

	Text labelText_;
	Text valueText_;

	bool initialized_ = false;
	bool visible_ = true;

	// Textへ前回反映した状態。
	double displayedFPS_ = -2.0;
	float displayedWidth_ = -1.0f;

	std::atomic<double> averageFPS_{-1.0};

	std::thread worker_;
	std::mutex mutex_;
	std::condition_variable wakeup_;

	// 以下の値はmutex_で保護する。
	bool stopRequested_ = false;
	std::uint64_t frameCount_ = 0;
	Clock::time_point windowStart_{};
};
#include "FPSChecker.h"

#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

#include "GameBase.h"

FPSChecker::~FPSChecker() { Finalize(); }

void FPSChecker::Initialize() {
	if (initialized_) {
		return;
	}

	// FPS専用のフォントを作成する。
	// 他のTextで使うフォントのサイズに影響を与えない。
	const uint32_t fontHandle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Bold.ttf", 0);

	FreeTypeManager::SetPixelSizes(fontHandle, 18, 18);

	labelText_.Initialize(fontHandle);
	labelText_.SetAlign(TextAlign::Left);
	labelText_.SetSize({160.0f, 40.0f});
	labelText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	labelText_.SetString(U"FPS");

	valueText_.Initialize(fontHandle);
	valueText_.SetAlign(TextAlign::Left);
	valueText_.SetSize({110.0f, 40.0f});
	valueText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	valueText_.SetString(U"--");

	visible_ = true;
	displayedFPS_ = -2.0;
	displayedWidth_ = -1.0f;
	averageFPS_.store(-1.0, std::memory_order_relaxed);

	{
		std::lock_guard<std::mutex> lock(mutex_);
		stopRequested_ = false;
		frameCount_ = 0;
		windowStart_ = Clock::now();
	}

	worker_ = std::thread(&FPSChecker::MeasureLoop, this);
	initialized_ = true;
}

void FPSChecker::Finalize() {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		stopRequested_ = true;
	}

	// 1秒の待機中でも、すぐ終了できるように起こす。
	wakeup_.notify_one();

	if (worker_.joinable()) {
		worker_.join();
	}

	initialized_ = false;
}

void FPSChecker::CountFrame() {
	if (!initialized_) {
		return;
	}

	std::lock_guard<std::mutex> lock(mutex_);
	++frameCount_;
}

void FPSChecker::MeasureLoop() {
	std::unique_lock<std::mutex> lock(mutex_);

	while (!stopRequested_) {
		const auto deadline = windowStart_ + std::chrono::seconds(1);

		// 待機中はmutex_が解放され、CountFrameできる。
		const bool stopped = wakeup_.wait_until(lock, deadline, [this] { return stopRequested_; });

		if (stopped) {
			break;
		}

		const auto now = Clock::now();
		const double elapsed = std::chrono::duration<double>(now - windowStart_).count();

		// 待機時間の誤差も含め、実際の秒数で割る。
		const double fps = static_cast<double>(frameCount_) / elapsed;

		frameCount_ = 0;
		windowStart_ = now;

		averageFPS_.store(fps, std::memory_order_relaxed);
	}
}

void FPSChecker::Draw() {
	if (!initialized_ || !visible_) {
		return;
	}

	const double fps = GetAverageFPS();
	const float screenWidth = GameBase::GetInstance()->GetScreenWidth();

	const bool valueChanged = fps != displayedFPS_;
	const bool positionChanged = screenWidth != displayedWidth_;

	if (valueChanged) {
		if (fps < 0.0) {
			// 最初の1秒は計測待ち。
			valueText_.SetString(U"--");
			valueText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		} else {
			// 小数第1位まで表示する。
			std::ostringstream stream;
			stream.imbue(std::locale::classic());
			stream << std::fixed << std::setprecision(1) << fps;

			const std::string number = stream.str();
			valueText_.SetString(std::u32string(number.begin(), number.end()));

			// 色は丸める前の平均FPSで判定する。
			if (fps >= 50.0) {
				valueText_.SetColor({0.0f, 1.0f, 0.0f, 1.0f});
			} else if (fps >= 30.0) {
				valueText_.SetColor({1.0f, 1.0f, 0.0f, 1.0f});
			} else {
				valueText_.SetColor({1.0f, 0.0f, 0.0f, 1.0f});
			}
		}

		displayedFPS_ = fps;
	}

	if (positionChanged) {
		// 幅160pxの表示領域。
		// 現在のText実装に合わせて左寄せで配置する。
		const float left = screenWidth - 100.0f;
		constexpr float y = 32.0f;

		labelText_.SetPosition({left, y});
		valueText_.SetPosition({left + 44.0f, y});

		displayedWidth_ = screenWidth;
	}

	// SetString / SetPositionだけではレイアウトが変わらない。
	// falseを指定すると、タイプ表示せず全文を表示する。
	if (positionChanged) {
		labelText_.UpdateLayout(false);
	}

	if (valueChanged || positionChanged) {
		valueText_.UpdateLayout(false);
	}

	labelText_.Draw();
	valueText_.Draw();
}
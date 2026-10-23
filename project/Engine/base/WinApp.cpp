#define NOMINMAX
#include "WinApp.h"
#include <algorithm>
#include <d3d12.h>
#include <dbt.h>
#include <strsafe.h>

#ifdef USE_IMGUI
#include "externals/imgui/imgui_impl_win32.h"
#endif // USE_IMGUI

// timeBeginPeriodで使用するライブラリ。
#pragma comment(lib, "winmm.lib")

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);
#endif // USE_IMGUI

namespace {

// サイズ調整時に維持するクライアント領域の縦横比。
constexpr float kTargetAspectRatio = 16.0f / 9.0f;

// タイトルバー、システムメニュー、最小化ボタンを持つウィンドウスタイル。
constexpr DWORD kFixedWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

// ウィンドウモードの基準サイズ。
constexpr int32_t kWindowedClientWidth = 1280;
constexpr int32_t kWindowedClientHeight = 720;

// フルスクリーン状態と、切り替え前のウィンドウ情報。
bool gIsFullscreen = false;
RECT gWindowedRect{};
DWORD gWindowedStyle = 0;

// ウィンドウモードと、モニター全体を覆うフルスクリーン表示を切り替える。
void ToggleFullscreen(HWND hwnd) {
	if (!gIsFullscreen) {
		// 復帰時に使用する位置とウィンドウスタイルを保存する。
		gWindowedStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
		GetWindowRect(hwnd, &gWindowedRect);

		// ウィンドウに最も近いモニターの表示範囲を取得する。
		MONITORINFO monitorInfo{};
		monitorInfo.cbSize = sizeof(monitorInfo);
		GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST), &monitorInfo);

		// タイトルバーなどを外し、モニター全体へウィンドウを広げる。
		SetWindowLongPtr(hwnd, GWL_STYLE, static_cast<LONG_PTR>(gWindowedStyle & ~WS_OVERLAPPEDWINDOW));

		SetWindowPos(
		    hwnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
		    SWP_FRAMECHANGED);

		gIsFullscreen = true;
	} else {
		// 元のスタイルへ戻す。
		SetWindowLongPtr(hwnd, GWL_STYLE, static_cast<LONG_PTR>(gWindowedStyle));

		// クライアント領域が基準サイズになるよう、枠を含むサイズを計算する。
		RECT windowRect{0, 0, kWindowedClientWidth, kWindowedClientHeight};

		const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_EXSTYLE));

		AdjustWindowRectEx(&windowRect, gWindowedStyle, false, exStyle);

		// 保存した位置へ、1280×720のクライアント領域で復帰する。
		SetWindowPos(hwnd, nullptr, gWindowedRect.left, gWindowedRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED | SWP_NOOWNERZORDER | SWP_NOZORDER);

		gIsFullscreen = false;
	}
}

// 指定したクライアントサイズに必要な、枠を含むウィンドウサイズを求める。
SIZE CalculateMinWindowSizeForClient(HWND hwnd, int32_t minClientWidth, int32_t minClientHeight) {

	RECT windowRect{0, 0, minClientWidth, minClientHeight};

	const DWORD style = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_STYLE));
	const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd, GWL_EXSTYLE));

	AdjustWindowRectEx(&windowRect, style, false, exStyle);

	return {windowRect.right - windowRect.left, windowRect.bottom - windowRect.top};
}

// サイズ変更中の矩形を補正し、クライアント領域の縦横比を16:9に保つ。
void KeepClientAspectRatio16By9(HWND hwnd, WPARAM edge, RECT* rect) {
	if (!rect) {
		return;
	}

	RECT currentWindowRect{};
	RECT currentClientRect{};

	if (!GetWindowRect(hwnd, &currentWindowRect) || !GetClientRect(hwnd, &currentClientRect)) {
		return;
	}

	// ウィンドウ全体とクライアント領域の差から、枠やタイトルバーの大きさを求める。
	const int32_t nonClientWidth = (currentWindowRect.right - currentWindowRect.left) - (currentClientRect.right - currentClientRect.left);

	const int32_t nonClientHeight = (currentWindowRect.bottom - currentWindowRect.top) - (currentClientRect.bottom - currentClientRect.top);

	const int32_t windowWidth = rect->right - rect->left;
	const int32_t windowHeight = rect->bottom - rect->top;

	int32_t clientWidth = std::max(1, windowWidth - nonClientWidth);
	int32_t clientHeight = std::max(1, windowHeight - nonClientHeight);

	// 幅から高さを補正する場合と、高さから幅を補正する場合を比較する。
	const int32_t heightFromWidth = std::max(1, static_cast<int32_t>(static_cast<float>(clientWidth) / kTargetAspectRatio));

	const int32_t widthFromHeight = std::max(1, static_cast<int32_t>(static_cast<float>(clientHeight) * kTargetAspectRatio));

	const int32_t adjustHeightAmount = std::abs(heightFromWidth - clientHeight);
	const int32_t adjustWidthAmount = std::abs(widthFromHeight - clientWidth);

	// 修正量が小さくなる方を採用する。
	if (adjustWidthAmount < adjustHeightAmount) {
		clientWidth = widthFromHeight;
	} else {
		clientHeight = heightFromWidth;
	}

	const int32_t adjustedWindowWidth = clientWidth + nonClientWidth;
	const int32_t adjustedWindowHeight = clientHeight + nonClientHeight;

	// 操作中の辺に合わせ、左右のどちらを動かすかを決める。
	switch (edge) {
	case WMSZ_LEFT:
	case WMSZ_TOPLEFT:
	case WMSZ_BOTTOMLEFT:
		rect->left = rect->right - adjustedWindowWidth;
		break;
	default:
		rect->right = rect->left + adjustedWindowWidth;
		break;
	}

	// 操作中の辺に合わせ、上下のどちらを動かすかを決める。
	switch (edge) {
	case WMSZ_TOP:
	case WMSZ_TOPLEFT:
	case WMSZ_TOPRIGHT:
		rect->top = rect->bottom - adjustedWindowHeight;
		break;
	default:
		rect->bottom = rect->top + adjustedWindowHeight;
		break;
	}
}

} // namespace

// privateなstaticメンバーの実体。
// 外部からの読み取りにはGetClientWidth／GetClientHeightを使用する。
int32_t WinApp::kClientWidth = 1280;
int32_t WinApp::kClientHeight = 720;

// Windowsから届いたメッセージを処理する。
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	// F11キーでフルスクリーン表示を切り替える。
	if (msg == WM_KEYDOWN && wparam == VK_F11) {
		ToggleFullscreen(hwnd);
		return 0;
	}

#ifdef USE_IMGUI
	// ImGuiが処理したメッセージは、ここで処理を終える。
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
#endif // USE_IMGUI

	switch (msg) {
	case WM_DESTROY:
		// メッセージキューへ終了要求を追加する。
		PostQuitMessage(0);
		return 0;

	case WM_SIZING:
		// サイズ変更中の矩形を16:9に補正する。
		KeepClientAspectRatio16By9(hwnd, wparam, reinterpret_cast<RECT*>(lparam));
		return TRUE;

	case WM_GETMINMAXINFO: {
		// 最小サイズを、1280×720のクライアント領域を確保できる大きさにする。
		MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(lparam);

		if (minMaxInfo) {
			const SIZE minWindowSize = CalculateMinWindowSizeForClient(hwnd, kWindowedClientWidth, kWindowedClientHeight);

			minMaxInfo->ptMinTrackSize.x = minWindowSize.cx;
			minMaxInfo->ptMinTrackSize.y = minWindowSize.cy;
		}
		return 0;
	}

	case WM_DEVICECHANGE:
		if (wparam == DBT_DEVICEARRIVAL || wparam == DBT_DEVICEREMOVECOMPLETE || wparam == DBT_DEVNODES_CHANGED) {

			// ウィンドウに保存したWinAppのポインターを取得する。
			WinApp* winApp = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

			if (winApp) {
				// パッド再列挙フラグを立てる。
				winApp->IsPad_ = true;
			}
		}
		break;
	}

	// この関数で処理しなかったメッセージはWindowsの標準処理へ渡す。
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// ウィンドウを作成し、表示する。
void WinApp::Initialize(const wchar_t* TitleName, int32_t clientWidth, int32_t clientHeight) {

	// 既存の仕様として、引数のサイズではなく1280×720で初期化する。
	(void)clientWidth;
	(void)clientHeight;
	kClientWidth = kWindowedClientWidth;
	kClientHeight = kWindowedClientHeight;

	// COMをマルチスレッドアパートメントで初期化する。
	HRESULT hr = CoInitializeEx(0, COINITBASE_MULTITHREADED);

#ifdef USE_IMGUI
	// フルスクリーン時を含む高DPI環境で、
	// ImGuiのマウス座標と描画座標がずれないようにする。
	ImGui_ImplWin32_EnableDpiAwareness();
#endif // USE_IMGUI

	// ウィンドウクラスの情報を設定する。
	wc_.lpfnWndProc = WindowProc;
	wc_.lpszClassName = L"CG2WindowClass";
	wc_.hInstance = GetModuleHandle(nullptr);
	wc_.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc_);

	// 指定したクライアント領域を確保できるよう、ウィンドウの外寸を計算する。
	wrc_ = {0, 0, kClientWidth, kClientHeight};
	AdjustWindowRect(&wrc_, kFixedWindowStyle, false);

	hwnd_ = CreateWindow(wc_.lpszClassName, TitleName, kFixedWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, wrc_.right - wrc_.left, wrc_.bottom - wrc_.top, nullptr, nullptr, wc_.hInstance, nullptr);

	ShowWindow(hwnd_, SW_SHOW);

	// staticなWindowProcからこのインスタンスを参照できるようにする。
	SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	// タイマーの精度を1msに設定する。
	timeBeginPeriod(1);
}

// アイコンファイルから大小のアイコンを読み込み、ウィンドウへ設定する。
bool WinApp::SetWindowIconFromFile(const std::wstring& iconPath) {
	if (!hwnd_ || iconPath.empty()) {
		return false;
	}

	HICON loadedLargeIcon = reinterpret_cast<HICON>(LoadImageW(nullptr, iconPath.c_str(), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE));

	HICON loadedSmallIcon = reinterpret_cast<HICON>(LoadImageW(nullptr, iconPath.c_str(), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE));

	// 片方でも失敗した場合は、読み込み済みの分も解放する。
	if (!loadedLargeIcon || !loadedSmallIcon) {
		if (loadedLargeIcon) {
			DestroyIcon(loadedLargeIcon);
		}
		if (loadedSmallIcon) {
			DestroyIcon(loadedSmallIcon);
		}
		return false;
	}

	// 古いアイコンを解放してから、新しいアイコンへ置き換える。
	if (largeIcon_) {
		DestroyIcon(largeIcon_);
		largeIcon_ = nullptr;
	}
	if (smallIcon_) {
		DestroyIcon(smallIcon_);
		smallIcon_ = nullptr;
	}

	largeIcon_ = loadedLargeIcon;
	smallIcon_ = loadedSmallIcon;

	SendMessageW(hwnd_, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(largeIcon_));

	SendMessageW(hwnd_, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon_));

	return true;
}

// クライアントサイズを更新し、それに合わせてウィンドウの外寸を変更する。
void WinApp::SetClientSize(int32_t clientWidth, int32_t clientHeight) {
	if (!hwnd_) {
		return;
	}

	// 最小サイズは1280×720とする。
	const int32_t clampedClientWidth = std::max(clientWidth, kWindowedClientWidth);
	const int32_t clampedClientHeight = std::max(clientHeight, kWindowedClientHeight);

	kClientWidth = clampedClientWidth;
	kClientHeight = clampedClientHeight;

	RECT windowRect{0, 0, clampedClientWidth, clampedClientHeight};

	const DWORD style = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_STYLE));
	const DWORD exStyle = static_cast<DWORD>(GetWindowLongPtr(hwnd_, GWL_EXSTYLE));

	// タイトルバーや枠の大きさを加味する。
	AdjustWindowRectEx(&windowRect, style, false, exStyle);

	// ウィンドウ位置と前後関係を維持したままサイズを変更する。
	SetWindowPos(hwnd_, nullptr, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
}

// 現在、毎フレームの追加処理はない。
void WinApp::Update() {}

// 保持しているアイコンを解放し、既存のウィンドウ終了処理を行う。
void WinApp::Finalize() {
	if (largeIcon_) {
		DestroyIcon(largeIcon_);
		largeIcon_ = nullptr;
	}
	if (smallIcon_) {
		DestroyIcon(smallIcon_);
		smallIcon_ = nullptr;
	}

	CloseWindow(hwnd_);
}

// メッセージを1件取り出して処理する。
// WM_QUITを受け取った場合はfalseを返し、メインループの終了を通知する。
bool WinApp::ProcessMessage() {

	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return false;
	}

	return true;
}
#pragma once
#include <Windows.h>
#include <cstdint>
#include <d3d12.h>
#include <string>
#include <wrl.h>

// ウィンドウの生成、メッセージ処理、サイズ変更、終了処理を管理する。
class WinApp {

private:
	// ウィンドウクラスの登録情報。
	WNDCLASS wc_{};

	// ウィンドウ作成時に使用する矩形。
	RECT wrc_;

	// 作成したウィンドウのハンドル。
	HWND hwnd_;

	// DirectXのデバッグ制御用オブジェクト。
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;

	// ウィンドウタイトル用の文字列。
	const wchar_t* TitleName_ = L"CG2";

	// ファイルから読み込んだ大小のアイコン。
	HICON largeIcon_ = nullptr;
	HICON smallIcon_ = nullptr;

	// デバイス構成の変更時に立てる、パッドの再列挙要求フラグ。
	bool IsPad_ = false;

	// クライアント領域の幅と高さ。

	static int32_t kClientWidth;
	static int32_t kClientHeight;

public:
	// ウィンドウクラスを登録し、ウィンドウを作成して表示する。
	void Initialize(const wchar_t* TitleName, int32_t clientWidth, int32_t clientHeight);

	// 指定したアイコンファイルを読み込み、ウィンドウへ設定する。
	// 読み込みに失敗した場合はfalseを返す。
	bool SetWindowIconFromFile(const std::wstring& iconPath);

	// クライアント領域のサイズを変更する。
	// サイズの更新は、公開変数への代入ではなく、この関数を通して行う。
	void SetClientSize(int32_t clientWidth, int32_t clientHeight);

	// 毎フレームの更新処理。
	void Update();

	// アイコンの解放とウィンドウの終了処理。
	void Finalize();

	// メッセージを処理し、終了要求があればfalseを返す。
	bool ProcessMessage();

	// ウィンドウハンドルを取得する。
	// constにより、このゲッター内でメンバーを書き換えないことを示す。
	HWND GetHwnd() const { return hwnd_; };

	// アプリケーションのインスタンスハンドルを取得する。
	HINSTANCE GetHinstance() const { return wc_.hInstance; };

	// 画面サイズを値として取得する。
	static int32_t GetClientWidth() { return kClientWidth; }
	static int32_t GetClientHeight() { return kClientHeight; }

	// パッドの再列挙が必要かどうかを取得する。
	bool GetIsPad() const { return IsPad_; };

	// パッドの再列挙要求フラグを更新する。
	void SetIsPad(bool isPad) { IsPad_ = isPad; };

private:
	// Windowsから送られるメッセージを受け取るコールバック関数。
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
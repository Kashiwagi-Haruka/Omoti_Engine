#define NOMINMAX
#include "GameBase.h"
#include "Engine/Editor/EditorManager/EditorManager.h"
#include "Engine/ScreenShot/ScreenShot.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "ParticleManager.h"
#include "SpriteCommon.h"
#include "SrvManager/SrvManager.h"
#include "Text/FreeTypeManager/FreetypeManager.h"
#include "TextureManager.h"
#include <DbgHelp.h>
#include <dxgidebug.h>
#include <shellapi.h>
#include <strsafe.h>

// DirectX、ダンプ出力、URLを開く処理などで使用するライブラリ。
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "Shell32.lib")

// 共有インスタンスの実体。
std::unique_ptr<GameBase> GameBase::instance = nullptr;

// 初回アクセス時に生成し、それ以降は同じインスタンスを返す。
GameBase* GameBase::GetInstance() {
	if (instance == nullptr) {
		instance = std::make_unique<GameBase>();
	}
	return instance.get();
}

// 各システムを終了する。
// 描画リソースを使用するシステムを終了してから、DirectXとウィンドウを解放する。
void GameBase::Finalize() {
	// 描画環境やフォントを解放する前に集計スレッドを停止。
	fpsChecker_.Finalize();
	// フォント関連のリソースを解放する。
	FreeTypeManager::Finalize();

	// ImGuiを終了し、管理オブジェクトを解放する。
	if (imguiM_) {
		imguiM_->Finalize();
		imguiM_.reset();
	}

	// 音声と、各種描画リソースを管理するシステムを終了する。
	Audio::GetInstance()->Finalize();

	TextureManager::GetInstance()->Finalize();

	ParticleManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	EditorManager::GetInstance()->Finalize();
	SpriteCommon::GetInstance()->Finalize();
	Object3dCommon::GetInstance()->Finalize();

	// DirectX側の終了処理を行ってから、管理オブジェクトを解放する。
	if (dxCommon_) {
		dxCommon_->Finalize();
		dxCommon_.reset();
	}

	// 最後にウィンドウ側の終了処理を行う。
	if (winApp_) {
		winApp_->Finalize();
		winApp_.reset();
	}

	// 共有インスタンス自身を解放するため、この後はメンバーへアクセスしない。
	instance.reset();
}

// 各システムを、必要な依存先が利用できる順番で初期化する。
void GameBase::Initialize(const wchar_t* TitleName, int32_t WindowWidth, int32_t WindowHeight) {

	// DirectXの初期化に必要なウィンドウを先に作成する。
	winApp_ = std::make_unique<WinApp>();
	winApp_->Initialize(TitleName, WindowWidth, WindowHeight);

	// ウィンドウに対応する描画環境を作成する。
	dxCommon_ = std::make_unique<DirectXCommon>();
	dxCommon_->initialize(winApp_.get());

	// テクスチャ管理へDirectXの管理オブジェクトを渡す。
	TextureManager::GetInstance()->Initialize(dxCommon_.get());

	// 入力とImGuiに、ウィンドウや描画環境を渡す。
	Input::GetInstance()->Initialize(winApp_.get());

	imguiM_ = std::make_unique<ImGuiManager>();
	imguiM_->Initialize(winApp_.get(), dxCommon_.get());

	// 音声、パーティクル、モデルの管理システムを初期化する。
	Audio::GetInstance()->InitializeIXAudio();
	ParticleManager::GetInstance()->Initialize(dxCommon_.get());
	ModelManager::GetInstance()->Initialize(dxCommon_.get());

	// 3Dオブジェクトとスプライトの共通描画処理を初期化する。
	Object3dCommon::GetInstance()->Initialize(dxCommon_.get());
	SpriteCommon::GetInstance()->Initialize(dxCommon_.get());

	// フォント描画に使用するシステムを初期化する。
	FreeTypeManager::Initialize();

	fpsChecker_.Initialize();
}

// ウィンドウが存在する場合に、アイコンの設定を委譲する。
bool GameBase::SetWindowIconFromFile(const std::wstring& iconPath) {
	if (!winApp_) {
		return false;
	}
	return winApp_->SetWindowIconFromFile(iconPath);
}

// Windowsメッセージの処理結果を返す。
bool GameBase::ProcessMessage() { return winApp_->ProcessMessage(); }

// 例外発生時の情報をミニダンプへ保存する。
LONG WINAPI GameBase::ExportDump(EXCEPTION_POINTERS* exception) {
	// 現在のローカル時刻を、出力ファイル名に使用する。
	SYSTEMTIME time;
	GetLocalTime(&time);

	// Dumpsディレクトリを作成し、出力先のパスを組み立てる。
	wchar_t filePath[MAX_PATH] = {0};
	CreateDirectory(L"./Dumps", nullptr);

	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);

	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	// 実行中のプロセスと、例外が発生したスレッドのIDを取得する。
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();

	// ダンプに含める例外情報を設定する。
	MINIDUMP_EXCEPTION_INFORMATION mindumpInformation{0};
	mindumpInformation.ThreadId = threadId;
	mindumpInformation.ExceptionPointers = exception;
	mindumpInformation.ClientPointers = TRUE;

	// 最低限の情報を含むミニダンプを出力する。
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &mindumpInformation, nullptr, nullptr);

	// 例外ハンドラーとして処理を終えることを通知する。
	return EXCEPTION_EXECUTE_HANDLER;
}

// ゲーム本体の更新・描画に先立って、フレームの準備を行う。
void GameBase::BeginFlame() {

	// エディター側のワールド描画状態を、このフレーム用に戻す。
	EditorManager::GetInstance()->ResetEditorWorldDrawState();

	// 描画先の準備と、ImGuiのフレーム開始処理。
	dxCommon_->PreDraw();
	imguiM_->Begin();

	// 入力状態と、テクスチャ・音声管理の更新。
	Input::GetInstance()->Update();
	TextureManager::GetInstance()->Update();
	Audio::GetInstance()->Update();
}

// ゲーム本体の描画後に、画面への出力を行う。
void GameBase::EndFlame() {
	imguiM_->End();

	// シーン画像をバックバッファへ描画。
	dxCommon_->DrawSceneTextureToBackBuffer();

	// エディターなどのUIを描画。
	imguiM_->Draw(dxCommon_.get());

	// ImGuiより後に、FPSを最前面へ描画する。
	if (fpsChecker_.IsVisible()) {
		const float width = GetScreenWidth();
		const float height = GetScreenHeight();

		if (width > 0.0f && height > 0.0f) {
			auto* commandList = dxCommon_->GetCommandList();

			// ImGui描画後の描画範囲を画面全体へ戻す。
			D3D12_VIEWPORT viewport{};
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
			viewport.Width = width;
			viewport.Height = height;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;

			D3D12_RECT scissor{};
			scissor.left = 0;
			scissor.top = 0;
			scissor.right = static_cast<LONG>(width);
			scissor.bottom = static_cast<LONG>(height);

			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissor);

			// Text描画で使うSRVヒープを設定。
			TextureManager::GetInstance()->GetSrvManager()->PreDraw();

			fpsChecker_.Draw();
		}
	}

	// FPSを含む描画コマンドを実行して表示。
	dxCommon_->PostDraw();

	// 非表示中もフレーム数は計測する。
	fpsChecker_.CountFrame();

	FreeTypeManager::ResetFontUsage();
}

// 指定したURLを、OSに関連付けられたアプリケーションで開く。
void GameBase::OpenHttps(const std::wstring& url) { ShellExecuteW(nullptr, L"open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL); }

// DirectXCommonで計測したフレーム経過時間を取得する。
float GameBase::GetDeltaTime() { return dxCommon_->GetDeltaTime(); }

// 描画環境が作成済みの場合に、DirectXデバイスを返す。
ID3D12Device* GameBase::GetD3D12Device() {
	if (!dxCommon_) {
		return nullptr;
	}
	return dxCommon_->GetDevice();
}

// 現在のシーンをバックバッファへ出力してから、画像として保存する。
bool GameBase::SaveCurrentFrameScreenShot(const std::string& filePath) {
	if (!dxCommon_) {
		return false;
	}

	dxCommon_->EnsureSceneTextureCopiedToBackBuffer();
	return ScreenShot::SaveBackBuffer(dxCommon_.get(), filePath);
}

// WinAppで管理しているクライアント領域の幅を取得する。
// ScreenSize.hの固定値と異なり、呼び出した時点の値を返す。
float GameBase::GetScreenWidth() const { return static_cast<float>(WinApp::GetClientWidth()); }

// WinAppで管理しているクライアント領域の高さを取得する。
float GameBase::GetScreenHeight() const { return static_cast<float>(WinApp::GetClientHeight()); }

// 現在の幅から半幅を計算する。
float GameBase::GetScreenHalfWidth() const { return GetScreenWidth() * 0.5f; }

// 現在の高さから半高さを計算する。
float GameBase::GetScreenHalfHeight() const { return GetScreenHeight() * 0.5f; }

void GameBase::SetFPSVisible(bool visible) { fpsChecker_.SetVisible(visible); }

bool GameBase::IsFPSVisible() const { return fpsChecker_.IsVisible(); }

void GameBase::ToggleFPSVisible() { fpsChecker_.ToggleVisible(); }
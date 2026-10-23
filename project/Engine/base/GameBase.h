#pragma once
#include "Audio.h"
#include "BlendMode/BlendModeManager.h"
#include "Camera.h"
#include "DirectXCommon.h"
#include "Function.h"

#include "Data/VertexData.h"
#include "TextureManager.h"
#include "Vector4.h"
#include "WinApp.h"
#include <memory>
#include <string>

class SrvManager;
class ImGuiManager;

// ウィンドウ、描画、入力、音声などの初期化と終了をまとめて管理する。
// フレームの開始・終了処理と、画面サイズの取得窓口も提供する。
class GameBase {

private:
	// ゲーム全体で共有するインスタンス。
	static std::unique_ptr<GameBase> instance;

	// ウィンドウの生成とWindowsメッセージの処理を担当する。
	std::unique_ptr<WinApp> winApp_ = nullptr;

	// DirectXの初期化、描画先の管理、画面表示、GPU同期を担当する。
	std::unique_ptr<DirectXCommon> dxCommon_ = nullptr;

	// ImGuiのフレーム開始・終了と描画を担当する。
	std::unique_ptr<ImGuiManager> imguiM_ = nullptr;

	// Windows APIなどの処理結果を保持するための変数。
	HRESULT hr_;

public:
	// 共有インスタンスを取得する。
	// 初回の呼び出し時にインスタンスを生成する。
	static GameBase* GetInstance();

	// 各システムを終了し、保持しているリソースを解放する。
	void Finalize();

	// ウィンドウと、ゲームに必要な各システムを初期化する。
	void Initialize(const wchar_t* TitleName, int32_t WindowWidth, int32_t WindowHeight);

	// ウィンドウのアイコンをファイルから設定する。
	// ウィンドウが未生成の場合や読み込み失敗時はfalseを返す。
	bool SetWindowIconFromFile(const std::wstring& iconPath);

	// フレームの開始処理。
	// 描画準備、ImGui開始、入力・テクスチャ・音声の更新を行う。
	void BeginFlame();

	// フレームの終了処理。
	// シーン出力、ImGui描画、画面表示、GPU完了待ちを行う。
	void EndFlame();

	// Windowsメッセージを処理する。
	// 終了要求を受け取った場合はfalseを返す。
	bool ProcessMessage();

	// 指定したURLを、OSに関連付けられたアプリケーションで開く。
	void OpenHttps(const std::wstring& url);

	// 前フレームの経過時間を秒単位で取得する。
	float GetDeltaTime();

	// DirectXのデバイスを取得する。
	// 描画システムが未生成の場合はnullptrを返す。
	ID3D12Device* GetD3D12Device();

	// 現在のフレームを画像ファイルとして保存する。
	bool SaveCurrentFrameScreenShot(const std::string& filePath);

	// ScreenSize.hにあった値を、取得時に計算するゲッターへまとめる。
	// floatで返すため、UI配置などの座標計算にそのまま使用できる。
	// constにより、取得時にGameBaseのメンバーを書き換えない。
	float GetScreenWidth() const;
	float GetScreenHeight() const;
	float GetScreenHalfWidth() const;
	float GetScreenHalfHeight() const;

	// 例外発生時に、調査用のミニダンプファイルを出力する。
	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
};
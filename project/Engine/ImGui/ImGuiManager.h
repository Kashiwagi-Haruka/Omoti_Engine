#pragma once

#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>

// ポインターとして参照するクラスを前方宣言する。
class WinApp;
class DirectXCommon;
class SrvManager;

// ImGuiの初期化、フレーム開始・終了、描画、終了処理を管理する。
// エディター画面の構築と、マウス座標の描画サイズへの変換も行う。
class ImGuiManager {

	// SRVのインデックスを保持するための変数。
	// 現在の実装では使用していない。
	uint32_t srvIndex_;

	// 描画環境とウィンドウへの参照。
	// 所有権は持たないため、使用中は参照先が有効である必要がある。
	DirectXCommon* dxCommon_ = nullptr;
	WinApp* winApp_ = nullptr;

	// 前回のエディターレイアウト設定。
	// 現在の実装では値の保存のみ行い、判定には使用していない。
	bool prevEditorLayoutEnabled_ = false;

public:
	// ImGuiのコンテキスト、フォント、Win32・DX12バックエンドを初期化する。
	void Initialize(WinApp* winapp, DirectXCommon* dxCommon);

	// 新しいフレームを開始し、エディターのUIを構築する。
	void Begin();

	// UIの構築を終え、描画データを生成する。
	void End();

	// 生成済みの描画データを、DirectXのコマンドリストへ記録する。
	void Draw(DirectXCommon* dxCommon);

	// バックエンドとImGuiのコンテキストを破棄する。
	void Finalize();
};
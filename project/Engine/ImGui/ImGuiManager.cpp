#define NOMINMAX
#include "ImGuiManager.h"
#include "Engine/Editor/EditorManager/EditorManager.h"
#include "Engine/Editor/EditorTool/Hierarchy/Hierarchy.h"
#include "GameBase.h"
#include <dxgi1_6.h>

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#endif

#include "DirectXCommon.h"
#include "Input.h"
#include "SrvManager/SrvManager.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <algorithm>
#include <cfloat>
#include <format>
#include <string>

// ImGuiを使用するための共通設定と、各バックエンドの初期化を行う。
// maybe_unusedは、USE_IMGUI無効時に引数が未使用になることを許容する。
void ImGuiManager::Initialize([[maybe_unused]] WinApp* winApp, [[maybe_unused]] DirectXCommon* dxCommon) {
#ifdef USE_IMGUI
	auto* srvManager = TextureManager::GetInstance()->GetSrvManager();

	// フレーム開始時の座標補正と描画設定に使う参照を保存する。
	dxCommon_ = dxCommon;
	winApp_ = winApp;

	// ヘッダーとライブラリの互換性を確認し、ImGuiの状態を作成する。
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ダークテーマを適用し、入力欄などの背景色を調整する。
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.02f, 0.02f, 0.02f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.0f);

	ImGuiIO& io = ImGui::GetIO();

	// エディター用フォントを全体に適用（日本語グリフを含む）
	ImFont* editorFont = io.Fonts->AddFontFromFileTTF("Resources/Editor/rounded-l-mplus-1c-regular.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	if (editorFont) {
		io.FontDefault = editorFont;
	}

	// Win32 backend（先に）
	// ウィンドウ情報や入力情報を扱うバックエンドを初期化する。
	ImGui_ImplWin32_Init(winApp->GetHwnd());

	// ---- DX12 backend（新API）----
	// ImGuiが描画に使用するデバイス、キュー、出力形式などを渡す。
	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = dxCommon->GetDevice();
	init_info.CommandQueue = dxCommon->GetCommandQueue().Get();
	init_info.NumFramesInFlight = static_cast<int>(dxCommon->GetSwapChainResourcesNum());
	init_info.RTVFormat = dxCommon->GetRtvDesc().Format; // 固定値より安全
	init_info.SrvDescriptorHeap = srvManager->GetDescriptorHeap().Get();

	// ImGuiがSRVを必要としたときに呼ぶ割り当て処理。
	// 共通のSrvManagerから領域を確保し、CPU・GPU両方のハンドルを返す。
	init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle) {
		auto* sm = reinterpret_cast<SrvManager*>(info->UserData);
		uint32_t index = sm->Allocate();
		*out_cpu_handle = sm->GetCPUDescriptorHandle(index);
		*out_gpu_handle = sm->GetGPUDescriptorHandle(index);
	};

	// ImGuiからSRVの解放を要求されたときに呼ばれる。
	init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE) {
		// 今回はフリーリスト無しなので何もしない
	};

	// 割り当てコールバックからSrvManagerを参照できるようにする。
	init_info.UserData = srvManager;


	ImGui_ImplDX12_Init(&init_info);

	// 任意のフラグ（順序はどちらでもOK）
	// キーボードとゲームパッドによるUI操作を有効にする。
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#endif
}

// バックエンドの状態を更新し、新しいImGuiフレームを開始する。
void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	if (winApp_) {
		RECT clientRect{};

		if (GetClientRect(winApp_->GetHwnd(), &clientRect)) {
			// 実際のクライアント領域の大きさを取得する。
			// 座標変換で0除算が起きないよう、最小値を1にする。
			const float clientWidth = static_cast<float>(std::max(1L, clientRect.right - clientRect.left));
			const float clientHeight = static_cast<float>(std::max(1L, clientRect.bottom - clientRect.top));

			// ImGuiの描画座標として使用するサイズ。
			const float renderWidth = static_cast<float>(GameBase::GetInstance()->GetScreenWidth());
			const float renderHeight = static_cast<float>(GameBase::GetInstance()->GetScreenHeight());

			// スクリーン座標のカーソル位置を、クライアント座標へ変換する。
			POINT cursorPoint{};

			if (GetCursorPos(&cursorPoint) && ScreenToClient(winApp_->GetHwnd(), &cursorPoint) != 0 && cursorPoint.x >= 0 && cursorPoint.y >= 0 &&
			    cursorPoint.x < (clientRect.right - clientRect.left) && cursorPoint.y < (clientRect.bottom - clientRect.top)) {

				// 実際のウィンドウサイズと描画サイズの差を補正する。
				const float mappedMouseX = static_cast<float>(cursorPoint.x) * (renderWidth / clientWidth);
				const float mappedMouseY = static_cast<float>(cursorPoint.y) * (renderHeight / clientHeight);
				io.AddMousePosEvent(mappedMouseX, mappedMouseY);
			} else {
				// ウィンドウ外や座標取得失敗時は、マウス位置を無効として通知する。
				io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
			}

			// UIの座標系を描画サイズに揃える。
			io.DisplaySize = ImVec2(renderWidth, renderHeight);
			io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		}
	}

	ImGui::NewFrame();

	// フレームレートを表示するウィンドウ。
	// Beginの戻り値にかかわらず、対応するEndを呼ぶ。
	if (ImGui::Begin("Performance")) {
		ImGui::Text("FPS: %.1f", io.Framerate);
	}
	ImGui::End();
#endif

	// エディター画面の構築処理は、USE_IMGUIの条件外から呼び出している。
	EditorManager* editorManager = EditorManager::GetInstance();

#ifdef USE_IMGUI
	// 現在の実装では、エディターレイアウトを常に有効にする。
	constexpr bool isEditorLayoutEnabled = true;

	if (dxCommon_) {
		dxCommon_->SetEditorLayoutEnabled(isEditorLayoutEnabled);
	}

	const Hierarchy* hierarchy = Hierarchy::GetInstance();
	const bool isPlayMode = hierarchy && hierarchy->IsPlayMode();

	// 編集中はカーソルの固定を解除し、UIを操作できるよう表示する。
	if (isEditorLayoutEnabled && !isPlayMode) {
		Input* input = Input::GetInstance();

		if (input) {
			input->SetIsCursorStability(false);
			input->SetIsCursorVisible(true);
		}
	}

	prevEditorLayoutEnabled_ = isEditorLayoutEnabled;
#endif

	// オブジェクト編集画面とアセット一覧を構築する。
	editorManager->DrawObjectEditors();
	editorManager->DrawAssetWindow();
}

// UIの構築を終了し、描画用データを生成する。
// この時点では、DirectXの描画コマンドはまだ記録しない。
void ImGuiManager::End() {
#ifdef USE_IMGUI
	ImGui::Render();
#endif
}

// Endで生成した描画データを、DirectXのコマンドリストへ記録する。
void ImGuiManager::Draw([[maybe_unused]] DirectXCommon* dxCommon) {
#ifdef USE_IMGUI
	auto* srvManager = TextureManager::GetInstance()->GetSrvManager();

	// 描画に必要なヒープとコマンドリストが存在することを確認する。
	if (!srvManager->GetDescriptorHeap().Get() || !dxCommon->GetCommandList()) {
		OutputDebugStringA("ImGui Render Error: srvDescriptorHeap_ or commandList_ is null\n");
		return;
	}

	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager->GetDescriptorHeap().Get()};
	dxCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

	// ImGuiの頂点・インデックスなどを使用した描画コマンドを記録する。
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());

#endif
}

// バックエンドを終了してから、ImGuiのコンテキストを破棄する。
void ImGuiManager::Finalize() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

#endif
}
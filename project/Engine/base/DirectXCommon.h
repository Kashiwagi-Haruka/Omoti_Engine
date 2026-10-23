#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

// DXC (DirectX Shader Compiler)
#include <dxcapi.h>

// SAL マクロ用
#include <sal.h>

#include <array>
#include <chrono>
#include <string>
#include <vector>

#include "WinApp.h"

#include "BlendMode/BlendModeManager.h"
#include "Data/VertexData.h"
#include "Function.h"
#include "Light/CommonLight/DirectionalCommonLight.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

class BlendModeManager;
class SrvManager;

// DirectX 12 の初期化、描画先の管理、画面への出力、GPU同期を担当する。
// シーンを一度テクスチャへ描画し、ポストエフェクトを適用してバックバッファへ出力する。
class DirectXCommon {

	// DirectX API の実行結果と、GPU・アダプターを扱うためのオブジェクト。
	HRESULT hr_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_;

	// コマンドアロケーターは、GPUへ渡すコマンドの記録用メモリを管理する。
	static const UINT kFrameCount = 2; // スワップチェインのバッファ数に合わせる
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators_[kFrameCount];
	UINT frameIndex_ = 0;

	// コマンドリストに描画処理を記録し、コマンドキュー経由でGPUへ送信する。
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_;
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_ = nullptr;
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex_;

	// ディスクリプタの間隔。隣のビューのハンドルを計算するときに使用する。
	uint32_t descriptorSizeSRV_ = 0;
	uint32_t descriptorSizeRTV_ = 0;
	uint32_t descriptorSizeDSV_ = 0;

	// RTVは描画先、SRVはシェーダーからの参照先、DSVは深度・ステンシルの参照先を表す。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;

	// 表示用のバックバッファと、ポストエフェクト用の描画テクスチャ。
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources_;
	Microsoft::WRL::ComPtr<ID3D12Resource> sceneColorResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> sceneOutlineResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> sceneEmissionResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> sceneSrvDescriptorHeap_ = nullptr;

	// 各テクスチャのRTVと、シーン用SRVテーブルの先頭ハンドル。
	D3D12_CPU_DESCRIPTOR_HANDLE sceneRtvHandle_{};
	D3D12_CPU_DESCRIPTOR_HANDLE sceneOutlineRtvHandle_{};
	D3D12_CPU_DESCRIPTOR_HANDLE sceneEmissionRtvHandle_{};
	D3D12_CPU_DESCRIPTOR_HANDLE sceneSrvHandleCPU_{};
	D3D12_GPU_DESCRIPTOR_HANDLE sceneSrvHandleGPU_{};

	// ディゾルブ用のノイズ画像と、その画像をGPUへ転送するためのバッファ。
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveMaskResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveMaskUploadResource_ = nullptr;

	// 全画面描画で使用するルートシグネチャ、PSO、定数バッファ。
	Microsoft::WRL::ComPtr<ID3D12RootSignature> copyRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> copyPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> postEffectParameterResource_ = nullptr;

	// シェーダーに渡すポストエフェクト設定。
	// メンバーの並びとサイズは、HLSL側の定数バッファのレイアウトに対応させる。
	// 有効・無効の値も、GPUへはfloatの0.0fまたは1.0fとして渡す。
	struct PostEffectParameters {
		float vignetteStrength;
		float vignetteColor[3];
		float randomNoiseEnabled;
		float randomNoiseScale;
		float randomNoiseTime;
		float randomNoiseBlendMode;
		float boxFilterKernelSize;
		float fullscreenGrayscaleEnabled;
		float fullscreenSepiaEnabled;
		float fullscreenFilterType;
		float gaussianFilterSigma;
		float dissolveEnabled;
		float dissolveThreshold;
		float dissolveEdgeWidth;
		float dissolvePadding;
		float radialBlurCenter[2];
		float radialBlurWidth;
		float radialBlurSampleCount;
		float chromaticAberrationEnabled;
		float chromaticAberrationIntensity;
		float chromaticAberrationPadding[2];
		float selectiveBloomEnabled;
		float selectiveBloomIntensity;
		float selectiveBloomRadius;
		float selectiveBloomPadding;
		float fullscreenBinarizationEnabled;
		float binarizationThreshold;
		float binarizationPadding;
		float fullscreenGrayscaleIntensity;
		float glitchEnabled;
		float glitchIntensity;
		float glitchTime;
	};

	// Mapで取得した書き込み先。CPUから定数バッファの内容を更新する。
	PostEffectParameters* postEffectParameterMappedData_ = nullptr;

	// CPU側で保持する各ポストエフェクトの設定値。
	float vignetteStrength_ = 0.0f;
	Vector3 vignetteColor_ = {0.0f, 0.0f, 0.0f};
	bool randomNoiseEnabled_ = false;
	float randomNoiseScale_ = 512.0f;
	float randomNoiseTime_ = 0.0f;
	int randomNoiseBlendMode_ = 0;
	int boxFilterKernelSize_ = 1;
	int fullscreenFilterType_ = 0;
	float gaussianFilterSigma_ = 2.0f;
	bool dissolveEnabled_ = false;
	float dissolveThreshold_ = 0.0f;
	float dissolveEdgeWidth_ = 0.02f;
	Vector2 radialBlurCenter_ = {0.5f, 0.5f};
	float radialBlurWidth_ = 0.01f;
	int radialBlurSampleCount_ = 10;
	bool chromaticAberrationEnabled_ = false;
	float chromaticAberrationIntensity_ = 0.0f;
	bool fullscreenGrayscaleEnabled_ = false;
	float fullscreenGrayscaleIntensity_ = 1.0f;
	float fullscreenGrayscaleOverlayIntensity_ = 0.0f;
	bool glitchEnabled_ = false;
	float glitchIntensity_ = 0.0f;
	bool fullscreenSepiaEnabled_ = false;
	bool selectiveBloomEnabled_ = true;
	float selectiveBloomIntensity_ = 1.5f;
	float selectiveBloomRadius_ = 18.0f;
	bool fullscreenBinarizationEnabled_ = false;
	float binarizationThreshold_ = 0.5f;

	// エディター表示と、現在のフレーム・描画先の状態。
	bool editorLayoutEnabled_ = false;
	bool sceneCopiedToBackBufferThisFrame_ = false;
	bool inOutlineRenderTarget_ = false;
	bool inEmissionRenderTarget_ = false;
	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

	// HLSLファイルの読み込み、コンパイル、includeの解決に使用する。
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

	// 奥行き判定に使用する深度・ステンシルバッファ。
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStenicilResource_ = nullptr;

	// 初期値0でFenceをつくる
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	// FenceのSignalを持つためのイベントを作成する
	HANDLE fenceEvent_;

	// ビューポートは描画結果を配置する画面領域を指定する。
	D3D12_VIEWPORT viewport_;
	// シザー矩形
	D3D12_RECT scissorRect_;

	// transitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier_{};

	// 前フレームからの経過秒数と、フレーム時間の計測基準。
	float deltaTime_ = 1.0f / 60.0f;
	WinApp* winApp_ = nullptr;
	std::chrono::steady_clock::time_point reference_;

public:
	// ウィンドウを受け取り、描画に必要なDirectXオブジェクトを生成する。
	void initialize(WinApp* winApp);
	void InitializeFixFPS();

	// フレーム時間の更新と、フレーム前後の描画処理。
	void UpdateFixFPS();
	void PreDraw();
	void PostDraw();

	// メイン描画先を設定してクリアする／クリアせずに復帰する。
	void SetMainRenderTarget();
	void RestoreMainRenderTarget();

	// 記録済みコマンドをGPUで実行し、完了後に記録状態を再準備する。
	void ExecuteCommandListAndWait();

	// シーンテクスチャへポストエフェクトを適用し、バックバッファへ描画する。
	void DrawSceneTextureToBackBuffer();
	void EnsureSceneTextureCopiedToBackBuffer();

	// CPUから書き込めるUPLOADヒープ上のバッファを作成する。
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytes);
	void Finalize();
	float GetDeltaTime() const { return deltaTime_; }

	// ビネットの強さと色。
	void SetVignetteStrength(float strength);
	float GetVignetteStrength() const { return vignetteStrength_; }
	void SetVignetteColor(const Vector3& color);
	Vector3 GetVignetteColor() const { return vignetteColor_; }

	// ランダムノイズの有効状態、スケール、合成方式。
	void SetRandomNoiseEnabled(bool enabled);
	bool GetRandomNoiseEnabled() const { return randomNoiseEnabled_; }
	void SetRandomNoiseScale(float scale);
	float GetRandomNoiseScale() const { return randomNoiseScale_; }
	void SetRandomNoiseBlendMode(int blendMode);
	int GetRandomNoiseBlendMode() const { return randomNoiseBlendMode_; }

	// ボックスフィルターとガウシアンフィルターの設定。
	void SetBoxFilterKernelSize(int kernelSize);
	int GetBoxFilterKernelSize() const { return boxFilterKernelSize_; }
	void SetFullscreenFilterType(int filterType);
	int GetFullscreenFilterType() const { return fullscreenFilterType_; }
	void SetGaussianFilterSigma(float sigma);
	float GetGaussianFilterSigma() const { return gaussianFilterSigma_; }

	// 放射状ブラーの中心、幅、サンプル数。
	void SetRadialBlurCenter(const Vector2& center);
	Vector2 GetRadialBlurCenter() const { return radialBlurCenter_; }
	void SetRadialBlurWidth(float width);
	float GetRadialBlurWidth() const { return radialBlurWidth_; }
	void SetRadialBlurSampleCount(int sampleCount);
	int GetRadialBlurSampleCount() const { return radialBlurSampleCount_; }

	// 色収差とディゾルブの設定。
	void SetChromaticAberrationEnabled(bool enabled);
	bool GetChromaticAberrationEnabled() const { return chromaticAberrationEnabled_; }
	void SetChromaticAberrationIntensity(float intensity);
	float GetChromaticAberrationIntensity() const { return chromaticAberrationIntensity_; }
	void SetDissolveEnabled(bool enabled);
	bool GetDissolveEnabled() const { return dissolveEnabled_; }
	void SetDissolveThreshold(float threshold);
	float GetDissolveThreshold() const { return dissolveThreshold_; }
	void SetDissolveEdgeWidth(float width);
	float GetDissolveEdgeWidth() const { return dissolveEdgeWidth_; }

	// グレースケールとグリッチの設定。
	void SetFullscreenGrayscaleEnabled(bool enabled) { fullscreenGrayscaleEnabled_ = enabled; }
	bool IsFullscreenGrayscaleEnabled() const { return fullscreenGrayscaleEnabled_; }
	void SetFullscreenGrayscaleIntensity(float intensity);
	float GetFullscreenGrayscaleIntensity() const { return fullscreenGrayscaleIntensity_; }
	void SetFullscreenGrayscaleOverlayIntensity(float intensity);
	void SetGlitchEnabled(bool enabled);
	bool GetGlitchEnabled() const { return glitchEnabled_; }
	void SetGlitchIntensity(float intensity);
	float GetGlitchIntensity() const { return glitchIntensity_; }

	// セピアと二値化の設定。
	void SetFullscreenSepiaEnabled(bool enabled) { fullscreenSepiaEnabled_ = enabled; }
	bool IsFullscreenSepiaEnabled() const { return fullscreenSepiaEnabled_; }
	void SetFullscreenBinarizationEnabled(bool enabled);
	bool IsFullscreenBinarizationEnabled() const { return fullscreenBinarizationEnabled_; }
	void SetBinarizationThreshold(float threshold);
	float GetBinarizationThreshold() const { return binarizationThreshold_; }

	// エディター表示と、発光部分を対象にするブルームの設定。
	void SetEditorLayoutEnabled(bool enabled) { editorLayoutEnabled_ = enabled; }
	bool IsEditorLayoutEnabled() const { return editorLayoutEnabled_; }
	void SetSelectiveBloomEnabled(bool enabled);
	bool IsSelectiveBloomEnabled() const { return selectiveBloomEnabled_; }
	void SetSelectiveBloomIntensity(float intensity);
	float GetSelectiveBloomIntensity() const { return selectiveBloomIntensity_; }
	void SetSelectiveBloomRadius(float radius);
	float GetSelectiveBloomRadius() const { return selectiveBloomRadius_; }

	// 発光用・アウトライン用テクスチャへの描画開始と終了。
	void BeginEmissionRenderTarget();
	void EndEmissionRenderTarget();
	void BeginOutlineRenderTarget();
	void EndOutlineRenderTarget();

	// 他の描画クラスが使用するDirectXオブジェクトを取得する。
	ID3D12Device* GetDevice() { return device_.Get(); };
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); };
	ID3D12CommandQueue* GetCommandQueueRaw() { return commandQueue_.Get(); }
	ID3D12Resource* GetCurrentBackBufferResource() {
		if (backBufferIndex_ >= swapChainResources_.size()) {
			return nullptr;
		}
		return swapChainResources_[backBufferIndex_].Get();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilViewHandle() const { return dsvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(); }

	D3D12_RENDER_TARGET_VIEW_DESC GetRtvDesc() { return rtvDesc_; }

	size_t GetSwapChainResourcesNum() const { return swapChainResources_.size(); }
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> GetCommandQueue() { return commandQueue_; }

	// HLSLファイルを指定したシェーダープロファイルでコンパイルする。
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader( // CompilerするShaderファイルへのパス
	    const std::wstring& filePath,
	    // Compilerに使用するProfile
	    const wchar_t* profile);

	// 指定した種類・個数・シェーダー可視性のディスクリプタヒープを作成する。
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

private:
	// デバイスと描画コマンドの初期化。
	void DebugLayer();
	void DeviceInitialize();
	void DebugError();
	void CommandListInitialize();
	void SwapChainInitialize();
	void DepthBufferCreate();
	void DescriptorHeapCreate();

	// オフスクリーン描画とポストエフェクトに必要なリソースの生成。
	void SceneColorResourceCreate();
	void SceneColorViewCreate();
	void DissolveMaskTextureCreate();
	void SceneCopyPipelineCreate();

	// 各ビュー、GPU同期、描画範囲、シェーダーコンパイラの初期化。
	void RenderTargetViewInitialize();
	void DepthStencilViewInitialize();
	void FenceCreate();
	void ViewportRectInitialize();
	void ScissorRectInitialize();
	void DXCCompilerCreate();

	void FrameStart();      // フレーム最初の準備
	void DrawCommandList(); // 描画コマンドリスト

	// バックバッファを表示可能な状態へ遷移させる。
	void CrtvTransitionBarrier();
};
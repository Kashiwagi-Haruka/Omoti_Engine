#pragma once
#include "D3DResourceLeakChecker.h"
#include "FrameWork.h"
#include "GameBase.h"
#include "Scene/SceneFactory/SceneFactory.h"
#include <memory>
class BaseScene;
/// <summary>
/// ゲームのメインクラス
/// </summary>
class Game : public FrameWork {

	D3DResourceLeakChecker d3dResourceLeakChecker; // Direct3Dリソースリークチェッカー

	std::unique_ptr<SceneFactory> sceneFactory_ = nullptr; // シーンファクトリー

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Game() = default;
};

#pragma once
#include "Input.h"
#include "Engine/Texture/Mesh/SkyBox/SkyBox.h"
#include "Transform.h"
#include <memory>

class GameBase;
class Camera;

/// <summary>
/// スカイ
/// </summary>
class Sky {

private:
	Transform transform_; // スカイ
	std::unique_ptr<SkyBox> skyBox_ = nullptr; // スカイボックス
	Camera* camera_;                           // カメラ

public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	Sky();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Sky() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera"> カメラ </param>
	void Initialize(Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// カメラを設定する
	/// </summary>
	/// <param name="camera"> カメラ </param>
	void SetCamera(Camera* camera) { camera_ = camera; }
};

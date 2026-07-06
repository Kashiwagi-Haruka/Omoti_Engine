#pragma once
#include <memory>
#include "Object3d/Object3d.h"
#include "Transform.h"
/// <summary>
/// キャラクター画面のスカイドーム
/// </summary>
class CharacterDisplaySkyDome {

	std::unique_ptr<Object3d> object_; // スカイドームのオブジェクト
	Transform transform_;              // スカイドームの変換情報

	public:

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
};

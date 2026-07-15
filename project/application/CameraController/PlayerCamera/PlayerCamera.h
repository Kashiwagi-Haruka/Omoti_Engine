#pragma once
#include "Transform.h"
#include <memory>
class Camera;
/// <summary>
/// プレイヤーの視点を制御するカメラクラス
/// </summary>
class PlayerCamera {

	Transform transform_{}; // カメラのトランスフォーム
	std::unique_ptr<Camera> camera_; // カメラのインスタンス
	float mouseSensitivity_ = 0.002f; // マウス感度
	float stickSensitivity_ = 0.04f;  // スティック感度
	Vector3 playerPos = {0.0f, 0.0f, 0.0f}; // プレイヤーの位置
	float orbitYaw_ = 0.0f;                 // カメラのヨー角
	float orbitPitch_ = 0.15f;              // カメラのピッチ角
	float shakeTimer_ = 0.0f;               // カメラの揺れのタイマー
	float shakeDuration_ = 0.0f;            // カメラの揺れの継続時間
	float shakeAmplitude_ = 0.6f;           //	カメラの揺れの振幅

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PlayerCamera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラを揺らす
	/// </summary>
	/// <param name="durationSeconds"></param>
	void StartShake(float durationSeconds = 1.0f);

	/// <summary>
	/// カメラを取得する
	/// </summary>
	/// <returns></returns>
	Camera* GetCamera();

	/// <summary>
	/// カメラのトランスフォームを取得する
	/// </summary>
	/// <returns></returns>
	Transform GetTransform() { return transform_; }

	/// <summary>
	/// カメラのトランスフォームを設定する
	/// </summary>
	/// <param name="transform"></param>
	void SetTransform(Transform transform) { transform_ = transform; }

	/// <summary>
	/// カメラの回転をプレイヤーカメラの操作角度に反映する
	/// </summary>
	/// <param name="rotate"> 引き継ぐ回転 </param>
	void SetOrbitRotation(const Vector3& rotate);

	/// <summary>
	/// プレイヤーの位置を設定する
	/// </summary>
	/// <param name="pos"></param>
	void SetPlayerPos(const Vector3& pos) { playerPos = pos; }

		/// <summary>
	/// プレイヤー位置を基準に指定位置の方向へ視点を向ける
	/// </summary>
	/// <param name="targetPos"> 視点を向ける位置 </param>
	void LookAtFromPlayerPosition(const Vector3& targetPos);
};

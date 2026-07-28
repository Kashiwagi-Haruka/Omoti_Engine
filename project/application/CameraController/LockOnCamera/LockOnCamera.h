#pragma once
#include "Transform.h"
#include <memory>

class Camera;

/// <summary>
/// 敵をロックオンするカメラ処理のクラス
/// </summary>
class LockOnCamera {

	Transform transform_{}; // カメラのトランスフォーム
	std::unique_ptr<Camera> camera_; // カメラのインスタンス
	float mouseSensitivity_ = 0.002f; // マウス感度
	Vector3 playerPos = {0.0f, 0.0f, 0.0f}; // プレイヤーの座標
	float orbitYaw_ = 0.0f;                 // カメラの回転角度（ヨー軸）
	float orbitPitch_ = 0.55f;              // カメラの回転角度（ピッチ軸）
	float shakeTimer_ = 0.0f;               // カメラの揺れのタイマー
	float shakeDuration_ = 0.0f;            // カメラの揺れの継続時間
	float shakeAmplitude_ = 0.6f;           // カメラの揺れの振幅
	bool hasTarget_ = false;                // ロックオン対象がいるかどうかのフラグ
	Vector3 targetPos_ = {0.0f, 0.0f, 0.0f}; // ロックオン対象の座標
	bool hasFollowPosition_ = false;         // プレイヤーの座標を追従するかどうかのフラグ
	Vector3 followPosition_ = {0.0f, 0.0f, 0.0f}; // プレイヤーの座標
	float distance_ = 12.0f;                       // カメラとプレイヤーの距離

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	LockOnCamera();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LockOnCamera();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	///　更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラを揺らす
	/// </summary>
	/// <param name="durationSeconds"> 揺れの継続時間（秒） </param>
	void StartShake(float durationSeconds = 1.0f);

	/// <summary>
	/// カメラのインスタンスを取得する
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
	/// <param name="transform"> カメラのトランスフォーム </param>
	void SetTransform(Transform transform) { transform_ = transform; }

	/// <summary>
	/// プレイヤーの位置を設定する
	/// </summary>
	/// <param name="pos"> プレイヤーの位置 </param>
	void SetPlayerPos(const Vector3& pos) { playerPos = pos; }

	/// <summary>
	/// カメラのピッチ角度を設定する
	/// </summary>
	/// <param name="pitch"> カメラのピッチ角度 </param>
	void SetOrbitPitch(float pitch) { orbitPitch_ = pitch; }

	/// <summary>
	/// ロックオン対象の位置を設定する
	/// </summary>
	/// <param name="pos"> ロックオン対象の位置 </param>
	void SetTargetPos(const Vector3& pos) {
		targetPos_ = pos;
		hasTarget_ = true;
	}

	/// <summary>
	/// ロックオン対象をクリアする
	/// </summary>
	void ClearTarget() { hasTarget_ = false; }

	/// <summary>
	/// プレイヤーの追従位置を設定する
	/// </summary>
	/// <param name="pos"> プレイヤーの追従位置 </param>
	void SetFollowPosition(const Vector3& pos) {
		followPosition_ = pos;
		hasFollowPosition_ = true;
	}

	/// <summary>
	/// プレイヤーの追従位置をクリアする
	/// </summary>
	void ClearFollowPosition() { hasFollowPosition_ = false; }

	/// <summary>
	/// カメラとプレイヤーの距離を設定する
	/// </summary>
	/// <param name="distance"> カメラとプレイヤーの距離 </param>
	void SetDistance(float distance) { distance_ = distance; }
};
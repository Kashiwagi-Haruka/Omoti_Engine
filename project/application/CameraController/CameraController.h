#pragma once
#include "LockOnCamera/LockOnCamera.h"
#include "PlayerCamera/PlayerCamera.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>

class Camera;

/// <summary>
/// カメラの制御管理クラス
/// </summary>
class CameraController {
	/// <summary>
	/// カメラのモード
	/// </summary>
	enum class CameraMode { 
		kPlayerCamera, // プレイヤーカメラ
		kLockOnCamera, // ロックオンカメラ
		kNormalAttackCamera // 通常攻撃カメラ
	};
	CameraMode cameraMode_ = CameraMode::kPlayerCamera; // 現在のカメラモード
	CameraMode preCameraMode_ = CameraMode::kPlayerCamera; // 前回のカメラモード
	std::unique_ptr<PlayerCamera> playerCamera_;           // プレイヤーカメラのインスタンス
	std::unique_ptr<LockOnCamera> lockOnCamera_;           // ロックオンカメラのインスタンス
	std::unique_ptr<Camera> blendCamera_;                  // ブレンドカメラのインスタンス

	Vector3 playerPos = {0.0f, 0.0f, 0.0f}; // プレイヤーの位置
	Transform transform_{};                 // カメラの変換情報
	Camera* camera_ = nullptr;              // カメラのインスタンス

	bool isCameraSwitching_ = false; // カメラ切り替え中かどうかのフラグ
	float cameraSwitchTimer_ = 0.0f; // カメラ切り替えのタイマー
	float cameraSwitchDuration_ = 0.2f; // カメラ切り替えの時間
	float autoLockOnTimer_ = 0.0f;      // 自動ロックオンのタイマー
	Transform switchStartTransform_{};  // カメラ切り替え開始時の変換情報

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// カメラの取得
	/// </summary>
	/// <returns></returns>
	Camera* GetCamera();

	/// <summary>
	/// カメラのトランスフォームの取得
	/// </summary>
	/// <returns></returns>
	Transform GetTransform() { return transform_; }

	/// <summary>
	/// カメラのトランスフォームの設定
	/// </summary>
	/// <param name="transform"></param>
	void SetTransform(Transform transform) { transform_ = transform; }

	/// <summary>
	/// プレイヤーの位置の設定
	/// </summary>
	/// <param name="pos"> プレイヤーの位置 </param>
	void SetPlayerPos(const Vector3& pos) { playerPos = pos; }

	/// <summary>
	/// カメラを振動させる
	/// </summary>
	/// <param name="durationSeconds"> 振動時間（秒） </param>
	void StartShake(float durationSeconds = 1.0f);

	/// <summary>
	/// カメラモードの設定
	/// </summary>
	/// <param name="mode"> カメラモード </param>
	void SetCameraMode(CameraMode mode) { cameraMode_ = mode; }

	/// <summary>
	/// ロックオンターゲットの設定
	/// </summary>
	/// <param name="targetPos"> ターゲットの位置 </param>
	/// <param name="durationSeconds"> 持続時間（秒） </param>
	void SetLockOnTarget(const Vector3& targetPos, float durationSeconds = 1.0f);
};
#pragma once
#include "LockOnCamera/LockOnCamera.h"
#include "PlayerCamera/PlayerCamera.h"
#include "NormalAttackCamera/NormalAttackCamera.h"
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
	std::unique_ptr<NormalAttackCamera> normalAttackCamera_;//通常攻撃カメラのインスタンス
	std::unique_ptr<Camera> blendCamera_;                  // ブレンドカメラのインスタンス

	Vector3 playerPos = {0.0f, 0.0f, 0.0f}; // プレイヤーの位置
	Transform transform_{};                 // カメラの変換情報
	Camera* camera_ = nullptr;              // カメラのインスタンス

	bool isCameraSwitching_ = false; // カメラ切り替え中かどうかのフラグ
	float cameraSwitchTimer_ = 0.0f; // カメラ切り替えのタイマー
	float cameraSwitchDuration_ = 0.2f; // 現在のカメラ切り替えの時間
	bool hasPendingCameraMode_ = false;                        // 補間完了後に切り替えるカメラモードがあるか
	CameraMode pendingCameraMode_ = CameraMode::kPlayerCamera; // 補間完了後に切り替えるカメラモード
	static constexpr float kDefaultCameraSwitchDuration_ = 0.2f;
	static constexpr float kPlayerCameraSwitchDuration_ = 0.35f;
	static constexpr float kNormalAttackToLockOnSwitchDuration_ = 0.35f;
	float autoLockOnTimer_ = 0.0f;      // 自動ロックオンのタイマー
	float normalAttackIdleTimer_ = 1.0f; // 通常攻撃ボタンが押されていない時間
	Transform switchStartTransform_{};  // カメラ切り替え開始時の変換情報

	/// <summary>
	/// 指定されたカメラモードの回転をプレイヤーカメラへ引き継ぐ
	/// </summary>
	/// <param name="sourceMode"> 引き継ぎ元のカメラモード </param>
	void InheritPlayerCameraRotation(CameraMode sourceMode);

		/// <summary>
	/// プレイヤーカメラを出入りする切り替えかどうかを判定する
	/// </summary>
	bool IsPlayerCameraTransition(CameraMode from, CameraMode to) const;
	/// <summary>
	/// カメラモードの切り替えを要求する。補間中の要求は完了後まで保留する
	/// </summary>
	void RequestCameraMode(CameraMode mode);
	/// <summary>
	/// 操作用プレイヤーカメラへ戻し、攻撃カメラのターゲットを解除する
	/// </summary>
	void ReturnToPlayerCamera();

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
	/// プレイヤーカメラの視点を指定位置の方向へ向ける
	/// </summary>
	/// <param name="targetPos"> 視点を向ける位置 </param>
	void LookAtFromPlayerPosition(const Vector3& targetPos);
	/// <summary>
	/// カメラモードの設定
	/// </summary>
	/// <param name="mode"> カメラモード </param>
	void SetCameraMode(CameraMode mode) { RequestCameraMode(mode); }

	/// <summary>
	/// ロックオンターゲットの設定
	/// </summary>
	/// <param name="targetPos"> ターゲットの位置 </param>
	/// <param name="durationSeconds"> 持続時間（秒） </param>
	void SetLockOnTarget(const Vector3& targetPos, float durationSeconds = 1.0f);

	/// <summary>
	/// 通常攻撃カメラのターゲット設定
	/// </summary>
	void SetNormalAttackTarget(const Vector3& targetPos);
	/// <summary>
	/// 攻撃カメラを終了してプレイヤーカメラへ戻す
	/// </summary>
	void ClearAttackCameraTarget();
};
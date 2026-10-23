#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include "Vector2.h"
#include "WinApp.h"
#include <Windows.h>
#include <Xinput.h>
#include <cstdint>
#include <dinput.h>
#include <memory>
#include <wrl.h>

// キーボード、ゲームパッド、マウスの入力を管理する。
// 現在と前フレームの状態を比較し、押下・押した瞬間・離した瞬間を判定する。
class Input {
	// ゲーム全体で共有するインスタンス。
	static std::unique_ptr<Input> instance_;

	// COMオブジェクトの参照を管理するスマートポインターの別名。
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	// DirectInput本体と、各入力デバイス。
	ComPtr<IDirectInput8> directInput = nullptr;
	ComPtr<IDirectInputDevice8> keyboard;
	ComPtr<IDirectInputDevice8> gamePadDevice_;
	ComPtr<IDirectInputDevice8> mouseDevice_;

	// 現在と前フレームのキーボード状態。
	BYTE key[256] = {0};
	BYTE preKey[256] = {0};

	DIJOYSTATE padState_{};    // ゲームパッドの現在の状態
	DIJOYSTATE prePadState_{}; // 前フレームの状態

	// DirectInputのスティック判定に使用する。
	// XInput側は、APIで定義されたデッドゾーンを使用する。
	float deadZone_ = 0.2f; // デッドゾーン(0.0f～1.0f)

	// マウスの相対移動量、ホイール、ボタンの状態。
	DIMOUSESTATE2 mouseState_{};
	DIMOUSESTATE2 prevMouseState_{};

	// Windows APIで取得する、クライアント座標系のマウス位置。
	LONG mouseX_ = 0;
	LONG mouseY_ = 0;

	// 現在の実装では使用していないカウンター。
	int noPadCounter = 0;

	// DirectInputでトリガーをボタンとして扱う場合の番号。
	int leftTriggerButtonIndex_ = 10;
	int rightTriggerButtonIndex_ = 11;

	// XInputの接続状態と、現在・前フレームの入力。
	bool isXInputConnected_ = false;
	XINPUT_STATE xInputState_{};
	XINPUT_STATE preXInputState_{};

	// 入力対象のウィンドウ。所有権は持たない。
	WinApp* winApp_ = nullptr;

	// カーソル固定の要求と、実際に適用する固定状態。
	bool isCursorStabilityRequested_ = false;
	bool isCursorStability = false;

	// 通常の表示要求と、最後に適用した表示状態。
	bool isCursorVisibleRequested_ = true;
	bool isCursorVisible_ = true;

	// F1キーで設定する表示状態の上書き。
	bool isCursorVisibilityForceEnabled_ = false;
	bool isCursorVisibilityForced_ = true;

public:
	// 共有インスタンスを取得する。初回アクセス時に生成する。
	static Input* GetInstance();

	// 入力管理を複製・移動できないようにする。
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator=(Input&&) = delete;

	// ゲームパッドの論理ボタン。
	// XInput使用時は、対応するボタンマスクへ変換する。
	enum class PadButton {
		kButtonA = 0,         // rgbButtons[0]
		kButtonB,             // rgbButtons[1]
		kButtonX,             // rgbButtons[2]
		kButtonY,             // rgbButtons[3]
		kButtonLeftShoulder,  // rgbButtons[4]
		kButtonRightShoulder, // rgbButtons[5]
		kButtonBack,          // rgbButtons[6]
		kButtonStart,         // rgbButtons[7]
		kButtonLeftThumb,     // rgbButtons[8]
		kButtonRightThumb,    // rgbButtons[9]

		// 十字キーは POV（rgdwPOV[0]）を別判定で扱う
		kButtonUp,
		kButtonDown,
		kButtonLeft,
		kButtonRight,

		kMaxButtons
	};

	// スティックの傾きを分類する8方向。
	enum class JoyconStickDirection {

		UP,        // 上
		UPRIGHT,   // 右上
		RIGHT,     // 右
		DOWNRIGHT, // 右下
		DOWN,      // 下
		DOWNLEFT,  // 左下
		LEFT,      // 左
		UPLEFT     // 左上

	};

	// このクラスで判定対象にするマウスボタン。
	enum class MouseButton { kLeft = 0, kRight, kMiddle, kMaxButtons };

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="wc"></param>
	/// <param name="hwnd"></param>
	// DirectInputを作成し、各デバイスの取得形式と協調レベルを設定する。
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 更新
	/// </summary>
	// 各デバイスの状態取得と、カーソル表示・固定の適用を行う。
	void Update();

	// キーボード

	/// <summary>
	/// キーが押されているか
	/// </summary>
	/// <param name="keyNumber">キーコード</param>
	/// <returns></returns>
	// DIK_SPACEなど、DirectInputのキーコードを指定する。
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーが押された瞬間か
	/// </summary>
	/// <param name="keyNumber"></param>
	/// <returns></returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーが離された瞬間か
	/// </summary>
	/// <param name="keyNumber">キーコード</param>
	/// <returns></returns>
	bool ReleaseKey(BYTE keyNumber);

	// ゲームパッド
	// XInputの接続がある場合は、そちらの状態を優先する。
	bool PushButton(PadButton button);    // ボタンが押されているか
	bool TriggerButton(PadButton button); // ボタンが押された瞬間か
	bool ReleaseButton(PadButton button); // ボタンが離されているか
	// ReleaseButtonは、押下状態から非押下状態へ変化したフレームを判定する。

	// ジョイスティック

	/// <summary>
	/// ジョイスティックのX軸を取得
	/// </summary>
	/// <returns></returns>
	float GetJoyStickLX() const;

	/// <summary>
	/// ジョイスティックのY軸を取得
	/// </summary>
	/// <returns></returns>
	float GetJoyStickLY() const;

	/// <summary>
	/// Vector2でジョイスティックのXYを取得
	/// </summary>
	/// <returns></returns>
	Vector2 GetJoyStickLXY() const;

	/// <summary>
	/// 選んだ向きに左スティックが傾けられているか
	/// </summary>
	/// <param name="direction"></param>
	/// <returns></returns>
	bool IsJoyStickSelectDirectionL(JoyconStickDirection direction) const;

	// 右スティック
	float GetJoyStickRX() const;
	float GetJoyStickRY() const;
	Vector2 GetJoyStickRXY() const;

	/// <summary>
	/// 選んだ向きに右スティックが傾けられているか
	/// </summary>
	/// <param name="direction"></param>
	/// <returns></returns>
	bool IsJoyStickSelectDirectionR(JoyconStickDirection direction) const;

	// マウス
	float GetMouseX() const;                           // マウスのX座標を取得
	float GetMouseY() const;                           // マウスのY座標を取得
	Vector2 GetMouseMove() const;                      // マウスの移動量を取得
	float GetMouseWheelDelta() const;                  // マウスホイールの移動量を取得
	bool PushMouseButton(MouseButton button) const;    // マウスボタンが押されているか
	bool TriggerMouseButton(MouseButton button) const; // マウスボタンが押された瞬間か
	bool ReleaseMouseButton(MouseButton button) const; // マウスボタンが離された瞬間か

	// カーソルをウィンドウ中央へ固定するかを設定する。
	// 表示が強制的に有効になっている間は、固定を解除する。
	void SetIsCursorStability(bool isCursor) {
		isCursorStabilityRequested_ = isCursor;
		isCursorStability = (isCursorVisibilityForceEnabled_ && isCursorVisibilityForced_) ? false : isCursor;
	}

	// 通常のカーソル表示要求を設定する。実際の反映はUpdateで行う。
	void SetIsCursorVisible(bool isVisible);

	// 左右トリガーの押し込み量を0～1で取得する。
	float GetLeftTrigger() const;
	float GetRightTrigger() const;

	// 押し込み量がしきい値以上かを判定する。
	bool PushLeftTrigger(float threshold = 0.5f) const;
	bool PushRightTrigger(float threshold = 0.5f) const;

	// 押し込み量がしきい値未満から以上へ変化した瞬間を判定する。
	bool TriggerLeftTrigger(float threshold = 0.5f) const;
	bool TriggerRightTrigger(float threshold = 0.5f) const;
	;

	/// <summary>
	/// デッドゾーンの設定
	/// </summary>
	/// <param name="deadZone">初期値は0.2f</param>
	// DirectInput側のスティックに適用する値を0～1へ制限して保存する。
	void SetDeadZone(float deadZone);

private:
	// 外部からの直接生成を防ぎ、GetInstance経由で生成する。
	Input() = default;

	// Windowsのカーソル表示カウンターを調整して表示状態を反映する。
	void ApplyCursorVisibility(bool isVisible);

	// デバイス列挙用の static コールバック
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);
};
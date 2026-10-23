#define NOMINMAX
#include "Input.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numbers>

#ifdef USE_IMGUI
#include "imgui.h"
#endif

// DirectInputとXInputで使用するライブラリ。
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

using namespace Microsoft::WRL;

// 共有インスタンスの実体。
std::unique_ptr<Input> Input::instance_ = nullptr;

namespace {

// 値を0～1へ制限する。
float Clamp01(float value) { return std::clamp(value, 0.0f, 1.0f); }

// DirectInputの指定した軸について、取得値の最小・最大を設定する。
bool SetJoystickAxisRange(LPDIRECTINPUTDEVICE8 device, DWORD objectOffset, LONG minValue, LONG maxValue) {
	if (!device) {
		return false;
	}

	DIPROPRANGE range{};
	range.diph.dwSize = sizeof(DIPROPRANGE);
	range.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	range.diph.dwObj = objectOffset;
	range.diph.dwHow = DIPH_BYOFFSET;
	range.lMin = minValue;
	range.lMax = maxValue;

	return SUCCEEDED(device->SetProperty(DIPROP_RANGE, &range.diph));
}

// 各軸の入力範囲を0～65535へ揃える。
// デバイスが対応していない軸の設定結果は、ここでは使用しない。
void ConfigureJoystickAxisRanges(LPDIRECTINPUTDEVICE8 device) {
	constexpr LONG kAxisMin = 0;
	constexpr LONG kAxisMax = 65535;

	SetJoystickAxisRange(device, DIJOFS_X, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_Y, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_Z, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_RX, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_RY, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_RZ, kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_SLIDER(0), kAxisMin, kAxisMax);
	SetJoystickAxisRange(device, DIJOFS_SLIDER(1), kAxisMin, kAxisMax);
}

// トリガーを通常ボタンとして取得する場合の判定。
// 押されていれば1、押されていなければ0を返す。
float GetDigitalTrigger(const DIJOYSTATE& state, int buttonIndex) {
	if (buttonIndex < 0 || buttonIndex >= 32) {
		return 0.0f;
	}
	return (state.rgbButtons[buttonIndex] & 0x80) != 0 ? 1.0f : 0.0f;
}

// 共有Z軸の中心より小さい側を、左トリガーの押し込み量へ変換する。
float GetCombinedTriggerNegative(LONG zAxis) { return Clamp01((32767.0f - static_cast<float>(zAxis)) / 32767.0f); }

// 共有Z軸の中心より大きい側を、右トリガーの押し込み量へ変換する。
float GetCombinedTriggerPositive(LONG zAxis) { return Clamp01((static_cast<float>(zAxis) - 32767.0f) / 32767.0f); }

// 共有軸と通常ボタンの情報を組み合わせ、左右トリガーの値を求める。
void GetCombinedTriggerValues(const DIJOYSTATE& state, int leftButtonIndex, int rightButtonIndex, float& outLeft, float& outRight) {
	// 共有Z軸は LT=負側, RT=正側 の固定割り当てにする
	outLeft = GetCombinedTriggerNegative(state.lZ);
	outRight = GetCombinedTriggerPositive(state.lZ);

	const float leftDigital = GetDigitalTrigger(state, leftButtonIndex);
	const float rightDigital = GetDigitalTrigger(state, rightButtonIndex);

	// ボタン情報で左右同時押しを検出した場合は、両方を最大値にする。
	if (leftDigital > 0.0f && rightDigital > 0.0f) {
		outLeft = 1.0f;
		outRight = 1.0f;
	} else {
		outLeft = std::max(outLeft, leftDigital);
		outRight = std::max(outRight, rightDigital);
	}
}

// XInputの0～255を、0～1の押し込み量へ変換する。
float NormalizeXInputTrigger(BYTE triggerValue) { return Clamp01(static_cast<float>(triggerValue) / 255.0f); }

// XInputのスティック値にデッドゾーンを適用する。
// デッドゾーンの外側を改めて0～1へ広げ、最後に元の符号を戻す。
float NormalizeXInputThumb(SHORT value, SHORT deadZone) {
	const int rawValue = static_cast<int>(value);
	const int deadZoneValue = static_cast<int>(deadZone);
	const int absValue = std::abs(rawValue);

	if (absValue <= deadZoneValue) {
		return 0.0f;
	}

	// SHORTの正側と負側で最大の大きさが異なることを考慮する。
	const int maxValue = rawValue < 0 ? 32768 : 32767;
	const float normalized = static_cast<float>(absValue - deadZoneValue) / static_cast<float>(maxValue - deadZoneValue);

	return std::copysign(Clamp01(normalized), static_cast<float>(rawValue));
}

// 独自のボタン列挙値を、XInputのボタンマスクへ変換する。
WORD GetXInputButtonMask(Input::PadButton button) {
	switch (button) {
	case Input::PadButton::kButtonA:
		return XINPUT_GAMEPAD_A;
	case Input::PadButton::kButtonB:
		return XINPUT_GAMEPAD_B;
	case Input::PadButton::kButtonX:
		return XINPUT_GAMEPAD_X;
	case Input::PadButton::kButtonY:
		return XINPUT_GAMEPAD_Y;
	case Input::PadButton::kButtonLeftShoulder:
		return XINPUT_GAMEPAD_LEFT_SHOULDER;
	case Input::PadButton::kButtonRightShoulder:
		return XINPUT_GAMEPAD_RIGHT_SHOULDER;
	case Input::PadButton::kButtonBack:
		return XINPUT_GAMEPAD_BACK;
	case Input::PadButton::kButtonStart:
		return XINPUT_GAMEPAD_START;
	case Input::PadButton::kButtonLeftThumb:
		return XINPUT_GAMEPAD_LEFT_THUMB;
	case Input::PadButton::kButtonRightThumb:
		return XINPUT_GAMEPAD_RIGHT_THUMB;
	case Input::PadButton::kButtonUp:
		return XINPUT_GAMEPAD_DPAD_UP;
	case Input::PadButton::kButtonDown:
		return XINPUT_GAMEPAD_DPAD_DOWN;
	case Input::PadButton::kButtonLeft:
		return XINPUT_GAMEPAD_DPAD_LEFT;
	case Input::PadButton::kButtonRight:
		return XINPUT_GAMEPAD_DPAD_RIGHT;
	default:
		return 0;
	}
}

// 指定したXInputボタンが押されているかを調べる。
bool GetXInputButtonState(const XINPUT_STATE& state, Input::PadButton button) {
	const WORD mask = GetXInputButtonMask(button);
	return mask != 0 && (state.Gamepad.wButtons & mask) != 0;
}

// 入力対象のウィンドウが、現在の前面ウィンドウかを調べる。
bool IsForegroundWindow(const WinApp* winApp) {
	if (!winApp) {
		return false;
	}
	return GetForegroundWindow() == winApp->GetHwnd();
}

// マウス位置が、クライアント領域内にあるかを調べる。
bool IsMouseInClientArea(const WinApp* winApp, LONG mouseX, LONG mouseY) {
	if (!winApp) {
		return false;
	}

	RECT clientRect{};
	if (!GetClientRect(winApp->GetHwnd(), &clientRect)) {
		return false;
	}

	return mouseX >= clientRect.left && mouseX < clientRect.right && mouseY >= clientRect.top && mouseY < clientRect.bottom;
}

// ImGuiがマウス入力を使用したい状態かを調べる。
bool IsImGuiCapturingMouse() {
#ifdef USE_IMGUI
	if (ImGui::GetCurrentContext() == nullptr) {
		return false;
	}
	return ImGui::GetIO().WantCaptureMouse;
#else
	return false;
#endif
}

// ImGuiがキーボード入力や文字入力を使用したい状態かを調べる。
bool IsImGuiCapturingKeyboard() {
#ifdef USE_IMGUI
	if (ImGui::GetCurrentContext() == nullptr) {
		return false;
	}
	return ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantTextInput;
#else
	return false;
#endif
}

} // namespace

// privateコンストラクターで生成し、共有インスタンスを返す。
Input* Input::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = std::unique_ptr<Input>(new Input());
	}
	return instance_.get();
}

// 列挙されたゲームパッドのデバイス作成を試みる。
// 最初に作成できたデバイスを採用し、列挙を終了する。
BOOL CALLBACK Input::EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext) {
	Input* input = reinterpret_cast<Input*>(pContext);
	HRESULT hr = input->directInput->CreateDevice(pdidInstance->guidInstance, &input->gamePadDevice_, NULL);

	if (FAILED(hr)) {
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

// DirectInputと、キーボード・ゲームパッド・マウスを初期化する。
void Input::Initialize(WinApp* winApp) {

	winApp_ = winApp;

	HRESULT result;

	// DirectInput 作成
	result = DirectInput8Create(winApp_->GetHinstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボード
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	// 前面時のみ取得し、他アプリとの共有を許可する。
	// DISCL_NOWINKEYにより、取得中のWindowsキーを無効にする。
	result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// ゲームパッド列挙 → 最初の1台を使用
	directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);

	if (gamePadDevice_) {
		result = gamePadDevice_->SetDataFormat(&c_dfDIJoystick);
		assert(SUCCEEDED(result));

		ConfigureJoystickAxisRanges(gamePadDevice_.Get());

		result = gamePadDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		assert(SUCCEEDED(result));
	}

	// マウスは、8ボタンまで保持できるDIMOUSESTATE2形式を使用する。
	result = directInput->CreateDevice(GUID_SysMouse, &mouseDevice_, nullptr);
	assert(SUCCEEDED(result));

	result = mouseDevice_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));

	result = mouseDevice_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

// 通常のカーソル表示要求を保存する。
void Input::SetIsCursorVisible(bool isVisible) { isCursorVisibleRequested_ = isVisible; }

// ShowCursorの内部カウンターを、表示・非表示になるまで調整する。
void Input::ApplyCursorVisibility(bool isVisible) {
	if (isCursorVisible_ == isVisible) {
		return;
	}

	if (isVisible) {
		while (ShowCursor(TRUE) < 0) {
		}
	} else {
		while (ShowCursor(FALSE) >= 0) {
		}
	}

	isCursorVisible_ = isVisible;
}

// 前フレームの状態を保存してから、各デバイスの最新状態を取得する。
void Input::Update() {
	// 前のフレームのキー入力を保存
	memcpy(preKey, key, sizeof(key));

	// キーボードの状態を取得
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key), key);

	// F1を押した瞬間に、カーソル表示の上書きを有効にして状態を切り替える。
	if (IsForegroundWindow(winApp_) && key[DIK_F1] && !preKey[DIK_F1]) {
		isCursorVisibilityForceEnabled_ = true;
		isCursorVisibilityForced_ = !isCursorVisible_;
	}

	// カーソル表示が強制されている間は、中央固定を解除する。
	isCursorStability = (isCursorVisibilityForceEnabled_ && isCursorVisibilityForced_) ? false : isCursorStabilityRequested_;
	ApplyCursorVisibility(isCursorVisibilityForceEnabled_ ? isCursorVisibilityForced_ : isCursorVisibleRequested_);

	// ゲームパッド
	prePadState_ = padState_;
	preXInputState_ = xInputState_;

	// XInputはユーザーインデックス0のデバイスを取得する。
	xInputState_ = {};
	isXInputConnected_ = XInputGetState(0, &xInputState_) == ERROR_SUCCESS;

	if (gamePadDevice_) {
		HRESULT hr = gamePadDevice_->Acquire();

		if (SUCCEEDED(hr)) {
			hr = gamePadDevice_->GetDeviceState(sizeof(DIJOYSTATE), &padState_);

			if (FAILED(hr)) {
				if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
					// 再度 Acquire を試みる
					gamePadDevice_->Acquire();
				} else {
					// 本当にダメな場合だけリセット
					gamePadDevice_.Reset();
				}
			}
		} else {
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				// 未取得なら再度 Acquire を試みる
				gamePadDevice_->Acquire();
			} else {
				// それ以外の致命的なエラーならリセット
				gamePadDevice_.Reset();
			}
		}
	}

	// WinAppからデバイス構成の変更通知が来ていれば、必要に応じて再列挙する。
	if (winApp_->GetIsPad()) {

		// --- もしデバイスが切れていたら再列挙 ---
		if (!gamePadDevice_) {
			directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);

			if (gamePadDevice_) {
				gamePadDevice_->SetDataFormat(&c_dfDIJoystick);
				ConfigureJoystickAxisRanges(gamePadDevice_.Get());
				gamePadDevice_->SetCooperativeLevel(GetActiveWindow(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			}
		}

		// 通知を処理したので、再列挙要求を解除する。
		winApp_->SetIsPad(false);
	}

	prevMouseState_ = mouseState_;

	// ２）DirectInput で相対移動だけ取得（マウスホイールやボタンは要るなら使う）
	// 取得した状態には、相対移動に加えてホイールとボタン情報も含まれる。
	if (mouseDevice_) {
		mouseDevice_->Acquire();
		mouseDevice_->GetDeviceState(sizeof(mouseState_), &mouseState_);
	}

	// ３）Windows API でマウスの絶対位置を取得
	POINT pt;
	GetCursorPos(&pt);                       // スクリーン座標
	ScreenToClient(winApp_->GetHwnd(), &pt); // クライアント座標に変換
	mouseX_ = pt.x;                          // ここではクランプせずそのまま代入
	mouseY_ = pt.y;

	// 固定が有効で、対象ウィンドウが前面の場合はカーソルを中央へ戻す。
	if (isCursorStability) {
		if (GetForegroundWindow() == winApp_->GetHwnd()) {
			RECT clientRect{};

			if (GetClientRect(winApp_->GetHwnd(), &clientRect)) {
				POINT center{(clientRect.right - clientRect.left) / 2, (clientRect.bottom - clientRect.top) / 2};
				POINT screenCenter = center;

				ClientToScreen(winApp_->GetHwnd(), &screenCenter);
				SetCursorPos(screenCenter.x, screenCenter.y);

				mouseX_ = center.x;
				mouseY_ = center.y;
			}
		}
	}
}

// 現在キーが押されているかを返す。
// 非前面時やImGuiがキーボードを使用する間は、ゲーム側へ入力を返さない。
bool Input::PushKey(BYTE keyNumber) {
	if (!IsForegroundWindow(winApp_) || IsImGuiCapturingKeyboard()) {
		return false;
	}

	if (key[keyNumber]) {

		return true;
	}

	return false;
}

// 前フレームでは押されておらず、現在押されている場合にtrueを返す。
bool Input::TriggerKey(BYTE keyNumber) {
	if (!IsForegroundWindow(winApp_) || IsImGuiCapturingKeyboard()) {
		return false;
	}

	if (key[keyNumber] && !preKey[keyNumber]) {

		return true;
	}

	return false;
}

// 前フレームでは押されており、現在離されている場合にtrueを返す。
bool Input::ReleaseKey(BYTE keyNumber) {
	if (!IsForegroundWindow(winApp_) || IsImGuiCapturingKeyboard()) {
		return false;
	}

	if (!key[keyNumber] && preKey[keyNumber]) {

		return true;
	}

	return false;
}

// ゲームパッドのボタン押下状態を取得する。
// XInputが接続されていれば、DirectInputより優先する。
bool Input::PushButton(PadButton button) {
	if (isXInputConnected_) {
		return GetXInputButtonState(xInputState_, button);
	}

	if (!gamePadDevice_)
		return false;

	int index = static_cast<int>(button);

	if (index <= static_cast<int>(PadButton::kButtonRightThumb)) {
		// 通常ボタン
		return (padState_.rgbButtons[index] & 0x80) != 0;
	} else {
		// POV（十字キー）
		// DirectInput側では上下左右の角度と完全一致する場合のみ判定する。
		DWORD pov = padState_.rgdwPOV[0];

		if (pov == 0 && button == PadButton::kButtonUp)
			return true;
		if (pov == 9000 && button == PadButton::kButtonRight)
			return true;
		if (pov == 18000 && button == PadButton::kButtonDown)
			return true;
		if (pov == 27000 && button == PadButton::kButtonLeft)
			return true;
	}

	return false;
}

// ゲームパッドのボタンが押された瞬間を判定する。
bool Input::TriggerButton(PadButton button) {
	if (isXInputConnected_) {
		const bool now = GetXInputButtonState(xInputState_, button);
		const bool prev = GetXInputButtonState(preXInputState_, button);
		return now && !prev;
	}

	if (!gamePadDevice_)
		return false;

	int index = static_cast<int>(button);

	if (index <= static_cast<int>(PadButton::kButtonRightThumb)) {
		// 通常ボタン
		bool now = (padState_.rgbButtons[index] & 0x80) != 0;
		bool prev = (prePadState_.rgbButtons[index] & 0x80) != 0;
		return (now && !prev);
	} else {
		// POV（十字キー）
		DWORD povNow = padState_.rgdwPOV[0];
		DWORD povPrev = prePadState_.rgdwPOV[0];

		if (button == PadButton::kButtonUp)
			return (povNow == 0 && povPrev != 0);
		if (button == PadButton::kButtonRight)
			return (povNow == 9000 && povPrev != 9000);
		if (button == PadButton::kButtonDown)
			return (povNow == 18000 && povPrev != 18000);
		if (button == PadButton::kButtonLeft)
			return (povNow == 27000 && povPrev != 27000);
	}

	return false;
}

// ゲームパッドのボタンが離された瞬間を判定する。
bool Input::ReleaseButton(PadButton button) {
	if (isXInputConnected_) {
		const bool now = GetXInputButtonState(xInputState_, button);
		const bool prev = GetXInputButtonState(preXInputState_, button);
		return !now && prev;
	}

	if (!gamePadDevice_)
		return false;

	int index = static_cast<int>(button);

	if (index <= static_cast<int>(PadButton::kButtonRightThumb)) {
		// 通常ボタン
		bool now = (padState_.rgbButtons[index] & 0x80) != 0;
		bool prev = (prePadState_.rgbButtons[index] & 0x80) != 0;
		return (!now && prev);
	} else {
		// POV（十字キー）
		DWORD povNow = padState_.rgdwPOV[0];
		DWORD povPrev = prePadState_.rgdwPOV[0];

		if (button == PadButton::kButtonUp)
			return (povNow != 0 && povPrev == 0);
		if (button == PadButton::kButtonRight)
			return (povNow != 9000 && povPrev == 9000);
		if (button == PadButton::kButtonDown)
			return (povNow != 18000 && povPrev == 18000);
		if (button == PadButton::kButtonLeft)
			return (povNow != 27000 && povPrev == 27000);
	}

	return false;
}

// 左スティックのX軸を取得する。
float Input::GetJoyStickLX() const {
	if (isXInputConnected_) {
		return NormalizeXInputThumb(xInputState_.Gamepad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}

	if (!gamePadDevice_)
		return 0.0f;

	// DirectInputでは、中心32767を基準に値を変換する。
	float norm = (padState_.lX - 32767.0f) / 32767.0f;

	if (fabs(norm) < deadZone_)
		norm = 0.0f;

	return norm;
}

// 左スティックのY軸を取得する。上方向を正として返す。
float Input::GetJoyStickLY() const {
	if (isXInputConnected_) {
		return NormalizeXInputThumb(xInputState_.Gamepad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	}

	if (!gamePadDevice_)
		return 0.0f;

	float norm = (padState_.lY - 32767.0f) / 32767.0f;

	if (fabs(norm) < deadZone_)
		norm = 0.0f;

	// DirectInputは上が小さい値なので上下逆にしたいならマイナスをかける
	return -norm;
}

// 左スティックのX・Yをまとめて取得する。
Vector2 Input::GetJoyStickLXY() const { return Vector2(GetJoyStickLX(), GetJoyStickLY()); }

// 右スティックのX軸を取得する。
float Input::GetJoyStickRX() const {
	if (isXInputConnected_) {
		return NormalizeXInputThumb(xInputState_.Gamepad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}

	if (!gamePadDevice_)
		return 0.0f;

	float norm = (padState_.lRx - 32767.0f) / 32767.0f; // 右スティックX

	if (fabs(norm) < deadZone_)
		norm = 0.0f;

	return norm;
}

// 左スティックの角度を8方向へ分類し、指定された方向と比較する。
bool Input::IsJoyStickSelectDirectionL(JoyconStickDirection direction) const {
	const float x = GetJoyStickLX();
	const float y = GetJoyStickLY();

	// 入力がなければ、どの方向にも一致しない。
	if (x == 0.0f && y == 0.0f) {
		return false;
	}

	constexpr float kDegreesPerRadian = 180.0f / std::numbers::pi_v<float>;
	const float angle = std::atan2(y, x) * kDegreesPerRadian;

	// atan2の結果を、0以上360未満の角度へ揃える。
	const float normalizedAngle = angle < 0.0f ? angle + 360.0f : angle;

	// 右を0度とし、45度ごとの領域へ分類する。
	JoyconStickDirection selectedDirection = JoyconStickDirection::RIGHT;

	if (normalizedAngle < 22.5f || normalizedAngle >= 337.5f) {
		selectedDirection = JoyconStickDirection::RIGHT;
	} else if (normalizedAngle < 67.5f) {
		selectedDirection = JoyconStickDirection::UPRIGHT;
	} else if (normalizedAngle < 112.5f) {
		selectedDirection = JoyconStickDirection::UP;
	} else if (normalizedAngle < 157.5f) {
		selectedDirection = JoyconStickDirection::UPLEFT;
	} else if (normalizedAngle < 202.5f) {
		selectedDirection = JoyconStickDirection::LEFT;
	} else if (normalizedAngle < 247.5f) {
		selectedDirection = JoyconStickDirection::DOWNLEFT;
	} else if (normalizedAngle < 292.5f) {
		selectedDirection = JoyconStickDirection::DOWN;
	} else {
		selectedDirection = JoyconStickDirection::DOWNRIGHT;
	}

	return selectedDirection == direction;
}

// 右スティックのY軸を取得する。
// DirectInput側は、左スティックと異なり符号を反転していない。
float Input::GetJoyStickRY() const {
	if (isXInputConnected_) {
		return NormalizeXInputThumb(xInputState_.Gamepad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	}

	if (!gamePadDevice_)
		return 0.0f;

	float norm = (padState_.lRy - 32767.0f) / 32767.0f; // 右スティックY

	if (fabs(norm) < deadZone_)
		norm = 0.0f;

	return norm; // 上をプラスにしたいなら符号を反転
}

// 右スティックのX・Yをまとめて取得する。
Vector2 Input::GetJoyStickRXY() const { return Vector2(GetJoyStickRX(), GetJoyStickRY()); }

// 左トリガーの押し込み量を取得する。
float Input::GetLeftTrigger() const {
	if (isXInputConnected_) {
		return NormalizeXInputTrigger(xInputState_.Gamepad.bLeftTrigger);
	}

	if (!gamePadDevice_)
		return 0.0f;

	// 現在と前フレームのRzが両方0なら、共有Z軸として処理する。
	if (padState_.lRz == 0 && prePadState_.lRz == 0) {
		float left = 0.0f;
		float right = 0.0f;
		GetCombinedTriggerValues(padState_, leftTriggerButtonIndex_, rightTriggerButtonIndex_, left, right);
		return left;
	}

	// 独立したZ軸と、ボタン情報の大きい方を採用する。
	const float analog = Clamp01(static_cast<float>(padState_.lZ) / 65535.0f);
	return std::max(analog, GetDigitalTrigger(padState_, leftTriggerButtonIndex_));
}

// 右スティックを8方向へ分類し、指定された方向と比較する。
// 判定にはGetJoyStickRYが返す符号をそのまま使用する。
bool Input::IsJoyStickSelectDirectionR(JoyconStickDirection direction) const {

	const float x = GetJoyStickRX();
	const float y = GetJoyStickRY();

	if (x == 0.0f && y == 0.0f) {
		return false;
	}

	constexpr float kDegreesPerRadian = 180.0f / std::numbers::pi_v<float>;
	const float angle = std::atan2(y, x) * kDegreesPerRadian;
	const float normalizedAngle = angle < 0.0f ? angle + 360.0f : angle;

	JoyconStickDirection selectedDirection = JoyconStickDirection::RIGHT;

	if (normalizedAngle < 22.5f || normalizedAngle >= 337.5f) {
		selectedDirection = JoyconStickDirection::RIGHT;
	} else if (normalizedAngle < 67.5f) {
		selectedDirection = JoyconStickDirection::UPRIGHT;
	} else if (normalizedAngle < 112.5f) {
		selectedDirection = JoyconStickDirection::UP;
	} else if (normalizedAngle < 157.5f) {
		selectedDirection = JoyconStickDirection::UPLEFT;
	} else if (normalizedAngle < 202.5f) {
		selectedDirection = JoyconStickDirection::LEFT;
	} else if (normalizedAngle < 247.5f) {
		selectedDirection = JoyconStickDirection::DOWNLEFT;
	} else if (normalizedAngle < 292.5f) {
		selectedDirection = JoyconStickDirection::DOWN;
	} else {
		selectedDirection = JoyconStickDirection::DOWNRIGHT;
	}

	return selectedDirection == direction;
}

// 右トリガーの押し込み量を取得する。
float Input::GetRightTrigger() const {
	if (isXInputConnected_) {
		return NormalizeXInputTrigger(xInputState_.Gamepad.bRightTrigger);
	}

	if (!gamePadDevice_)
		return 0.0f;

	if (padState_.lRz == 0 && prePadState_.lRz == 0) {
		float left = 0.0f;
		float right = 0.0f;
		GetCombinedTriggerValues(padState_, leftTriggerButtonIndex_, rightTriggerButtonIndex_, left, right);
		return right;
	}

	// 独立したRz軸と、ボタン情報の大きい方を採用する。
	const float analog = Clamp01(static_cast<float>(padState_.lRz) / 65535.0f);
	return std::max(analog, GetDigitalTrigger(padState_, rightTriggerButtonIndex_));
}

// トリガーが指定したしきい値以上まで押されているかを調べる。
bool Input::PushLeftTrigger(float threshold) const { return GetLeftTrigger() >= threshold; }

bool Input::PushRightTrigger(float threshold) const { return GetRightTrigger() >= threshold; }

// 左トリガーが、しきい値未満から以上へ変化した瞬間を判定する。
bool Input::TriggerLeftTrigger(float threshold) const {
	if (isXInputConnected_) {
		const float now = NormalizeXInputTrigger(xInputState_.Gamepad.bLeftTrigger);
		const float prev = NormalizeXInputTrigger(preXInputState_.Gamepad.bLeftTrigger);
		return now >= threshold && prev < threshold;
	}

	// 現在値と同じ方式で、前フレームの押し込み量を求める。
	float prevNorm = 0.0f;

	if (padState_.lRz == 0 && prePadState_.lRz == 0) {
		float prevLeft = 0.0f;
		float prevRight = 0.0f;
		GetCombinedTriggerValues(prePadState_, leftTriggerButtonIndex_, rightTriggerButtonIndex_, prevLeft, prevRight);
		prevNorm = prevLeft;
	} else {
		prevNorm = std::max(Clamp01(static_cast<float>(prePadState_.lZ) / 65535.0f), GetDigitalTrigger(prePadState_, leftTriggerButtonIndex_));
	}

	return GetLeftTrigger() >= threshold && prevNorm < threshold;
}

// 右トリガーが、しきい値未満から以上へ変化した瞬間を判定する。
bool Input::TriggerRightTrigger(float threshold) const {
	if (isXInputConnected_) {
		const float now = NormalizeXInputTrigger(xInputState_.Gamepad.bRightTrigger);
		const float prev = NormalizeXInputTrigger(preXInputState_.Gamepad.bRightTrigger);
		return now >= threshold && prev < threshold;
	}

	float prevNorm = 0.0f;

	if (padState_.lRz == 0 && prePadState_.lRz == 0) {
		float prevLeft = 0.0f;
		float prevRight = 0.0f;
		GetCombinedTriggerValues(prePadState_, leftTriggerButtonIndex_, rightTriggerButtonIndex_, prevLeft, prevRight);
		prevNorm = prevRight;
	} else {
		prevNorm = std::max(Clamp01(static_cast<float>(prePadState_.lRz) / 65535.0f), GetDigitalTrigger(prePadState_, rightTriggerButtonIndex_));
	}

	return GetRightTrigger() >= threshold && prevNorm < threshold;
}

// DirectInput側のスティック用デッドゾーンを、0～1に制限して保存する。
void Input::SetDeadZone(float deadZone) {
	if (deadZone < 0.0f)
		deadZone = 0.0f;
	if (deadZone > 1.0f)
		deadZone = 1.0f;

	deadZone_ = deadZone;
}

// クライアント座標のマウス位置を取得する。
// 位置の取得自体には、前面判定やImGuiの入力制御を適用しない。
float Input::GetMouseX() const { return static_cast<float>(mouseX_); }

float Input::GetMouseY() const { return static_cast<float>(mouseY_); }

// マウスの相対移動量を取得する。
// 非前面、クライアント領域外、ImGui操作中の場合は0を返す。
Vector2 Input::GetMouseMove() const {
	if (!IsForegroundWindow(winApp_) || !IsMouseInClientArea(winApp_, mouseX_, mouseY_) || IsImGuiCapturingMouse()) {
		return Vector2();
	}

	// DIMOUSESTATE2 の lX, lY は相対移動量
	return Vector2(static_cast<float>(mouseState_.lX), static_cast<float>(mouseState_.lY));
}

// マウスホイールの移動量を、そのまま取得する。
float Input::GetMouseWheelDelta() const {
	if (!IsForegroundWindow(winApp_) || !IsMouseInClientArea(winApp_, mouseX_, mouseY_) || IsImGuiCapturingMouse()) {
		return 0.0f;
	}
	return static_cast<float>(mouseState_.lZ);
}

// 指定したマウスボタンが押されているかを判定する。
bool Input::PushMouseButton(MouseButton button) const {
	if (!IsForegroundWindow(winApp_) || !IsMouseInClientArea(winApp_, mouseX_, mouseY_) || IsImGuiCapturingMouse()) {
		return false;
	}

	int index = static_cast<int>(button);
	if (index < 0 || index >= static_cast<int>(MouseButton::kMaxButtons)) {
		return false;
	}

	return (mouseState_.rgbButtons[index] & 0x80) != 0;
}

// 指定したマウスボタンが押された瞬間を判定する。
bool Input::TriggerMouseButton(MouseButton button) const {
	if (!IsForegroundWindow(winApp_) || !IsMouseInClientArea(winApp_, mouseX_, mouseY_) || IsImGuiCapturingMouse()) {
		return false;
	}

	int index = static_cast<int>(button);
	if (index < 0 || index >= static_cast<int>(MouseButton::kMaxButtons)) {
		return false;
	}

	bool now = (mouseState_.rgbButtons[index] & 0x80) != 0;
	bool prev = (prevMouseState_.rgbButtons[index] & 0x80) != 0;
	return (now && !prev);
}

// 指定したマウスボタンが離された瞬間を判定する。
bool Input::ReleaseMouseButton(MouseButton button) const {
	if (!IsForegroundWindow(winApp_) || !IsMouseInClientArea(winApp_, mouseX_, mouseY_) || IsImGuiCapturingMouse()) {
		return false;
	}

	int index = static_cast<int>(button);
	if (index < 0 || index >= static_cast<int>(MouseButton::kMaxButtons)) {
		return false;
	}

	bool now = (mouseState_.rgbButtons[index] & 0x80) != 0;
	bool prev = (prevMouseState_.rgbButtons[index] & 0x80) != 0;
	return (!now && prev);
}
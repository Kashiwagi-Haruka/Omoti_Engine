#include "PlayCommand.h"
#include "Input.h"

bool PlayCommand::GetMOVE_FRONT() {
	if (Input::GetInstance()->PushKey(DIK_W)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetMOVE_BACK() {
	if (Input::GetInstance()->PushKey(DIK_S)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetMOVE_LEFT() {
	if (Input::GetInstance()->PushKey(DIK_A)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetMOVE_RIGHT() {
	if (Input::GetInstance()->PushKey(DIK_D)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetJUMP() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetDASH() {
	if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kRight)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetNORMAL_ATTACK_PUSH() {
	if (Input::GetInstance()->PushMouseButton(Input::MouseButton::kLeft) ||Input::GetInstance()->PushButton(Input::PadButton::kButtonB)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetNORMAL_ATTACK_TRIGGER() {
	if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonB)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetNORMAL_ATTACK_RELEASE() {
	if (Input::GetInstance()->ReleaseMouseButton(Input::MouseButton::kLeft) || Input::GetInstance()->ReleaseButton(Input::PadButton::kButtonB)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetSKILL_ATTACK() {
	if (Input::GetInstance()->TriggerKey(DIK_E) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonY)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetSPECIAL_ATTACK() {
	if (Input::GetInstance()->TriggerKey(DIK_Q) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonX)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetDESIDE() {
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetESCAPE() {
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		return true;
	}
	return false;
}
bool PlayCommand::GetCURSOR_DISPLAY(){ 
	if (Input::GetInstance()->PushKey(DIK_LALT) || Input::GetInstance()->PushKey(DIK_RALT)) {
		return true;
	}
	return false;

}
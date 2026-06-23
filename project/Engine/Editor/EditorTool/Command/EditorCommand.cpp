#include "EditorCommand.h"
#include "Input.h"

bool EditorCommand::GetUndo(){ 
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) || Input::GetInstance()->PushKey(DIK_RCONTROL)) {
		if (Input::GetInstance()->TriggerKey(DIK_Z)) {
			return true;
		}
	}
	return false;
}

bool EditorCommand::GetRedo() {
	if (Input::GetInstance()->PushKey(DIK_LCONTROL) || Input::GetInstance()->PushKey(DIK_RCONTROL)) {
		if (Input::GetInstance()->TriggerKey(DIK_Y)) {
			return true;
		}
	}
	return false;
}
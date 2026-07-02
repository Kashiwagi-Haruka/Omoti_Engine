#include "EditorCommand.h"
#include "Input.h"

namespace {
bool IsControlPressed(Input* input) { return input && (input->PushKey(DIK_LCONTROL) || input->PushKey(DIK_RCONTROL)); }

bool IsShiftPressed(Input* input) { return input && (input->PushKey(DIK_LSHIFT) || input->PushKey(DIK_RSHIFT)); }
} // namespace

bool EditorCommand::GetUndo() {
	Input* input = Input::GetInstance();
	if (IsControlPressed(input) && !IsShiftPressed(input) && input->TriggerKey(DIK_Z)) {
		return true;
	}
	return false;
}

bool EditorCommand::GetRedo() {
	Input* input = Input::GetInstance();
	if (IsControlPressed(input) && (input->TriggerKey(DIK_Y) || (IsShiftPressed(input) && input->TriggerKey(DIK_Z)))) {
		return true;
	}
	return false;
}
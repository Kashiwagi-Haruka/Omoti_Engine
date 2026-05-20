#include "PlayCommand.h"
#include "Input.h"
bool PlayCommand::GetInput(Command& command) {
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		command = Command::DESIDE;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		command = Command::JUMP;
		return true;
	}
	if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kRight)) {
		command = Command::DASH;
		return true;
	}
	if (Input::GetInstance()->TriggerMouseButton(Input::MouseButton::kLeft)) {
		command = Command::NORMAL_ATTACK;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_E)) {
		command = Command::SKILL_ATTACK;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_Q)) {
		command = Command::SPECIAL_ATTACK;
		return true;
	}
	return false;
}
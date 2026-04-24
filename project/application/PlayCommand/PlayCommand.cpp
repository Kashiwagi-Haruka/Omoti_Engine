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
	if (Input::GetInstance()->TriggerKey(DIK_LSHIFT)) {
		command = Command::DASH;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_Z)) {
		command = Command::NORMAL_ATTACK;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_X)) {
		command = Command::SKILL_ATTACK;
		return true;
	}
	if (Input::GetInstance()->TriggerKey(DIK_C)) {
		command = Command::SPECIAL_ATTACK;
		return true;
	}
	return false;
}
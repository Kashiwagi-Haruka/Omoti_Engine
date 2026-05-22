#define NOMINMAX
#include "Player.h"
#include "Camera.h"
#include "Function.h"
#include "Model/ModelManager.h"
#include <algorithm>
#include <numbers>
#include "Object3d/Object3dCommon.h"
#include "PlayCommand/PlayCommand.h"
#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

Player::Player() {

	ModelManager::GetInstance()->LoadModel("Resources/3d","FallingEffect");
	ModelManager::GetInstance()->LoadModel("Resources/3d","playerSkillUp");
	ModelManager::GetInstance()->LoadModel("Resources/3d","playerSkillUnder");
	models_ = std::make_unique<PlayerModels>();
	
}

Player::~Player() { 

}

void Player::Initialize(Camera* camera) {

	state_ = State::kIdle;
	velocity_ = {0.0f, 0.0f, 0.0f};
	transform_ = {
	    .scale{1.0f, 1.0f, 1.0f},
        .rotate{0.0f, 0.0f, 0.0f},
        .translate{-5.0f, 3.6f, -5.0f}
    };
	camera_ = camera;

	isAlive = true;
	parameters_ = SetInit();
	models_->SetCamera(camera_);
	models_->SetPlayerTransform(transform_);
	models_->Initialize();
	hp_ = parameters_.hpMax_;
	bulletVelocity_ = {0, 0, 0};
	isDash = false;
	isJump = false;
	isfalling = false;
	isLevelUP = false;
	rotateTimer = 0.1f;
	damageTrigger_ = false;
	attack_ = std::make_unique<PlayerAttack>();
	attack_->SetCamera(camera_);
	attack_->SetTransform(transform_);
	attack_->SetModels(models_.get());
	attack_->Initialize();

}
void Player::SetCharacterType(const std::string& characterName) {
	if (models_) {
		models_->SetCharacterType(characterName);
	}
}
Attribute Player::GetCurrentAttribute() const { return models_ ? models_->GetCurrentAttribute() : Attribute::None; }
void Player::Move() {

	// ★ 落下攻撃中は移動できない
	if (!attack_->IsCanMove()) {
		return;
	}

	// 入力方向を記録する変数
	Vector3 inputDirection = {0.0f, 0.0f, 0.0f};
	Vector3 inputAxis = {0.0f, 0.0f, 0.0f};
	bool hasInput = false;

	if (PlayCommand::GetMOVE_LEFT() || PlayCommand::GetMOVE_RIGHT()) {

		if (!PlayCommand::GetMOVE_RIGHT()) {

			if (PlayCommand::GetMOVE_LEFT()) {

				inputAxis.x = -1.0f;
				hasInput = true;
				if (!attack_->IsAttacking()) {
					models_->SetStateM(PlayerModels::StateM::walk);
				}
			}
		}

		if (!PlayCommand::GetMOVE_LEFT()) {

			if (PlayCommand::GetMOVE_RIGHT()) {
				inputAxis.x = 1.0f;
				hasInput = true;
				if (!attack_->IsAttacking()) {
					models_->SetStateM(PlayerModels::StateM::walk);
				}
			}
		}
	}
	if (PlayCommand::GetMOVE_FRONT() || PlayCommand::GetMOVE_BACK()) {

		if (!PlayCommand::GetMOVE_FRONT()) {

			if (PlayCommand::GetMOVE_BACK()) {
				if (!attack_->IsAttacking()) {
					models_->SetStateM(PlayerModels::StateM::walk);
				}
				inputAxis.z = -1.0f;
				hasInput = true;
			}
		}

		if (!PlayCommand::GetMOVE_BACK()) {

			if (PlayCommand::GetMOVE_FRONT()) {
				inputAxis.z = 1.0f;
				hasInput = true;
				if (!attack_->IsAttacking()) {
					models_->SetStateM(PlayerModels::StateM::walk);
				}
			}
		}
	}

	// 入力がある場合、その方向に向きを回転
	if (hasInput) {
		const float inputLength = std::sqrt(inputAxis.x * inputAxis.x + inputAxis.z * inputAxis.z);
		if (inputLength > 0.0f) {
			inputAxis.x /= inputLength;
			inputAxis.z /= inputLength;
		}

		const float cameraYaw = camera_ ? camera_->GetRotate().y : 0.0f;
		const Vector3 forward = {std::sinf(cameraYaw), 0.0f, std::cosf(cameraYaw)};
		const Vector3 right = {std::cosf(cameraYaw), 0.0f, -std::sinf(cameraYaw)};
		inputDirection = forward * inputAxis.z + right * inputAxis.x;

		const float accel = parameters_.accelationRate * (parameters_.SpeedUp + 1);
		float currentSpeed = velocity_.x * inputDirection.x + velocity_.z * inputDirection.z;
		if (currentSpeed < 0.0f) {
			currentSpeed = 0.0f;
		}
		const float nextSpeed = std::min(parameters_.accelationMax, currentSpeed + accel);
		velocity_.x = inputDirection.x * nextSpeed;
		velocity_.z = inputDirection.z * nextSpeed;
		// 入力方向から目標角度を計算
		float targetAngle = std::atan2(inputDirection.x, inputDirection.z);

		// 現在の角度と目標角度の差を計算
		float angleDiff = targetAngle - transform_.rotate.y;

		// 角度を-π〜πの範囲に正規化
		while (angleDiff > std::numbers::pi_v<float>) {
			angleDiff -= 2.0f * std::numbers::pi_v<float>;
		}
		while (angleDiff < -std::numbers::pi_v<float>) {
			angleDiff += 2.0f * std::numbers::pi_v<float>;
		}

		// 滑らかに回転
		transform_.rotate.y = Function::Lerp(transform_.rotate.y, transform_.rotate.y + angleDiff, rotateTimer);
	}

	if (!PlayCommand::GetMOVE_LEFT() && !PlayCommand::GetMOVE_RIGHT() && !PlayCommand::GetMOVE_FRONT() && !PlayCommand::GetMOVE_BACK()) {
		isDash = false;
		if (!attack_->IsAttacking() && !attack_->isSkillAttacking()) {
		models_->SetStateM(PlayerModels::StateM::idle);
		}
	} else {
		isDash = PlayCommand::GetDASH();
	}

	if (PlayCommand::GetJUMP()) {
		if (!isfalling && !isJump) {
			isJump = true;
		}
	}

	if (!PlayCommand::GetMOVE_LEFT() && !PlayCommand::GetMOVE_RIGHT()) {
		velocity_.x *= (1.0f - parameters_.decelerationRate);
		if (velocity_.x > -0.01f && velocity_.x < 0.01f) {
			velocity_.x = 0.0f;
		}
	}
	if (!PlayCommand::GetMOVE_FRONT() && !PlayCommand::GetMOVE_BACK()) {
		velocity_.z *= (1.0f - parameters_.decelerationRate);
		if (velocity_.z > -0.01f && velocity_.z < 0.01f) {
			velocity_.z = 0.0f;
		}
	}

	velocity_.x = std::clamp(velocity_.x, -parameters_.accelationMax, parameters_.accelationMax);
	velocity_.z = std::clamp(velocity_.z, -parameters_.accelationMax, parameters_.accelationMax);
	if (isDash) {
		velocity_.x *= parameters_.dashMagnification;
		velocity_.z *= parameters_.dashMagnification;
	}

	if (PlayCommand::GetMOVE_LEFT()) {
		bulletVelocity_.x = -1;
	}
	if (PlayCommand::GetMOVE_RIGHT()) {
		bulletVelocity_.x = 1;
	}
}

void Player::Jump() {

	if (isJump) {
		velocity_.y = parameters_.jumpPower;

		if (jumpTimer >= parameters_.jumpTimerMax) {
			jumpTimer = 0.0f;
			isJump = false;
			isfalling = true;
		} else {
			jumpTimer += 0.1f * (1 / 60.0f);
		}
	}

	// ★ 落下攻撃を開始したら即座に落下状態に
	if (attack_->IsFallingAttacking() && isJump) {
		isJump = false;
		isfalling = true;
		jumpTimer = 0.0f;
	}
}

void Player::Falling() {

	if (isfalling) {

		// ★ 落下攻撃中は急降下
		if (attack_->IsFallingAttacking()) {
			velocity_.y -= parameters_.gravity * 3.0f; // 通常の3倍の速度で落下
			models_->SetStateM(PlayerModels::StateM::fallingAttack);
		} else {
			velocity_.y -= parameters_.gravity;
			models_->SetStateM(PlayerModels::StateM::idle);
		}

		if (transform_.translate.y <= 3.6f) {
			transform_.translate.y = 3.6f;
			velocity_.y = 0.0f;
			isfalling = false;

			// ★ 落下攻撃が地面に着いたら終了
			if (attack_->IsFallingAttacking()) {
				models_->SetStateM(PlayerModels::StateM::idle);
				attack_->EndAttack();
			}

			usedAirAttack = false;
		}
	}
}

void Player::Update() {
	attack_->Update();
	Move();
	Jump();
	Falling();

	// --- レベルアップ処理 ---
	if (parameters_.Level < parameters_.MaxLevel) {

		if (parameters_.EXP >= parameters_.MaxEXP) {

			parameters_.EXP -= parameters_.MaxEXP;
			parameters_.Level++;

			isLevelUP = true;
		}
	} else {
		if (parameters_.EXP > parameters_.MaxEXP) {
			parameters_.EXP = parameters_.MaxEXP;
		}
	}


	transform_.translate += velocity_;
	{
		const float radius = movementLimitRadius_;
		const float dx = transform_.translate.x - movementLimitCenter_.x;
		const float dz = transform_.translate.z - movementLimitCenter_.z;
		const float distSquared = dx * dx + dz * dz;
		if (distSquared > radius * radius) {
			const float dist = std::sqrt(distSquared);
			const float scale = radius / dist;
			transform_.translate.x = movementLimitCenter_.x + dx * scale;
			transform_.translate.z = movementLimitCenter_.z + dz * scale;
		}
	}

	models_->SetCamera(camera_);
	models_->SetPlayerTransform(transform_);
	models_->Update();

#ifdef USE_IMGUI

	//if (ImGui::Begin("Player")) {

	//	ImGui::DragInt("HP", &hp_);
	//	ImGui::DragFloat3("plPos", &transform_.translate.x);
	//	ImGui::DragFloat3("plRot", &transform_.rotate.x);
	//	ImGui::DragFloat("rotateSpeed", &rotateTimer, 0.01f, 0.0f, 1.0f);
	//	ImGui::DragFloat("comboWindow", &comboWindow_, 0.01f, 0.1f, 2.0f);
	//	ImGui::DragFloat("heavyAttackThreshold", &heavyAttackThreshold_, 0.01f, 0.1f, 2.0f);
	//	ImGui::Text("LMB = FIRE , WASD = MOVE , SPACE = JUMP");
	//	ImGui::Text("isDash: %d", isDash);
	//	ImGui::Text("Combo Step: %d / 4", comboStep_);
	//	ImGui::Text("Combo Timer: %.2f", comboTimer_);
	//	ImGui::Text("Can Combo: %s", canCombo_ ? "YES" : "NO");
	//	ImGui::Text("Hold Timer: %.2f", attackHoldTimer_);
	//	ImGui::Text("Falling Attack: %s", isFallingAttack_ ? "YES" : "NO");
	//}
	//ImGui::End();

#endif //

	parameters_.hpMax_ = parameters_.hpMax_ * (1 + parameters_.HPUp);

	// 死
	if (hp_ <= 0) {
		isAlive = false;
	}

	//  --- ダメージ後の無敵時間 ---
	if (isInvincible_) {
		invincibleTimer_ -= 1.0f / 60.0f;
		if (invincibleTimer_ <= 0.0f) {
			isInvincible_ = false;
		}
	}
}

void Player::EXPMath() { parameters_.EXP += 15; }

void Player::Draw() {

	models_->Draw();
	Object3dCommon::GetInstance()->DrawCommon();
	attack_->Draw();
}
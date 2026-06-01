#include "PlayerAttack.h"
#include "PlayCommand/PlayCommand.h"

PlayerAttack::PlayerAttack() {
	sword_ = std::make_unique<PlayerSword>();
	skill_ = std::make_unique<PlayerSkill>();
	special_ = std::make_unique<PlayerSpecialAttack>();
	key_ = std::make_unique<PlayerKey>();
}
PlayerAttack::~PlayerAttack() {
	Audio::GetInstance()->SoundUnload(&attackSE);
	Audio::GetInstance()->SoundUnload(&attackEndSE);
	Audio::GetInstance()->SoundUnload(&skillAttackSE);
};
void PlayerAttack::Initialize() {
	isAttacking_ = false;
	// コンボ関連の初期化
	comboStep_ = 0;
	comboTimer_ = 0.0f;
	comboWindow_ = 0.8f; // コンボ受付時間（秒）
	isAttacking_ = false;
	canCombo_ = false;
	nextComboInput_ = false;
	nextHeavyAttackInput_ = false;
	isfalling = false;
	isJump = false;

	// 重撃・落下攻撃関連
	attackHoldTimer_ = 0.0f;
	heavyAttackThreshold_ = 0.3f; // 長押し判定時間（秒）
	isFallingAttack_ = false;

	isSkillAttack = false;
	isSpecialAttack = false;

	attackSE = Audio::GetInstance()->SoundLoadFile("Resources/audio/SE/normalAttack.mp3");
	attackEndSE = Audio::GetInstance()->SoundLoadFile("Resources/audio/SE/endAttack.mp3");
	skillAttackSE = Audio::GetInstance()->SoundLoadFile("Resources/audio/SE/magic.mp3");

	sword_->Initialize();
	sword_->SetCamera(camera_);
	skill_->Initialize();
	skill_->SetCamera(camera_);
	special_->Initialize();
	special_->SetCamera(camera_);
	key_->Initialize();
	key_->SetCamera(camera_);
}
void PlayerAttack::SetAttackName(std::string AttackName) {
	// 攻撃名に応じた攻撃の初期化処理をここに記述
	// 例: if (AttackName == "NormalAttack") { /* 通常攻撃の初期化 */ }
}

void PlayerAttack::Update() {
	// 攻撃の更新処理をここに記述
	// ★ 落下攻撃中は他の攻撃不可
	if (isFallingAttack_) {
		return;
	}
	if (isSkillAttack) {
		models_->SetStateM(PlayerModels::StateM::skillAttack);
	}
	if (isSpecialAttack) {
		models_->SetStateM(PlayerModels::StateM::idle);
	}

	if (!isSkillAttack && !isSpecialAttack) {
	// コンボタイマーの更新
	if (comboTimer_ > 0.0f) {
		comboTimer_ -= 1.0f / 60.0f;
		if (comboTimer_ <= 0.0f) {
			// タイムアウト: コンボリセット
			comboStep_ = 0;
			canCombo_ = false;
		}
	}

	// ===== 左クリック長押し判定 =====
	if (PlayCommand::GetNORMAL_ATTACK_PUSH()) {
		attackHoldTimer_ += 1.0f / 60.0f;
	}

	// ===== 攻撃入力の処理 =====
	if (PlayCommand::GetNORMAL_ATTACK_TRIGGER()||(canCombo_&&nextComboInput_)) {

		// ★ 空中にいる場合は落下攻撃
		if (isfalling || isJump) {
			isFallingAttack_ = true;
			isAttacking_ = true;
			attackState_ = AttackState::kFallingAttack;
			sword_->StartAttack(5); // 5=落下攻撃

			// コンボリセット
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
			attackHoldTimer_ = 0.0f;
			return;
		}

		// 地上での通常攻撃
		if (!isAttacking_) {

			if (nextComboInput_&&canCombo_) {
				nextComboInput_ = false; // フラグをリセット
			}
			if (!canCombo_) {
				nextComboInput_ = true; // 自動的に次のコンボが始まるフラグを立てる
			}
			

			// 次のコンボ段階に進む
			comboStep_++;
			if (comboStep_ > 4) {
				comboStep_ = 1; // 4段階目の後は最初に戻る
			}

			// 攻撃状態の設定
			isAttacking_ = true;
			canCombo_ = false;          // 連打防止: 一旦コンボ不可に
			comboTimer_ = comboWindow_; // コンボ受付時間をリセット

			// 攻撃ステートの設定
			switch (comboStep_) {
			case 1:
				attackState_ = AttackState::kWeakAttack1;
				sword_->StartAttack(1); // 1段階目
				Audio::GetInstance()->SoundPlayWave(attackSE, false);
				models_->SetStateM(PlayerModels::StateM::attack1);
				break;
			case 2:
				attackState_ = AttackState::kWeakAttack2;
				sword_->StartAttack(2); // 2段階目
				Audio::GetInstance()->SoundPlayWave(attackSE, false);
				models_->SetStateM(PlayerModels::StateM::attack2);
				break;
			case 3:
				attackState_ = AttackState::kWeakAttack3;
				sword_->StartAttack(3); // 3段階目
				Audio::GetInstance()->SoundPlayWave(attackSE, false);
				models_->SetStateM(PlayerModels::StateM::attack3);
				break;
			case 4:
				attackState_ = AttackState::kWeakAttack4;
				sword_->StartAttack(4); // 4段階目（フィニッシュ）
				Audio::GetInstance()->SoundPlayWave(attackEndSE, false);
				models_->SetStateM(PlayerModels::StateM::attack4);
				break;
			}
		}
	}

	// ===== 左クリックを離したとき =====
	if (PlayCommand::GetNORMAL_ATTACK_RELEASE()) {

		// ★ 長押ししていた場合は重撃に変更
		if (attackHoldTimer_ >= heavyAttackThreshold_ && isAttacking_) {
			attackState_ = AttackState::kStrongAttack;
			sword_->StartAttack(6); // 6=重撃
			models_->SetStateM(PlayerModels::StateM::attack4);

			// コンボリセット
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
		}

		attackHoldTimer_ = 0.0f; // タイマーリセット
	}

	// 攻撃モーションが終了したか確認
	if (isAttacking_ && !isFallingAttack_ && models_->IsAttackAnimationFinished()) {
		isAttacking_ = false;
		canCombo_ = true; // 次のコンボ入力を受け付ける
		sword_->EndAttack();

		// 4段階目が終わったらコンボリセット
		if (comboStep_ >= 4) {
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
		}

		// 重撃が終わったらリセット
		if (attackState_ == AttackState::kStrongAttack) {
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
		}
	}
	}
	if (PlayCommand::GetSKILL_ATTACK()) {
		// スキル攻撃
		if (!isSkillAttack) {
			ResetNormalAttackState();
			isSkillAttack = true;
			attackState_ = AttackState::kSkillAttack;
			skill_->StartAttack(playerTransform_);
			Audio::GetInstance()->SoundPlayWave(skillAttackSE, false);
			// isSpecialAttack = true;
			// attackState_ = AttackState::kSpecialAttack;
			// skill_->StartSpecialAttack(playerTransform_, 6);

			// コンボリセット
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
		}
	}
	if (PlayCommand::GetSPECIAL_ATTACK()) {
		// 必殺技
		if (!isSpecialAttack) {
			ResetNormalAttackState();
			isSpecialAttack = true;
			attackState_ = AttackState::kSpecialAttack;
			special_->SetPlayerTransform(playerTransform_);
			special_->Start();

			Audio::GetInstance()->SoundPlayWave(skillAttackSE, false);

			// コンボリセット
			comboStep_ = 0;
			canCombo_ = false;
			comboTimer_ = 0.0f;
		}
	}
	key_->SetCamera(camera_);
	key_->SetPlayerTransform(playerTransform_);
	if (isSkillAttack) {
		key_->StartAnimation();
	}
	key_->Update();

	if (isSkillAttack) {
		skill_->SetCamera(camera_);
		skill_->Update();
		if (skill_->IsSkillEnd()) {
			isSkillAttack = false;
		}
	}

	if (isSpecialAttack) {
		special_->SetCamera(camera_);
		special_->Update();
		if (special_->IsEnd()) {
			isSpecialAttack = false;
		}
	}
	const auto swordJointMatrix = models_->GetJointWorldMatrix("剣");
	sword_->SetCamera(camera_);
	sword_->SetPlayerYaw(playerTransform_.rotate.y);
	sword_->Update(playerTransform_, swordJointMatrix);
}
void PlayerAttack::ResetNormalAttackState() {
	isAttacking_ = false;
	isFallingAttack_ = false;
	comboStep_ = 0;
	comboTimer_ = 0.0f;
	canCombo_ = false;
	nextComboInput_ = false;
	nextHeavyAttackInput_ = false;
	attackHoldTimer_ = 0.0f;
	sword_->EndAttack();
}
void PlayerAttack::EndAttack() {
	isAttacking_ = false;
	isFallingAttack_ = false;
	isSkillAttack = false;
	isSpecialAttack = false;
	sword_->EndAttack();
}
void PlayerAttack::Draw() {
	// 攻撃の描画処理をここに記述
	if (isSkillAttack) {
		skill_->Draw();
	}
	if (isSpecialAttack) {
		special_->Draw();
	}
	key_->Draw();
	sword_->Draw();
}
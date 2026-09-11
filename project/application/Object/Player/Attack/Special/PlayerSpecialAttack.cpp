#include "PlayerSpecialAttack.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <numbers>
PlayerSpecialAttack::PlayerSpecialAttack() {
	ModelManager::GetInstance()->LoadModel("Resources/3d", "iceFlower");
	sizuku_ = std::make_unique<SizukuSpecial>();
	mei_ = std::make_unique<MeiSpecial>();
}
void PlayerSpecialAttack::Initialize() {
	sizuku_->Initialize();
	mei_->Initialize();
}
void PlayerSpecialAttack::Start() {
	if (useMei_) {
		mei_->SetMeiHeight(transform_.scale.y);
		mei_->SetMeiTransform(transform_);
		mei_->Start();
		return;
	}
	sizuku_->SetSizukuHeight(transform_.scale.y);
	sizuku_->SetSizukuTransform(transform_);
	sizuku_->Start();
}
void PlayerSpecialAttack::End() { useMei_ ? mei_->End() : sizuku_->End(); }
void PlayerSpecialAttack::Update() {
	if (useMei_) {
		mei_->SetCamera(camera_);
		mei_->SetMeiHeight(transform_.scale.y);
		mei_->SetMeiTransform(transform_);
		mei_->Update();
		return;
	}
	sizuku_->SetCamera(camera_);
	sizuku_->SetSizukuHeight(transform_.scale.y);
	sizuku_->SetSizukuTransform(transform_);
	sizuku_->Update();
}
void PlayerSpecialAttack::SetPlayerTransform(const Transform& playerTransform) { transform_ = playerTransform; }

void PlayerSpecialAttack::Draw() { useMei_ ? mei_->Draw() : sizuku_->Draw(); }
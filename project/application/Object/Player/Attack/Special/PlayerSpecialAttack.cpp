#include "PlayerSpecialAttack.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <numbers>
PlayerSpecialAttack::PlayerSpecialAttack() { 
	ModelManager::GetInstance()->LoadModel("Resources/3d","iceFlower"); 
	sizuku_ = std::make_unique<SizukuSpecial>();
}
void PlayerSpecialAttack::Initialize() { sizuku_->Initialize(); }
void PlayerSpecialAttack::Start() { sizuku_->Start(); }
void PlayerSpecialAttack::End() { sizuku_->End(); }
void PlayerSpecialAttack::Update() { 
	sizuku_->SetCamera(camera_);
	sizuku_->SetSizukuHeight(transform_.scale.y);
	sizuku_->SetSizukuTransform(transform_);
	sizuku_->Update(); }
void PlayerSpecialAttack::SetPlayerTransform(const Transform& playerTransform) { transform_ = playerTransform; }

void PlayerSpecialAttack::Draw() { sizuku_->Draw(); }
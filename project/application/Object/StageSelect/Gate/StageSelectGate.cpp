#include "StageSelectGate.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

StageSelectGate::StageSelectGate() {
	gate_ = std::make_unique<Object3d>();
	Portal_ = std::make_unique<Primitive>();
}

void StageSelectGate::Initialize() {
	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");

	gate_->SetModel("gate");
	gate_->Initialize();

	Portal_->Initialize(Primitive::PrimitiveName::Plane, "Resources/3d/Rasen/Select/Portal.png");

	Portal_->SetEnableLighting(false);

	UpdateTransform();
}

void StageSelectGate::SetPosition(const Vector3& position) {
	position_ = position;
	UpdateTransform();
}

void StageSelectGate::UpdateTransform() {
	gate_->SetTranslate(position_);

	Portal_->SetTranslate(position_);
	Portal_->SetScale(portalScale_);

	portalAABB_.min = {position_.x - portalHalfSize_.x, position_.y - portalHalfSize_.y, position_.z - portalHalfSize_.z};

	portalAABB_.max = {position_.x + portalHalfSize_.x, position_.y + portalHalfSize_.y, position_.z + portalHalfSize_.z};
}

void StageSelectGate::Update() {
	gate_->SetCamera(camera_);
	Portal_->SetCamera(camera_);

	UpdateTransform();

	gate_->Update();
	Portal_->Update();
}

bool StageSelectGate::IsTouchingPlayer(const AABB& playerAABB) const { return RigidBody::isCollision(portalAABB_, playerAABB); }

void StageSelectGate::Draw() {
	gate_->Draw();

	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);

	Portal_->Draw();

	Object3dCommon::GetInstance()->DrawCommon();
}
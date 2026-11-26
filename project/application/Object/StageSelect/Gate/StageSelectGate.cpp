#include "StageSelectGate.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

StageSelectGate::StageSelectGate() {
	gate_ = std::make_unique<Object3d>();
	Portal_ = std::make_unique<Primitive>();
	StageNamePlate_ = std::make_unique<Primitive>();
}

void StageSelectGate::Initialize() {
	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");

	gate_->SetModel("gate");
	gate_->Initialize();

	Portal_->Initialize(Primitive::PrimitiveName::Plane, "Resources/3d/Rasen/Select/Portal.png");
	Portal_->SetEnableLighting(false);

	StageNamePlate_->Initialize(Primitive::PrimitiveName::Plane, "Resources/3d/Rasen/Select/" +stageName_+ ".png");
	StageNamePlate_->SetEnableLighting(false);

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

	StageNamePlate_->SetTranslate({position_.x, position_.y + 6.0f, position_.z - 1.5f});
	StageNamePlate_->SetScale({10.0f, 10.0f, 1.0f});

	portalAABB_ = RigidBody::MakeAABB(position_, portalHalfSize_);
}

void StageSelectGate::Update() {
	gate_->SetCamera(camera_);
	Portal_->SetCamera(camera_);
	StageNamePlate_->SetCamera(camera_);

	UpdateTransform();

	gate_->Update();
	Portal_->Update();
	StageNamePlate_->Update();
}

bool StageSelectGate::IsTouchingPlayer(const AABB& playerAABB) const { return RigidBody::isCollision(portalAABB_, playerAABB); }

void StageSelectGate::Draw() {
	gate_->Draw();

	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);

	Portal_->Draw();
	
	StageNamePlate_->Draw();

	Object3dCommon::GetInstance()->DrawCommon();
}
void StageSelectGate::SetStageName(const std::string& stageName) { stageName_ = stageName; }
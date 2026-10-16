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
}
void StageSelectGate::Update() { 
	gate_->SetCamera(camera_);
	Portal_->SetCamera(camera_);
	gate_->Update(); 
	Portal_->Update(); }
void StageSelectGate::Draw() { 
	gate_->Draw(); 
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);
	Portal_->Draw(); 
	Object3dCommon::GetInstance()->DrawCommon();
}
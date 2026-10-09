#include "StageSelectGate.h"
#include "Model/ModelManager.h"

StageSelectGate::StageSelectGate() { 
	gate_ = std::make_unique<Object3d>(); 
	Portal_ = std::make_unique<Primitive>();
}
void StageSelectGate::Initialize() {
	ModelManager::GetInstance()->LoadModel("Resources/3d/Rasen/Select/gate", "gate");
	gate_->SetModel("gate");
	gate_->Initialize();
	Portal_->Initialize(Primitive::PrimitiveName::Plane, "Resources/3d/Rasen/Select/Portal.png");
}
void StageSelectGate::Update() { 
	gate_->Update(); 
	Portal_->Update(); }
void StageSelectGate::Draw() { 
	gate_->Draw(); 
	Portal_->Draw(); }
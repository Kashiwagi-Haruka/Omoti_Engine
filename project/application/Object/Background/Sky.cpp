#include "Sky.h"
#include "Camera.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

Sky::Sky() {
	
	skyBox_ = std::make_unique<SkyBox>();
}

void Sky::Initialize(Camera* camera) {

	skyBox_->Initialize();
	skyBox_->SetDDSTexture("Resources/SkyBox/sky.dds");
	camera_ = camera;
	skyBox_->SetCamera(camera_);
	transform_.scale = {100.0f, 100.0f, 100.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.translate = {0.0f, 2.50f, 0.0f};
	skyBox_->SetScale(transform_.scale);
	skyBox_->SetRotation(transform_.rotate);
	skyBox_->Update();
}
void Sky::Update() {
	skyBox_->SetCamera(camera_);
	skyBox_->Update();
}
void Sky::Draw() { 
	Object3dCommon::GetInstance()->DrawCommonSkybox();
	
	skyBox_->Draw(); 
}
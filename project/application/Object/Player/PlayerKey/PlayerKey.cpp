#define NOMINMAX
#include "PlayerKey.h"
#include "Mesh/Model/ModelManager.h"
#include "Mesh/Object3d/Object3dCommon.h"
#include "Engine/math/Function.h"

namespace{
const float planeRotateAcceleration = 0.05f;
const float planeRotateMaxVelocity = 5.0f;
}


PlayerKey::PlayerKey() {
	/*keyObj_ = std::make_unique<Object3d>();*/
	keyHoleObj_ = std::make_unique<Primitive>();
	planeCircleObj_ = std::make_unique<Primitive>();
	/*ModelManager::GetInstance()->LoadModel("Resources/3d", "cube");*/
}
PlayerKey::~PlayerKey() = default;

void PlayerKey::Initialize() { 

	keyTransform_ = {
	    .scale = {0.5f, 0.5f, 0.5f},
          .rotate = {0.0f, 0.0f, 0.0f},
          .translate = {0.0f, 0.0f, -1.0f}
    };
	keyHoleTransform_ = {
		.scale = {0.5f, 0.5f, 0.5f},
		  .rotate = {0.0f, 0.0f, 0.0f},
		  .translate = {0.0f, 0.0f, -1.0f}
    };
	planeCircleTransform_ = {.scale = {0.1f, 0.1f, 1.0f},
		  .rotate = {0.0f, 0.0f, 0.0f},
		  .translate = {0.0f, 0.0f, -1.0f}
    };

	//keyObj_->Initialize();
	//keyObj_->SetModel("cube");

	keyHoleObj_->Initialize(Primitive::Plane);
	planeCircleObj_->Initialize(Primitive::Plane, "Resources/2d/KeyCircle.png");


}
void PlayerKey::Update() {

	switch (keyAnimation_) {
	case PlayerKey::KeyAnimation::Normal:
		if (isAninmationStarted_) {
			keyAnimation_ = KeyAnimation::Start;
			isAninmationStarted_ = false;
		}
		break;
	case PlayerKey::KeyAnimation::Start:
		planeRotateVelocity_ += planeRotateAcceleration;
		planeRotateVelocity_ = std::min(planeRotateVelocity_, planeRotateMaxVelocity);
		planeCircleTransform_.rotate.z += planeRotateVelocity_;


		if (planeRotateVelocity_ >= planeRotateMaxVelocity) {
			keyAnimation_ = KeyAnimation::Open;
		}
		break;
	case PlayerKey::KeyAnimation::Open:
		
		keyAnimation_ = KeyAnimation::Normal;

		break;
	default:
		break;
	}




	Matrix4x4 keyWorldMatrix = Function::MakeParentAffineMatrix(keyTransform_,playerTransform_);
	Matrix4x4 keyHoleWorldMatrix = Function::MakeParentAffineMatrix(keyHoleTransform_, playerTransform_);
	Matrix4x4 planeWorldMatrix = Function::MakeParentAffineMatrix(planeCircleTransform_, playerTransform_);
	/*keyObj_->SetWorldMatrix(keyWorldMatrix);*/
	keyHoleObj_->SetWorldMatrix(keyHoleWorldMatrix);
	planeCircleObj_->SetWorldMatrix(planeWorldMatrix);

	/*keyObj_->Update();*/
	keyHoleObj_->Update();
	planeCircleObj_->Update();
}

void PlayerKey::Draw() {
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
	/*keyObj_->Draw();*/
	Object3dCommon::GetInstance()->DrawCommonNoCull();
	keyHoleObj_->Draw();
	planeCircleObj_->Draw();
}
#define NOMINMAX
#include "PlayerKey.h"
#include "Mesh/Model/ModelManager.h"
#include "Mesh/Object3d/Object3dCommon.h"
#include "Engine/math/Function.h"
#include "Engine/Texture/Data/Color.h"
#include <numbers>

namespace{
const float planeRotateAcceleration = 0.05f;
const float planeRotateMaxVelocity = 2.0f;
}


PlayerKey::PlayerKey() {
	keyObj_ = std::make_unique<Object3d>();
	keyHoleObj_ = std::make_unique<Primitive>();
	planeCircleObj_ = std::make_unique<Primitive>();
	ModelManager::GetInstance()->LoadModel("Resources/3d/Key", "Key");
}
PlayerKey::~PlayerKey() = default;

void PlayerKey::Initialize() { 

	keyTransform_ = {
	    .scale = {0.2f, 0.2f, 0.2f},
          .rotate = {std::numbers::pi_v<float>/2.0f, std::numbers::pi_v<float>, 0.0f },
          .translate = {0.0f, 0.0f, -2.3f}
    };
	keyHoleTransform_ = {
		.scale = {1.0f, 1.0f, 0.5f},
		  .rotate = {0.0f, 0.0f, 0.0f},
		  .translate = {0.0f, 0.0f, -1.0f}
    };
	planeCircleTransform_ = {.scale = {2.0f, 2.0f, 1.0f},
		  .rotate = {0.0f, 0.0f, 0.0f},
		  .translate = {0.0f, 0.0f, -1.0f}
    };

	keyObj_->Initialize();
	keyObj_->SetModel("Key");

	keyHoleObj_->Initialize(Primitive::Plane,"Resources/2d/KeyPrimitive/KeyHole.png");
	planeCircleObj_->Initialize(Primitive::Plane, "Resources/2d/KeyPrimitive/KeyCircle.png");

}
void PlayerKey::SetCamera(Camera* camera) {
	camera_ = camera;
	if (keyObj_) {
		keyObj_->SetCamera(camera_);
	}
	if (keyHoleObj_) {
		keyHoleObj_->SetCamera(camera_);
	}
	if (planeCircleObj_) {
		planeCircleObj_->SetCamera(camera_);
	}
}
void PlayerKey::Update() {

	switch (keyAnimation_) {
	case PlayerKey::KeyAnimation::Normal:
		if (isAninmationStarted_) {
			keyAnimation_ = KeyAnimation::Start;
			isAninmationStarted_ = false;
			planeRotateVelocity_ = 0.0f;
			planeCircleTransform_.rotate.z = 0.0f;
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
	keyObj_->SetWorldMatrix(keyWorldMatrix);
	keyHoleObj_->SetWorldMatrix(keyHoleWorldMatrix);
	planeCircleObj_->SetWorldMatrix(planeWorldMatrix);

	keyObj_->Update();
	keyHoleObj_->Update();
	planeCircleObj_->Update();
}
void PlayerKey::AttibuteColorSetting() {
	switch (attribute_) {
	case Attribute::None:
		planeCircleObj_->SetColor(Color::RGBAToVector4(255,255,255,255));
		break;
	case Attribute::Fire:
		planeCircleObj_->SetColor(Color::RGBAToVector4(240, 40, 40, 255));
		break;
	case Attribute::Ice:
		planeCircleObj_->SetColor(Color::RGBAToVector4(130, 210, 240, 255));
		break;
	case Attribute::Wind:
		planeCircleObj_->SetColor(Color::RGBAToVector4(100, 210, 100, 255));
		break;
	case Attribute::Thunder:
		planeCircleObj_->SetColor(Color::RGBAToVector4(200, 110, 210, 255));
		break;
	case Attribute::Imaginary:
		planeCircleObj_->SetColor(Color::RGBAToVector4(240, 225, 70, 255));
		break;
	case Attribute::Quantum:
		planeCircleObj_->SetColor(Color::RGBAToVector4(85, 85, 220, 255));
		break;
	default:
		break;
	}
}
void PlayerKey::Draw() {
	Object3dCommon::GetInstance()->DrawCommon();
	keyObj_->Draw();
	Object3dCommon::GetInstance()->DrawCommonNoCull();
	keyHoleObj_->Draw();
	planeCircleObj_->Draw();
}
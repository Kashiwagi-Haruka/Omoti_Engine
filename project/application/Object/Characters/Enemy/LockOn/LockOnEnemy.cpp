#include "LockOnEnemy.h"
#include "Camera.h"
#include "Function.h"

void LockOnEnemy::Initialize() {
	lockOnPrimitive_ = std::make_unique<Primitive>();
	lockOnPrimitive_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/LockOn.png");
	lockOnPrimitive_->SetEnableLighting(false);
	lockOnPrimitive_->SetCamera(camera_);
	ApplyBillboardTransform();
}

void LockOnEnemy::Update() {
	if (!lockOnPrimitive_) {
		return;
	}
	lockOnPrimitive_->SetCamera(camera_);
	ApplyBillboardTransform();
	lockOnPrimitive_->UpdateCameraMatrices();
}

void LockOnEnemy::Draw() {
	if (!isActive_ || !lockOnPrimitive_) {
		return;
	}
	lockOnPrimitive_->Draw();
}

void LockOnEnemy::SetPosition(const Vector3& position) { position_ = position; }

void LockOnEnemy::SetCamera(Camera* camera) {
	camera_ = camera;
	if (!lockOnPrimitive_) {
		return;
	}
	lockOnPrimitive_->SetCamera(camera_);
	ApplyBillboardTransform();
	lockOnPrimitive_->UpdateCameraMatrices();
}

void LockOnEnemy::SetActive(bool isActive) { isActive_ = isActive; }

void LockOnEnemy::ApplyBillboardTransform() {
	if (!lockOnPrimitive_) {
		return;
	}

	Vector3 translate = position_;
	translate.y += verticalOffset_;
	const Vector3 scale = {size_, size_, 1.0f};

	if (!camera_) {
		lockOnPrimitive_->SetTransform({
		    scale, {0.0f, 0.0f, 0.0f},
             translate
        });
		return;
	}

	Matrix4x4 billboardMatrix = Function::Inverse(camera_->GetViewMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	const Matrix4x4 scaleMatrix = Function::MakeScaleMatrix(scale);
	const Matrix4x4 translateMatrix = Function::MakeTranslateMatrix(translate);
	const Matrix4x4 worldMatrix = Function::Multiply(Function::Multiply(scaleMatrix, billboardMatrix), translateMatrix);
	lockOnPrimitive_->SetWorldMatrix(worldMatrix);
}
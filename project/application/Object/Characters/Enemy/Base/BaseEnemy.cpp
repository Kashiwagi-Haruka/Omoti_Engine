#include "BaseEnemy.h"
#include "Camera.h"
#include "Function.h"
#include "Object3d/Object3d.h"

BaseEnemy::BaseEnemy() { object_ = std::make_unique<Object3d>(); }

BaseEnemy::~BaseEnemy() = default;

void BaseEnemy::Initialize(Camera* camera, const Vector3& translate) {
	camera_ = camera;
	isAlive_ = true;
	transform_ = {
	    .scale{1.0f, 1.0f, 1.0f},
	    .rotate{0.0f, 0.0f, 0.0f},
	    .translate = translate,
	};

	object_->Initialize();
	object_->SetModel(modelName_);
	object_->SetCamera(camera_);
	object_->SetTransform(transform_);
	object_->Update();
}

void BaseEnemy::Update() {
	if (!isAlive_) {
		return;
	}

	transform_.translate += velocity_;
	object_->SetCamera(camera_);
	object_->SetTransform(transform_);
	object_->Update();
}

void BaseEnemy::Draw() {
	if (!isAlive_) {
		return;
	}

	object_->Draw();
}

void BaseEnemy::SetCamera(Camera* camera) {
	camera_ = camera;
	if (object_) {
		object_->SetCamera(camera_);
	}
}

void BaseEnemy::SetPosition(const Vector3& position) {
	transform_.translate = position;
	if (object_) {
		object_->SetTransform(transform_);
	}
}

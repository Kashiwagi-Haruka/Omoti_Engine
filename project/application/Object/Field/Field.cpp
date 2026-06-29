#include "Field.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

Field::Field() {
	
	field_ = std::make_unique<Primitive>();
}

Field::~Field() {}

void Field::Initialize(Camera* camera) {

	camera_ = camera;

	transform_ = {
	    .scale{100.0f,  1.0f, 100.0f },
        .rotate{0.0f,   0.0f,  0.0f},
        .translate{0.0f, 0.0f, 0.0f}
    };

	field_->Initialize(Primitive::Cylinder, "Resources/2d/field/map.png");
	field_->SetTransform(transform_);
	field_->SetCamera(camera_);
	field_->SetEnvironmentCoefficient(0.2f);
}
void Field::Update() { 
	field_->SetCamera(camera_);
	field_->Update();
}

void Field::Draw() { 	
	Object3dCommon::GetInstance()->DrawCommonNoCull();
	field_->Draw();
}

#define NOMINMAX
#include "SizukuSpecial.h"
#include "Object3d/Object3dCommon.h"
#include "Model/ModelManager.h"

SizukuSpecial::SizukuSpecial(){ 
	fieldPlane_ = std::make_unique<Primitive>(); 
	ModelManager::GetInstance()->LoadModel("Resources/3d/iceFlower", "iceFlower");

}
void SizukuSpecial::Initialize() { 
	fieldPlane_->Initialize(Primitive::Plane);
}
void SizukuSpecial::Start() { 
	isStarted_ = true; 
	duration_ = 0; 
	rainTimer_ = 0; 
	iceRains_.clear(); 
	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_ * 0.5f;
}
void SizukuSpecial::End() { 
	isStarted_ = false; 
	fieldPlaneTransform_.scale = {0.0f, 0.0f, 1.0f};
	iceRains_.clear(); 
}
void SizukuSpecial::Update() { 
	if (isStarted_) {
		float fieldsize = fieldPlaneTransform_.scale.x;
		fieldsize += 0.02f;
		fieldPlaneTransform_.scale.x = std::min(fieldsize, fieldSize_);
		fieldPlaneTransform_.scale.y = fieldPlaneTransform_.scale.x;
		if (fieldPlaneTransform_.scale.x >= fieldSize_) {
			duration_ += 1.0f / 60.0f;
			rainTimer_ += 1.0f / 60.0f;
			if (duration_ >= durationMax_) {
				End();
			}

		}
	}


}
void SizukuSpecial::Draw() {
	if (!isStarted_) {
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon();
	if (fieldPlane_) {
		fieldPlane_->Draw();
	}
	//for (const auto& iceRain : iceRains_) {
	//	if (iceRain) {
	//		iceRain->Draw();
	//	}
	//}
}
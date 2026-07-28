#define NOMINMAX
#include "SizukuSpecial.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <numbers>

SizukuSpecial::SizukuSpecial() {
	fieldPlane_ = std::make_unique<Primitive>();
	skydomeObj_ = std::make_unique<Object3d>();
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/skydome", "sizukuSpecialDome");
}
void SizukuSpecial::Initialize() {
	fieldPlane_->Initialize(Primitive::Plane, "Resources/2d/Effect/sizukuField.png");
	fieldPlane_->SetEnableLighting(false);
	fieldPlaneTransform_.rotate.x = std::numbers::pi_v<float>/2.0f;
	skydomeObj_->Initialize();
	skydomeObj_->SetEnableLighting(false);
	skydomeObj_->SetModel("sizukuSpecialDome");
	skydomeTransform_ = {
	    {50.0f, 50.0f, 50.0f},
        {0.0f, 0.0f,  0.0f },
        {0.0f, 0.0f,  0.0f }
    };
	skydomeObj_->SetTransform(skydomeTransform_);
	animationTime_ = 0.0f;
}
void SizukuSpecial::Start() {
	isStarted_ = true;
	isEnd_ = false;
	duration_ = 0;
	rainTimer_ = 0;
	animationTime_ = 0.0f;
	iceRains_.clear();
	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_;
	skydomeTransform_.translate = sizukuTransform_.translate;
}
void SizukuSpecial::End() {
	isStarted_ = false;
	isEnd_ = true;
	fieldPlaneTransform_.scale = {0.0f, 0.0f, 1.0f};
	iceRains_.clear();
}
void SizukuSpecial::Update() {
	if (isStarted_) {
		fieldPlaneTransform_.translate = sizukuTransform_.translate;
		fieldPlaneTransform_.translate.y -= sizukuHeight_;
		animationTime_ = std::min(animationTime_ + GameBase::GetInstance()->GetDeltaTime(), animationTimeMax_);
		float fieldsize = fieldPlaneTransform_.scale.x;
		fieldsize += 2.0f;
		fieldPlaneTransform_.scale.x = std::min(fieldsize, fieldSize_);
		fieldPlaneTransform_.scale.y = fieldPlaneTransform_.scale.x;
		if (fieldPlaneTransform_.scale.x >= fieldSize_) {
			duration_ += 1.0f / 60.0f;
			rainTimer_ += 1.0f / 60.0f;
			if (duration_ >= durationMax_) {
				End();
			}
		}
		fieldPlane_->SetCamera(camera_);
		fieldPlane_->SetTransform(fieldPlaneTransform_);
		fieldPlane_->Update();
		skydomeTransform_.translate = sizukuTransform_.translate;
		skydomeObj_->SetCamera(camera_);
		skydomeObj_->SetTransform(skydomeTransform_);
		skydomeObj_->Update();
	}
}
void SizukuSpecial::Draw() {
	if (!isStarted_) {
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	/*Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);*/
	if (animationTime_ < animationTimeMax_) {
		if (fieldPlane_) {
			fieldPlane_->Draw();
		}
	}
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
	if (animationTime_ < animationTimeMax_) {
		skydomeObj_->Draw();
	}
	// for (const auto& iceRain : iceRains_) {
	//	if (iceRain) {
	//		iceRain->Draw();
	//	}
	// }
}
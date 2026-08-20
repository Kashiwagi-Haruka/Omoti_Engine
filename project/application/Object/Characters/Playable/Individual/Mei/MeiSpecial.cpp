#define NOMINMAX
#include "MeiSpecial.h"

#include "GameBase.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <numbers>

MeiSpecial::MeiSpecial() : fieldPlane_(std::make_unique<Primitive>()) {}

void MeiSpecial::Initialize() {
	fieldPlane_->Initialize(Primitive::Plane, "Resources/2d/Effect/MeiField.png");
	fieldPlane_->SetEnableLighting(false);
	fieldTransform_.rotate.x = std::numbers::pi_v<float> / 2.0f;
}

void MeiSpecial::Start() {
	isStarted_ = true;
	isEnd_ = false;
	elapsedTime_ = 0.0f;
	--damageId_;
	fieldTransform_.scale = {};
	fieldTransform_.translate = meiTransform_.translate;
	fieldTransform_.translate.y -= meiHeight_;
}

void MeiSpecial::End() {
	isStarted_ = false;
	isEnd_ = true;
	fieldTransform_.scale = {};
}

void MeiSpecial::Update() {
	if (!isStarted_) {
		return;
	}
	elapsedTime_ += GameBase::GetInstance()->GetDeltaTime();
	const float grow = std::clamp(elapsedTime_ / kGrowDuration_, 0.0f, 1.0f);
	fieldTransform_.scale = {kFieldRadius_ * grow, kFieldRadius_ * grow, 1.0f};
	fieldPlane_->SetCamera(camera_);
	fieldPlane_->SetTransform(fieldTransform_);
	fieldPlane_->Update();
	if (elapsedTime_ >= kDuration_) {
		End();
	}
}

void MeiSpecial::Draw() {
	if (!isStarted_) {
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	fieldPlane_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
}
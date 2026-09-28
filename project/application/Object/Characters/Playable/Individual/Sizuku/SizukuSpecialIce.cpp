#define NOMINMAX
#include "SizukuSpecialIce.h"
#include "Object3d/Object3dCommon.h"
#include "SizukuSpecial.h"
#include <algorithm>
#include <random>

void SizukuSpecial::StartIceSpecial() {
	// 元のフィールド、氷花、氷雨の処理は氷属性だけで使用する。
	animationTimeMax_ = 8.0f;
	fieldPlaneTransform_.scale = {};
	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_;
	skydomeTransform_.translate = sizukuTransform_.translate;
	iceFlowerTransform_.scale = {};
	iceFlowerTransform_.translate = fieldPlaneTransform_.translate;
	iceRainTransforms_.resize(iceRains_.size());
	for (size_t i = 0; i < iceRains_.size(); ++i) {
		ResetIceRain(i, true);
	}
	damagePosition_ = iceFlowerTransform_.translate;
	damageScale_ = {9.0f, 3.0f, 9.0f};
}

void SizukuSpecial::ResetIceRain(size_t index, bool randomizeHeight) {
	std::uniform_real_distribution<float> offset(-18.0f, 18.0f);
	std::uniform_real_distribution<float> height(0.0f, 14.0f);
	auto& transform = iceRainTransforms_[index];
	transform.scale = {0.45f, 0.8f, 0.45f};
	transform.rotate = {};
	transform.translate = {
	    sizukuTransform_.translate.x + offset(randomEngine_), fieldPlaneTransform_.translate.y + 12.0f + (randomizeHeight ? height(randomEngine_) : 14.0f),
	    sizukuTransform_.translate.z + offset(randomEngine_)};
}

void SizukuSpecial::UpdateIceSpecial(float deltaTime) {
	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_;
	fieldPlaneTransform_.scale.x = std::min(fieldPlaneTransform_.scale.x + 30.0f * deltaTime, 50.0f);
	fieldPlaneTransform_.scale.y = fieldPlaneTransform_.scale.x;
	skydomeTransform_.translate = sizukuTransform_.translate;

	if (elapsedTime_ >= 3.0f) {
		const float grow = std::clamp((elapsedTime_ - 3.0f) / 0.65f, 0.0f, 1.0f);
		iceFlowerTransform_.scale = {grow * 3.0f, grow * 3.0f, grow * 3.0f};
		iceFlowerTransform_.translate = fieldPlaneTransform_.translate;
		iceFlowerTransform_.translate.y -= (1.0f - grow) * 4.0f;
		damagePosition_ = iceFlowerTransform_.translate;
	}

	fieldPlane_->SetCamera(camera_);
	fieldPlane_->SetTransform(fieldPlaneTransform_);
	fieldPlane_->Update();
	skydomeObj_->SetCamera(camera_);
	skydomeObj_->SetTransform(skydomeTransform_);
	skydomeObj_->Update();
	iceFlower_->SetCamera(camera_);
	iceFlower_->SetTransform(iceFlowerTransform_);
	iceFlower_->Update();

	if (elapsedTime_ >= 5.0f) {
		for (size_t i = 0; i < iceRains_.size(); ++i) {
			iceRainTransforms_[i].translate.y -= 18.0f * deltaTime;
			if (iceRainTransforms_[i].translate.y <= fieldPlaneTransform_.translate.y)
				ResetIceRain(i, false);
			iceRains_[i]->SetCamera(camera_);
			iceRains_[i]->SetTransform(iceRainTransforms_[i]);
			iceRains_[i]->Update();
		}
	}
}

void SizukuSpecial::DrawIceSpecial() {
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	fieldPlane_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
	skydomeObj_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	if (elapsedTime_ >= 3.0f)
		iceFlower_->Draw();
	if (elapsedTime_ >= 5.0f)
		for (const auto& rain : iceRains_)
			rain->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
}

void SizukuSpecialIce::Start(SizukuSpecial& special) { special.StartIceSpecial(); }

void SizukuSpecialIce::Update(SizukuSpecial& special, float deltaTime) { special.UpdateIceSpecial(deltaTime); }
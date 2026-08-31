#define NOMINMAX
#include "SizukuSpecial.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <cmath>
#include <numbers>

namespace {
constexpr size_t kRainDropCount = 24;
}

SizukuSpecial::SizukuSpecial() {
	fieldPlane_ = std::make_unique<Primitive>();
	skydomeObj_ = std::make_unique<Object3d>();
	iceFlower_ = std::make_unique<Object3d>();
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/flower", "sizukuSpecial");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/skydome", "sizukuSpecialDome");
}

void SizukuSpecial::Initialize() {
	fieldPlane_->Initialize(Primitive::Plane, "Resources/2d/Effect/sizukuField.png");
	fieldPlane_->SetEnableLighting(false);
	fieldPlaneTransform_.rotate.x = std::numbers::pi_v<float> / 2.0f;
	skydomeObj_->Initialize();
	skydomeObj_->SetEnableLighting(false);
	skydomeObj_->SetModel("sizukuSpecialDome");
	skydomeTransform_.scale = {50.0f, 50.0f, 50.0f};

	iceFlower_->Initialize();
	iceFlower_->SetModel("sizukuSpecial");
	iceFlowerTransform_.scale = {};

	iceRains_.clear();
	iceRainTransforms_.resize(kRainDropCount);
	for (size_t i = 0; i < kRainDropCount; ++i) {
		auto rain = std::make_unique<Object3d>();
		rain->Initialize();
		rain->SetModel("sizukuSpecialRain");
		iceRains_.push_back(std::move(rain));
	}
	animationTime_ = 0.0f;
}

void SizukuSpecial::Start() {
	isStarted_ = true;
	isEnd_ = false;
	elapsedTime_ = 0.0f;
	animationTime_ = 0.0f;
	
	damageId_ -= 2;
	rainDamageId_ = damageId_ + 1;
	fieldPlaneTransform_.scale = {};
	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_;
	skydomeTransform_.translate = sizukuTransform_.translate;
	iceFlowerTransform_.scale = {};
	iceFlowerTransform_.translate = fieldPlaneTransform_.translate;
	for (size_t i = 0; i < iceRains_.size(); ++i) {
		ResetRainDrop(i, true);
	}
}

void SizukuSpecial::End() {
	isStarted_ = false;
	isEnd_ = true;
	fieldPlaneTransform_.scale = {};
	iceFlowerTransform_.scale = {};
}

void SizukuSpecial::ResetRainDrop(size_t index, bool randomizeHeight) {
	std::uniform_real_distribution<float> offset(-rainRadius_, rainRadius_);
	std::uniform_real_distribution<float> height(0.0f, 14.0f);
	auto& transform = iceRainTransforms_[index];
	transform.scale = {0.45f, 0.8f, 0.45f};
	transform.rotate = {0.0f, 0.0f, 0.0f};
	transform.translate = {
	    sizukuTransform_.translate.x + offset(randomEngine_), fieldPlaneTransform_.translate.y + 12.0f + (randomizeHeight ? height(randomEngine_) : 14.0f),
	    sizukuTransform_.translate.z + offset(randomEngine_)};
}

void SizukuSpecial::Update() {
	if (!isStarted_) {
		return;
	}
	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();
	elapsedTime_ += deltaTime;
	animationTime_ = std::min(elapsedTime_, animationTimeMax_);

	fieldPlaneTransform_.translate = sizukuTransform_.translate;
	fieldPlaneTransform_.translate.y -= sizukuHeight_;
	fieldPlaneTransform_.scale.x = std::min(fieldPlaneTransform_.scale.x + 30.0f * deltaTime, fieldSize_);
	fieldPlaneTransform_.scale.y = fieldPlaneTransform_.scale.x;
	skydomeTransform_.translate = sizukuTransform_.translate;

	if (elapsedTime_ >= attackStartTime_) {
		const float grow = std::clamp((elapsedTime_ - attackStartTime_) / flowerGrowTime_, 0.0f, 1.0f);
		// Begin below the floor so the flower visibly grows out of the ground during ATTACK.
		iceFlowerTransform_.scale = {grow * 3.0f, grow * 3.0f, grow * 3.0f};
		iceFlowerTransform_.translate = fieldPlaneTransform_.translate;
		iceFlowerTransform_.translate.y -= (1.0f - grow) * 4.0f;
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

	if (elapsedTime_ >= animationTimeMax_) {
		for (size_t i = 0; i < iceRains_.size(); ++i) {
			iceRainTransforms_[i].translate.y -= rainFallSpeed_ * deltaTime;
			if (iceRainTransforms_[i].translate.y <= fieldPlaneTransform_.translate.y) {
				ResetRainDrop(i, false);
			}
			iceRains_[i]->SetCamera(camera_);
			iceRains_[i]->SetTransform(iceRainTransforms_[i]);
			iceRains_[i]->Update();
		}
		if (elapsedTime_ >= animationTimeMax_ + rainDuration_) {
			End();
		}
	}
}

void SizukuSpecial::Draw() {
	if (!isStarted_) {
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	fieldPlane_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
	skydomeObj_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	if (elapsedTime_ >= attackStartTime_) {
		iceFlower_->Draw();
	}
	if (elapsedTime_ >= animationTimeMax_) {
		for (const auto& rain : iceRains_) {
			rain->Draw();
		}
	}
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
}
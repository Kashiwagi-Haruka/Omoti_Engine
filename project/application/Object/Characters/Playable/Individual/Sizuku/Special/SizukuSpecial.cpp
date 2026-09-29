#define NOMINMAX
#include "SizukuSpecial.h"
#include "GameBase.h"
#include "Model/ModelManager.h"
#include "ParticleManager.h"
#include "SizukuSpecialFire.h"
#include "SizukuSpecialIce.h"
#include "SizukuSpecialImaginary.h"
#include "SizukuSpecialQuantum.h"
#include "SizukuSpecialThunder.h"
#include "SizukuSpecialWind.h"
#include <algorithm>
#include <numbers>

namespace {
constexpr size_t kIceRainCount = 24;
constexpr const char* kParticleTexture = "Resources/2d/defaultParticle.png";
} // namespace

SizukuSpecial::SizukuSpecial()
    : fireSpecial_(std::make_unique<SizukuSpecialFire>()), iceSpecial_(std::make_unique<SizukuSpecialIce>()), windSpecial_(std::make_unique<SizukuSpecialWind>()),
      thunderSpecial_(std::make_unique<SizukuSpecialThunder>()), imaginarySpecial_(std::make_unique<SizukuSpecialImaginary>()), quantumSpecial_(std::make_unique<SizukuSpecialQuantum>()) {}
void SizukuSpecial::Initialize() {
	fireSpecial_->Initialize();
	iceSpecial_->Initialize();
	windSpecial_->Initialize();
	thunderSpecial_->Initialize();
	imaginarySpecial_->Initialize();
	quantumSpecial_->Initialize();
	animationTime_ = 0;
}
void SizukuSpecial::Start() {
	isStarted_ = true;
	isEnd_ = false;
	elapsedTime_ = animationTime_ = 0;
	damageId_ -= 2;
	rainDamageId_ = damageId_ + 1;
	switch (attribute_) {
	case Attribute::Fire:
		activeSpecial_ = fireSpecial_.get();
		break;
	case Attribute::Wind:
		activeSpecial_ = windSpecial_.get();
		break;
	case Attribute::Thunder:
		activeSpecial_ = thunderSpecial_.get();
		break;
	case Attribute::Imaginary:
		activeSpecial_ = imaginarySpecial_.get();
		break;
	case Attribute::Quantum:
		activeSpecial_ = quantumSpecial_.get();
		break;
	default:
		activeSpecial_ = iceSpecial_.get();
		break;
	}
	animationTimeMax_ = activeSpecial_->GetDuration();
	activeSpecial_->Start(Context());
}
void SizukuSpecial::Update() {
	if (!isStarted_)
		return;
	float dt = GameBase::GetInstance()->GetDeltaTime();
	elapsedTime_ += dt;
	animationTime_ = std::min(elapsedTime_, animationTimeMax_);
	activeSpecial_->Update(Context(), dt);
	if (elapsedTime_ >= animationTimeMax_)
		End();
}
void SizukuSpecial::End() {
	isStarted_ = false;
	isEnd_ = true;
}
bool SizukuSpecial::IsFlowerDamaging() const {
	if (!isStarted_)
		return false;
	switch (attribute_) {
	case Attribute::Fire:
		return elapsedTime_ >= 1 && elapsedTime_ < 1.35f;
	case Attribute::Wind:
		return elapsedTime_ >= .6f && elapsedTime_ < 4;
	case Attribute::Thunder:
		return elapsedTime_ >= .5f && elapsedTime_ < 2.1f;
	case Attribute::Imaginary:
		return elapsedTime_ >= 1.5f && elapsedTime_ < 1.9f;
	case Attribute::Quantum:
		return elapsedTime_ >= .9f && elapsedTime_ < 1.4f;
	default:
		return elapsedTime_ >= 3 && elapsedTime_ < 5;
	}
}
bool SizukuSpecial::IsRainDamaging() const { return isStarted_ && (attribute_ == Attribute::Ice || attribute_ == Attribute::None) && elapsedTime_ >= 5; }
void SizukuSpecial::Draw() {
	if (isStarted_)
		activeSpecial_->Draw();
}
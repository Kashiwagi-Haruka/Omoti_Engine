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

SizukuSpecial::SizukuSpecial() {
	fireSpecial_ = std::make_unique<SizukuSpecialFire>();
	iceSpecial_ = std::make_unique<SizukuSpecialIce>();
	windSpecial_ = std::make_unique<SizukuSpecialWind>();
	thunderSpecial_ = std::make_unique<SizukuSpecialThunder>();
	imaginarySpecial_ = std::make_unique<SizukuSpecialImaginary>();
	quantumSpecial_ = std::make_unique<SizukuSpecialQuantum>();
	fieldPlane_ = std::make_unique<Primitive>();
	thunderFieldPlane_ = std::make_unique<Primitive>();
	imaginaryFieldPlane_ = std::make_unique<Primitive>();
	skydomeObj_ = std::make_unique<Object3d>();
	iceFlower_ = std::make_unique<Object3d>();
	thunderProjectile_ = std::make_unique<Object3d>();
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/flower", "sizukuSpecial");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/skydome", "sizukuSpecialDome");
}

void SizukuSpecial::Initialize() {
	fieldPlane_->Initialize(Primitive::Plane, "Resources/2d/Effect/sizukuField.png");
	fieldPlane_->SetEnableLighting(false);
	fieldPlaneTransform_.rotate.x = std::numbers::pi_v<float> / 2.0f;
	thunderFieldPlane_->Initialize(Primitive::Plane, "Resources/3d/Character/Sizuku/Special/Thunder/sizukuSpecialThunder.png");
	thunderFieldPlane_->SetEnableLighting(false);
	thunderFieldPlaneTransform_.rotate.x = std::numbers::pi_v<float> / 2.0f;
	imaginaryFieldPlane_->Initialize(Primitive::Plane, "Resources/3d/Character/Sizuku/Special/Imaginary/sizukuImaginaryField.png");
	imaginaryFieldPlane_->SetEnableLighting(false);
	imaginaryFieldPlaneTransform_.rotate.x = std::numbers::pi_v<float> / 2.0f;

	skydomeObj_->Initialize();
	skydomeObj_->SetEnableLighting(false);
	skydomeObj_->SetModel("sizukuSpecialDome");
	skydomeTransform_.scale = {50.0f, 50.0f, 50.0f};

	iceFlower_->Initialize();
	iceFlower_->SetModel("sizukuSpecial");
	iceFlowerTransform_.scale = {};

	// 雷属性では雨モデルを一発だけ撃ち出す弾として利用する。
	thunderProjectile_->Initialize();
	thunderProjectile_->SetEnableLighting(false);
	thunderProjectile_->SetModel("sizukuSpecialRain");
	thunderProjectile_->SetColor({0.85f, 0.65f, 1.0f, 1.0f});

	iceRains_.clear();
	iceRainTransforms_.resize(kIceRainCount);
	for (size_t i = 0; i < kIceRainCount; ++i) {
		auto rain = std::make_unique<Object3d>();
		rain->Initialize();
		rain->SetModel("sizukuSpecialRain");
		iceRains_.push_back(std::move(rain));
	}

	// 氷以外は共通モデルを色替えせず、それぞれ専用のパーティクルで表現する。
	ParticleManager* particleManager = ParticleManager::GetInstance();
	particleManager->CreateParticleGroupIfMissing("sizukuSpecialMain", kParticleTexture);
	particleManager->CreateParticleGroupIfMissing("sizukuSpecialSub", kParticleTexture);
	mainEmitter_ = std::make_unique<ParticleEmitter>("sizukuSpecialMain");
	subEmitter_ = std::make_unique<ParticleEmitter>("sizukuSpecialSub");
	animationTime_ = 0.0f;
}

void SizukuSpecial::ConfigureEmitter(ParticleEmitter& emitter, const Vector4& color, uint32_t count, float speed, float life) {
	emitter.SetCount(count);
	emitter.SetFrequency(0.0f);
	emitter.SetAcceleration({0.0f, -0.35f, 0.0f});
	emitter.SetAreaMin({-0.35f, -0.35f, -0.35f});
	emitter.SetAreaMax({0.35f, 0.35f, 0.35f});
	emitter.SetEmissionAngle(std::numbers::pi_v<float> * 2.0f);
	emitter.SetEmissionSpeed(speed);
	emitter.SetLife(life);
	emitter.SetBeforeColor(color);
	emitter.SetAfterColor({color.x, color.y, color.z, 0.0f});
}

void SizukuSpecial::Start() {
	isStarted_ = true;
	isEnd_ = false;
	fireworkEmitted_ = false;
	attributeEffectEmitted_ = false;
	elapsedTime_ = 0.0f;
	animationTime_ = 0.0f;
	damageId_ -= 2;
	rainDamageId_ = damageId_ + 1;
	iceRainTransforms_.clear();

	// 現在属性に対応する、完全に独立した必殺技を開始する。
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
	case Attribute::Ice:
	case Attribute::None:
	case Attribute::MAXATTRIBUTE:
	default:
		activeSpecial_ = iceSpecial_.get();
		break;
	}
	activeSpecial_->Start(*this);
}

void SizukuSpecial::Update() {
	if (!isStarted_)
		return;
	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();
	elapsedTime_ += deltaTime;
	animationTime_ = std::min(elapsedTime_, animationTimeMax_);

	activeSpecial_->Update(*this, deltaTime);

	if (elapsedTime_ >= animationTimeMax_)
		End();
}

void SizukuSpecial::End() {
	isStarted_ = false;
	isEnd_ = true;
	fieldPlaneTransform_.scale = {};
	thunderFieldPlaneTransform_.scale = {};
	imaginaryFieldPlaneTransform_.scale = {};
	iceFlowerTransform_.scale = {};
}

bool SizukuSpecial::IsFlowerDamaging() const {
	if (!isStarted_)
		return false;
	switch (attribute_) {
	case Attribute::Fire:
		return elapsedTime_ >= 1.0f && elapsedTime_ < 1.35f;
	case Attribute::Wind:
		return elapsedTime_ >= 0.6f && elapsedTime_ < 4.0f;
	case Attribute::Thunder:
		return elapsedTime_ >= 0.5f && elapsedTime_ < 2.1f;
	case Attribute::Imaginary:
		return elapsedTime_ >= 1.5f && elapsedTime_ < 1.9f;
	case Attribute::Quantum:
		return elapsedTime_ >= 0.9f && elapsedTime_ < 1.4f;
	default:
		return elapsedTime_ >= 3.0f && elapsedTime_ < 5.0f;
	}
}

bool SizukuSpecial::IsRainDamaging() const {
	// 二段目の雨判定も元の氷属性だけに限定する。
	return isStarted_ && (attribute_ == Attribute::Ice || attribute_ == Attribute::None) && elapsedTime_ >= 5.0f;
}

void SizukuSpecial::DrawParticleSpecial() {
	mainEmitter_->Draw();
	if (attribute_ == Attribute::Fire || attribute_ == Attribute::Quantum)
		subEmitter_->Draw();
}

void SizukuSpecial::Draw() {
	if (!isStarted_)
		return;
	if (attribute_ == Attribute::Ice || attribute_ == Attribute::None || attribute_ == Attribute::MAXATTRIBUTE) {
		DrawIceSpecial();
		return;
	}
	if (attribute_ == Attribute::Thunder)
		DrawThunderSpecial();
	if (attribute_ == Attribute::Imaginary)
		DrawImaginarySpecial();
	DrawParticleSpecial();
}
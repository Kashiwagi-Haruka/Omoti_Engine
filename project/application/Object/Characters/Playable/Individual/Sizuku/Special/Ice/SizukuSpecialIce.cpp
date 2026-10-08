#include "SizukuSpecialIce.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include <algorithm>
#include <numbers>
void SizukuSpecialIce::Initialize() {
	auto* m = ModelManager::GetInstance();
	m->LoadModel("Resources/3d/Character/Sizuku/Special/flower", "sizukuSpecial");
	m->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	m->LoadModel("Resources/3d/Character/Sizuku/Special/skydome", "sizukuSpecialDome");
	field_ = std::make_unique<Primitive>();
	field_->Initialize(Primitive::Plane, "Resources/2d/Effect/sizukuField.png");
	field_->SetEnableLighting(false);
	fieldTransform_.rotate.x = std::numbers::pi_v<float> / 2;
	dome_ = std::make_unique<Object3d>();
	dome_->Initialize();
	dome_->SetEnableLighting(false);
	dome_->SetModel("sizukuSpecialDome");
	domeTransform_.scale = {50, 50, 50};
	flower_ = std::make_unique<Object3d>();
	flower_->Initialize();
	flower_->SetModel("sizukuSpecial");
	for (int i = 0; i < 24; i++) {
		auto rain = std::make_unique<Object3d>();
		rain->Initialize();
		rain->SetModel("sizukuSpecialRain");
		rains_.push_back(std::move(rain));
	}
}
void SizukuSpecialIce::Start(const SizukuSpecialContext& c) {
	player_ = c.transform;
	fieldTransform_.scale = {};
	fieldTransform_.translate = c.transform.translate;
	fieldTransform_.translate.y -= c.height;
	domeTransform_.translate = c.transform.translate;
	flowerTransform_.scale = {};
	flowerTransform_.translate = fieldTransform_.translate;
	rainTransforms_.resize(rains_.size());
	for (size_t i = 0; i < rains_.size(); i++)
		ResetRain(i, true);
	damagePosition_ = flowerTransform_.translate;
	damageScale_ = {9, 3, 9};
}
void SizukuSpecialIce::ResetRain(size_t i, bool randomHeight) {
	std::uniform_real_distribution<float> o(-18, 18), h(0, 14);
	auto& t = rainTransforms_[i];
	t.scale = {.45f, .8f, .45f};
	t.rotate = {};
	t.translate = {player_.translate.x + o(random_), fieldTransform_.translate.y + 12 + (randomHeight ? h(random_) : 14), player_.translate.z + o(random_)};
}
void SizukuSpecialIce::Update(const SizukuSpecialContext& c, float dt) {
	player_ = c.transform;
	fieldTransform_.translate = c.transform.translate;
	fieldTransform_.translate.y -= c.height;
	fieldTransform_.scale.x = std::min(fieldTransform_.scale.x + 30 * dt, 50.f);
	fieldTransform_.scale.y = fieldTransform_.scale.x;
	domeTransform_.translate = c.transform.translate;
	if (c.elapsedTime >= 3) {
		float g = std::clamp((c.elapsedTime - 3) / .65f, 0.f, 1.f);
		flowerTransform_.scale = {g * 3, g * 3, g * 3};
		flowerTransform_.translate = fieldTransform_.translate;
		flowerTransform_.translate.y -= (1 - g) * 4;
		damagePosition_ = flowerTransform_.translate;
	}
	field_->SetCamera(c.camera);
	field_->SetTransform(fieldTransform_);
	field_->Update();
	dome_->SetCamera(c.camera);
	dome_->SetTransform(domeTransform_);
	dome_->Update();
	flower_->SetCamera(c.camera);
	flower_->SetTransform(flowerTransform_);
	flower_->Update();
	if (c.elapsedTime >= 5)
		for (size_t i = 0; i < rains_.size(); i++) {
			rainTransforms_[i].translate.y -= 18 * dt;
			if (rainTransforms_[i].translate.y <= fieldTransform_.translate.y)
				ResetRain(i, false);
			rains_[i]->SetCamera(c.camera);
			rains_[i]->SetTransform(rainTransforms_[i]);
			rains_[i]->Update();
		}
}
void SizukuSpecialIce::Draw(Camera* camera) {
	field_->SetCamera(camera);
	field_->UpdateCameraMatrices();
	dome_->SetCamera(camera);
	dome_->UpdateCameraMatrices();
	flower_->SetCamera(camera);
	flower_->UpdateCameraMatrices();
	for (auto& r : rains_) {
		r->SetCamera(camera);
		r->UpdateCameraMatrices();
	}
	auto* c = Object3dCommon::GetInstance();
	c->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	c->SetBlendMode(BlendMode::kBlendModeAdd);
	field_->Draw();
	c->SetBlendMode(BlendMode::kBlendModeAlpha);
	c->DrawCommon();
	dome_->Draw();
	c->SetBlendMode(BlendMode::kBlendModeAdd);
	flower_->Draw();
	for (auto& r : rains_)
		r->Draw();
	c->SetBlendMode(BlendMode::kBlendModeAlpha);
}
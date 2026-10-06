#include "SizukuSpecialImaginary.h"
#include "Object3d/Object3dCommon.h"
#include "Object/Characters/Playable/Individual/Sizuku/Special/SizukuSpecialParticle.h"
#include <numbers>
void SizukuSpecialImaginary::Initialize() {
	field_ = std::make_unique<Primitive>();
	field_->Initialize(
	    Primitive::Plane, "Resources/3d/Character/Sizuku/Special/"
	                      "Imaginary/sizukuImaginaryField.png");
	field_->SetEnableLighting(false);
	fieldTransform_.rotate.x = std::numbers::pi_v<float> / 2;
	emitter_ = CreateSpecialEmitter("sizukuSpecialImaginary");
}
void SizukuSpecialImaginary::Start(const SizukuSpecialContext& c) {
	emitted_ = false;
	particleTransform_ = c.transform;
	particleTransform_.translate.y += 5;
	fieldTransform_.scale = {14, 14, 1};
	fieldTransform_.translate = c.transform.translate;
	fieldTransform_.translate.y -= c.height * 3;
	damagePosition_ = c.transform.translate;
	damageScale_ = {14, 6, 14};
	ConfigureSpecialEmitter(*emitter_, {1, .78f, .18f, 1}, 220, 4, 2.2f);
}
void SizukuSpecialImaginary::Update(const SizukuSpecialContext& c, float) {
	fieldTransform_.translate = c.transform.translate;
	fieldTransform_.translate.y -= c.height * 3;
	field_->SetCamera(c.camera);
	field_->SetTransform(fieldTransform_);
	field_->Update();
	emitter_->SetTransform(particleTransform_);
	if (!emitted_ && c.elapsedTime >= 1.5f) {
		emitter_->Emit();
		emitted_ = true;
	}
}
void SizukuSpecialImaginary::Draw(Camera* camera) {
	field_->SetCamera(camera);
	field_->UpdateCameraMatrices();
	auto* common = Object3dCommon::GetInstance();
	common->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	common->SetBlendMode(BlendMode::kBlendModeAdd);
	field_->Draw();
	common->SetBlendMode(BlendMode::kBlendModeAlpha);
	common->DrawCommon();
	emitter_->Draw();
}
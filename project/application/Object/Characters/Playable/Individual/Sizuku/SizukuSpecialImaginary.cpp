#define NOMINMAX
#include "SizukuSpecialImaginary.h"
#include "Object3d/Object3dCommon.h"
#include "SizukuSpecial.h"

void SizukuSpecial::StartImaginarySpecial() {
	// 虚数属性は頭上に光を集め、広範囲へ一度だけ解放する。
	animationTimeMax_ = 4.0f;
	particleTransform_ = sizukuTransform_;
	particleTransform_.translate.y += 5.0f;
	imaginaryFieldPlaneTransform_.scale = {14.0f, 14.0f, 1.0f};
	imaginaryFieldPlaneTransform_.translate = sizukuTransform_.translate;
	imaginaryFieldPlaneTransform_.translate.y -= sizukuHeight_*3;
	damagePosition_ = sizukuTransform_.translate;
	damageScale_ = {14.0f, 6.0f, 14.0f};
	ConfigureEmitter(*mainEmitter_, {1.0f, 0.78f, 0.18f, 1.0f}, 220, 4.0f, 2.2f);
}

void SizukuSpecial::UpdateImaginarySpecial(float) {
	imaginaryFieldPlaneTransform_.translate = sizukuTransform_.translate;
	imaginaryFieldPlaneTransform_.translate.y -= sizukuHeight_*3;
	imaginaryFieldPlane_->SetCamera(camera_);
	imaginaryFieldPlane_->SetTransform(imaginaryFieldPlaneTransform_);
	imaginaryFieldPlane_->Update();
	mainEmitter_->SetTransform(particleTransform_);
	if (!attributeEffectEmitted_ && elapsedTime_ >= 1.5f) {
		mainEmitter_->Emit();
		attributeEffectEmitted_ = true;
	}
}

void SizukuSpecial::DrawImaginarySpecial() {
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCullDepth);
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	imaginaryFieldPlane_->Draw();
	Object3dCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);
	Object3dCommon::GetInstance()->DrawCommon();
}

void SizukuSpecialImaginary::Start(SizukuSpecial& special) { special.StartImaginarySpecial(); }

void SizukuSpecialImaginary::Update(SizukuSpecial& special, float deltaTime) { special.UpdateImaginarySpecial(deltaTime); }
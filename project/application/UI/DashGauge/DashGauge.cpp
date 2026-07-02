#include "DashGauge.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <numbers>

namespace {
constexpr float kStartAngle = 220.0f * std::numbers::pi_v<float> / 180.0f;
constexpr float kEndAngle = 320.0f * std::numbers::pi_v<float> / 180.0f;
constexpr float kGaugeRotationAngle = 90.0f * std::numbers::pi_v<float> / 180.0f;
constexpr float kArcRadius = 165.0f;
constexpr Vector2 kSegmentSize{12.0f, 24.0f};
constexpr float kFramePadding = 2.0f;
constexpr Vector4 kFrameColor{0.08f, 0.12f, 0.18f, 0.75f};
constexpr Vector4 kFillColor{0.15f, 0.72f, 1.0f, 0.95f};
} // namespace

void DashGauge::Initialize() {
	position_ = {static_cast<float>(WinApp::kClientWidth) * 0.5f, static_cast<float>(WinApp::kClientHeight) * 0.5f};
	segmentSize_ = kSegmentSize;
	const uint32_t whiteTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");

	for (int i = 0; i < kSegmentCount; ++i) {
		frameSprites_[i] = std::make_unique<Sprite>();
		frameSprites_[i]->Initialize(whiteTextureHandle);
		frameSprites_[i]->SetAnchorPoint({0.5f, 0.5f});
		frameSprites_[i]->SetColor(kFrameColor);

		fillSprites_[i] = std::make_unique<Sprite>();
		fillSprites_[i]->Initialize(whiteTextureHandle);
		fillSprites_[i]->SetAnchorPoint({0.5f, 0.5f});
		fillSprites_[i]->SetColor(kFillColor);
	}

	Update();
}

void DashGauge::Update() {
	gaugeRate_ = std::clamp(gaugeRate_, 0.0f, 1.0f);
	const float fillSegmentCount = gaugeRate_ * static_cast<float>(kSegmentCount);

	for (int i = 0; i < kSegmentCount; ++i) {
		const float t = kSegmentCount <= 1 ? 0.0f : static_cast<float>(i) / static_cast<float>(kSegmentCount - 1);
		const float angle = kStartAngle + (kEndAngle - kStartAngle) * t + kGaugeRotationAngle;
		const Vector2 segmentPosition = {
		    position_.x + std::cos(angle) * kArcRadius,
		    position_.y + std::sin(angle) * kArcRadius,
		};
		const float tangentRotation = angle + (std::numbers::pi_v<float> * 0.5f);

		if (frameSprites_[i]) {
			frameSprites_[i]->SetPosition(segmentPosition);
			frameSprites_[i]->SetRotation(tangentRotation);
			frameSprites_[i]->SetScale({segmentSize_.x + kFramePadding, segmentSize_.y + kFramePadding});
			frameSprites_[i]->Update();
		}

		if (fillSprites_[i]) {
			const float segmentFill = std::clamp(fillSegmentCount - static_cast<float>(i), 0.0f, 1.0f);
			fillSprites_[i]->SetPosition(segmentPosition);
			fillSprites_[i]->SetRotation(tangentRotation);
			fillSprites_[i]->SetScale({segmentSize_.x * segmentFill, segmentSize_.y});
			fillSprites_[i]->SetColor({kFillColor.x, kFillColor.y, kFillColor.z, kFillColor.w * segmentFill});
			fillSprites_[i]->Update();
		}
	}
}

void DashGauge::Draw() {
	for (auto& frameSprite : frameSprites_) {
		if (frameSprite) {
			frameSprite->Draw();
		}
	}
	for (auto& fillSprite : fillSprites_) {
		if (fillSprite) {
			fillSprite->Draw();
		}
	}
}

void DashGauge::SetGaugeRate(float gaugeRate) { gaugeRate_ = gaugeRate; }
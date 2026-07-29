#define NOMINMAX
#include "AttackOperation.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "Sprite/Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"
#include <algorithm>

namespace {
constexpr Vector2 kOperationIconBaseSize{60.0f, 60.0f};
constexpr float kPressedScaleRate = 1.15f;
constexpr float kScaleReturnRate = 0.25f;
constexpr Vector2 kKeyboardDisplaySize{80.0f, 80.0f};
constexpr Vector2 kPadDisplaySize{60.0f, 60.0f};
constexpr float kKeyboardDisplayIconOffsetY = 50.0f;
} // namespace
AttackOperation::AttackOperation() {
	// スキルアイコンのテクスチャハンドルを取得
	dashSPData_.sprite = std::make_unique<Sprite>();
	jumpSPData_.sprite = std::make_unique<Sprite>();
	skillIconSPData_.sprite = std::make_unique<Sprite>();
	normalAttackSPData_.sprite = std::make_unique<Sprite>();
	specialAttackSPData_.sprite = std::make_unique<Sprite>();
	specialGaugeSPData_.sprite = std::make_unique<Sprite>();

	keyboardDashSPData_.sprite = std::make_unique<Sprite>();
	keyboardSkillIconSPData_.sprite = std::make_unique<Sprite>();
	keyboardJumpSPData_.sprite = std::make_unique<Sprite>();
	keyboardNormalAttackSPData_.sprite = std::make_unique<Sprite>();
	keyboardSpecialAttackSPData_.sprite = std::make_unique<Sprite>();

	padDashSPData_.sprite = std::make_unique<Sprite>();
	padSkillIconSPData_.sprite = std::make_unique<Sprite>();
	padJumpSPData_.sprite = std::make_unique<Sprite>();
	padNormalAttackSPData_.sprite = std::make_unique<Sprite>();
	padSpecialAttackSPData_.sprite = std::make_unique<Sprite>();
}
void AttackOperation::Initialize() {
	uint32_t dashHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/dash.png");
	dashSPData_.sprite->Initialize(dashHandle);
	dashSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(dashSPData_, kOperationIconBaseSize);
	// スキルアイコンの初期化
	uint32_t skillHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/skill.png");
	skillIconSPData_.sprite->Initialize(skillHandle);
	skillIconSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(skillIconSPData_, kOperationIconBaseSize);
	uint32_t jumpHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/jump.png");
	jumpSPData_.sprite->Initialize(jumpHandle);
	jumpSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(jumpSPData_, kOperationIconBaseSize);
	uint32_t normalAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/normalAttack.png");
	normalAttackSPData_.sprite->Initialize(normalAttackHandle);
	normalAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(normalAttackSPData_, kOperationIconBaseSize);
	uint32_t specialAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/special.png");
	specialAttackSPData_.sprite->Initialize(specialAttackHandle);
	specialAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(specialAttackSPData_, kOperationIconBaseSize);
	uint32_t specialGaugeAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/specialGauge.png");
	specialGaugeSPData_.sprite->Initialize(specialGaugeAttackHandle);
	specialGaugeSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(specialGaugeSPData_, kOperationIconBaseSize);

	uint32_t normalKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/normalAttackMouse.png");
	uint32_t dashKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/dashMouse.png");
	uint32_t jumpKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/jumpKey.png");
	uint32_t skillKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/skillKey.png");
	uint32_t specialKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/specialKey.png");

	keyboardDashSPData_.sprite->Initialize(dashKeyHandle);
	keyboardSkillIconSPData_.sprite->Initialize(skillKeyHandle);
	keyboardJumpSPData_.sprite->Initialize(jumpKeyHandle);
	keyboardNormalAttackSPData_.sprite->Initialize(normalKeyHandle);
	keyboardSpecialAttackSPData_.sprite->Initialize(specialKeyHandle);
	keyboardDashSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardSkillIconSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardJumpSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardNormalAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardSpecialAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});

	uint32_t normalPadHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Pad/X.png");
	uint32_t dashPadHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Pad/RT.png");
	uint32_t jumpPadHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Pad/A.png");
	uint32_t skillPadHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Pad/RB.png");
	uint32_t specialPadHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Pad/Y.png");
	padDashSPData_.sprite->Initialize(dashPadHandle);
	padSkillIconSPData_.sprite->Initialize(skillPadHandle);
	padJumpSPData_.sprite->Initialize(jumpPadHandle);
	padNormalAttackSPData_.sprite->Initialize(normalPadHandle);
	padSpecialAttackSPData_.sprite->Initialize(specialPadHandle);
	padDashSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	padSkillIconSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	padJumpSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	padNormalAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	padSpecialAttackSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
}

void AttackOperation::Update() {
	UpdateInputDisplayMode();

	skillIconSPData_.translate = {1200.0f, 400.0f};
	UpdateOperationSprite(skillIconSPData_, PlayCommand::GetSKILL_ATTACK());

	dashSPData_.translate = {skillIconSPData_.translate.x, skillIconSPData_.translate.y+skillIconSPData_.size.y/2.0f+80.0f};
	UpdateOperationSprite(dashSPData_, PlayCommand::GetDASH());


	specialAttackSPData_.translate = {dashSPData_.translate.x-80.0f, dashSPData_.translate.y + dashSPData_.size.y};
	UpdateOperationSprite(specialAttackSPData_, PlayCommand::GetSPECIAL_ATTACK());

	normalAttackSPData_.translate = {specialAttackSPData_.translate.x - specialAttackSPData_.size.x - 20.0f, specialAttackSPData_.translate.y + specialAttackSPData_.size.y/2.0f+20.0f};
	UpdateOperationSprite(normalAttackSPData_, PlayCommand::GetNORMAL_ATTACK_PUSH());
	jumpSPData_.translate = {specialAttackSPData_.translate.x, normalAttackSPData_.translate.y + normalAttackSPData_.size.y/2.0f + 20.0f};
	UpdateOperationSprite(jumpSPData_, PlayCommand::GetJUMP());



	UpdateControlGuideSprite(keyboardSkillIconSPData_, skillIconSPData_, kKeyboardDisplaySize);
	UpdateControlGuideSprite(keyboardJumpSPData_, jumpSPData_, kKeyboardDisplaySize);
	UpdateControlGuideSprite(keyboardNormalAttackSPData_, normalAttackSPData_, kKeyboardDisplaySize);
	UpdateControlGuideSprite(keyboardSpecialAttackSPData_, specialAttackSPData_, kKeyboardDisplaySize);
	UpdateControlGuideSprite(keyboardDashSPData_, dashSPData_, kKeyboardDisplaySize);
	UpdateControlGuideSprite(padSkillIconSPData_, skillIconSPData_, kPadDisplaySize);
	UpdateControlGuideSprite(padJumpSPData_, jumpSPData_, kPadDisplaySize);
	UpdateControlGuideSprite(padNormalAttackSPData_, normalAttackSPData_, kPadDisplaySize);
	UpdateControlGuideSprite(padSpecialAttackSPData_, specialAttackSPData_, kPadDisplaySize);
	UpdateControlGuideSprite(padDashSPData_, dashSPData_, kPadDisplaySize);
}
void AttackOperation::Draw() {
	// スキルアイコンを描画
	if (dashSPData_.sprite) {
		dashSPData_.sprite->Draw();
	}
	if (skillIconSPData_.sprite) {
		skillIconSPData_.sprite->Draw();
	}
	if (normalAttackSPData_.sprite) {
		normalAttackSPData_.sprite->Draw();
	}
	if (jumpSPData_.sprite) {
		jumpSPData_.sprite->Draw();
	}
	if (specialAttackSPData_.sprite) {
		specialAttackSPData_.sprite->Draw();
	}
	SpriteData* controlGuideSprites[] = {
	    inputDisplayMode_ == InputDisplayMode::Pad ? &padDashSPData_ : &keyboardDashSPData_,
	    inputDisplayMode_ == InputDisplayMode::Pad ? &padSkillIconSPData_ : &keyboardSkillIconSPData_,
	    inputDisplayMode_ == InputDisplayMode::Pad ? &padJumpSPData_ : &keyboardJumpSPData_,
	    inputDisplayMode_ == InputDisplayMode::Pad ? &padNormalAttackSPData_ : &keyboardNormalAttackSPData_,
	    inputDisplayMode_ == InputDisplayMode::Pad ? &padSpecialAttackSPData_ : &keyboardSpecialAttackSPData_,
	};
	for (SpriteData* spriteData : controlGuideSprites) {
		if (spriteData->sprite) {
			spriteData->sprite->Draw();
		}
	}
}
void AttackOperation::SetOperationSpriteBaseSize(SpriteData& spriteData, const Vector2& size) {
	spriteData.size = size;
	if (spriteData.sprite) {
		spriteData.sprite->SetScale(size);
	}
}

void AttackOperation::UpdateOperationSprite(SpriteData& spriteData, bool isPressed) {
	if (!spriteData.sprite) {
		return;
	}

	const Vector2 targetScale = isPressed ? Vector2{spriteData.size.x * kPressedScaleRate, spriteData.size.y * kPressedScaleRate} : spriteData.size;
	const Vector2 currentScale = spriteData.sprite->GetScale();
	const Vector2 nextScale{
	    currentScale.x + (targetScale.x - currentScale.x) * kScaleReturnRate,
	    currentScale.y + (targetScale.y - currentScale.y) * kScaleReturnRate,
	};

	spriteData.sprite->SetScale(nextScale);
	spriteData.sprite->SetPosition(spriteData.translate);
	spriteData.sprite->Update();
}

void AttackOperation::UpdateInputDisplayMode() {
	Input* input = Input::GetInstance();

	const bool isPadInput = input->PushButton(Input::PadButton::kButtonA) || input->PushButton(Input::PadButton::kButtonB) || input->PushButton(Input::PadButton::kButtonX) ||
	                        input->PushButton(Input::PadButton::kButtonY) || input->PushButton(Input::PadButton::kButtonLeftShoulder) || input->PushButton(Input::PadButton::kButtonRightShoulder) ||
	                        input->PushButton(Input::PadButton::kButtonBack) || input->PushButton(Input::PadButton::kButtonStart) || input->PushButton(Input::PadButton::kButtonLeftThumb) ||
	                        input->PushButton(Input::PadButton::kButtonRightThumb) || input->PushButton(Input::PadButton::kButtonUp) || input->PushButton(Input::PadButton::kButtonDown) ||
	                        input->PushButton(Input::PadButton::kButtonLeft) || input->PushButton(Input::PadButton::kButtonRight) || input->PushLeftTrigger() || input->PushRightTrigger();
	if (isPadInput) {
		inputDisplayMode_ = InputDisplayMode::Pad;
		return;
	}

	const bool isKeyboardInput = input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || input->PushKey(DIK_SPACE) || input->PushKey(DIK_E) ||
	                             input->PushKey(DIK_Q) || input->PushKey(DIK_ESCAPE) || input->PushKey(DIK_RETURN) || input->PushKey(DIK_1) || input->PushKey(DIK_2) || input->PushKey(DIK_3) ||
	                             input->PushKey(DIK_4) ||
	                             input->PushMouseButton(Input::MouseButton::kLeft) ||
	                             input->PushMouseButton(Input::MouseButton::kRight);
	if (isKeyboardInput) {
		inputDisplayMode_ = InputDisplayMode::Keyboard;
	}
}

void AttackOperation::UpdateControlGuideSprite(SpriteData& controlGuideSpriteData, const SpriteData& iconSpriteData, const Vector2& displaySize) {
	if (!controlGuideSpriteData.sprite) {
		return;
	}

	controlGuideSpriteData.size = displaySize;
	controlGuideSpriteData.translate = {
	    iconSpriteData.translate.x,
	    iconSpriteData.translate.y + kKeyboardDisplayIconOffsetY,
	};
	controlGuideSpriteData.sprite->SetPosition(controlGuideSpriteData.translate);
	controlGuideSpriteData.sprite->SetScale(controlGuideSpriteData.size);
	controlGuideSpriteData.sprite->Update();
}
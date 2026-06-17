#define NOMINMAX
#include "AttackOperation.h"
#include "PlayCommand/PlayCommand.h"
#include "Sprite/Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"

#include <algorithm>

namespace {
constexpr Vector2 kOperationIconBaseSize{90.0f, 90.0f};
constexpr float kPressedScaleRate = 1.15f;
constexpr float kScaleReturnRate = 0.25f;
constexpr Vector2 kKeyboardDisplaySize{100.0f, 40.0f};
constexpr float kKeyboardDisplayBottomMargin = 5.0f;
constexpr float kKeyboardDisplayIconOffsetY = 15.0f;
} // namespace
AttackOperation::AttackOperation() {
	// スキルアイコンのテクスチャハンドルを取得
	jumpSPData_.sprite = std::make_unique<Sprite>();
	skillIconSPData.sprite = std::make_unique<Sprite>();
	normalAttackSPData.sprite = std::make_unique<Sprite>();
	specialAttackSPData.sprite = std::make_unique<Sprite>();
	keyboardSkillIconSPData.sprite = std::make_unique<Sprite>();
	keyboardJumpSPData.sprite = std::make_unique<Sprite>();
	keyboardNormalAttackSPData.sprite = std::make_unique<Sprite>();
	keyboardSpecialAttackSPData.sprite = std::make_unique<Sprite>();
}
void AttackOperation::Initialize() {
	// スキルアイコンの初期化
	uint32_t skillHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/skill.png");
	skillIconSPData.sprite->Initialize(skillHandle);
	skillIconSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(skillIconSPData, kOperationIconBaseSize);
	uint32_t jumpHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/jump.png");
	jumpSPData_.sprite->Initialize(jumpHandle);
	jumpSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(jumpSPData_, kOperationIconBaseSize);
	uint32_t normalAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/NormalAttack.png");
	normalAttackSPData.sprite->Initialize(normalAttackHandle);
	normalAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(normalAttackSPData, kOperationIconBaseSize);
	uint32_t specialAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/special.png");
	specialAttackSPData.sprite->Initialize(specialAttackHandle);
	specialAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	SetOperationSpriteBaseSize(specialAttackSPData, kOperationIconBaseSize);
	uint32_t normalKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/normalAttackMouse.png");
	uint32_t dashKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/dashMouse.png");
	uint32_t jumpKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/jumpKey.png");
	uint32_t skillKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/skillKey.png");
	uint32_t specialKeyHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/Keyboard/specialKey.png");

	keyboardSkillIconSPData.sprite->Initialize(skillKeyHandle);
	keyboardJumpSPData.sprite->Initialize(jumpKeyHandle);
	keyboardNormalAttackSPData.sprite->Initialize(normalKeyHandle);
	keyboardSpecialAttackSPData.sprite->Initialize(specialKeyHandle);
	keyboardSkillIconSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardJumpSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardNormalAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	keyboardSpecialAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
}

void AttackOperation::Update() {
	skillIconSPData.translate = {1260.0f, 700.0f};
	UpdateOperationSprite(skillIconSPData, PlayCommand::GetSKILL_ATTACK());
	normalAttackSPData.translate = {skillIconSPData.translate.x - skillIconSPData.size.x - 20.0f, 700.0f};
	UpdateOperationSprite(normalAttackSPData, PlayCommand::GetNORMAL_ATTACK_PUSH());
	jumpSPData_.translate = {normalAttackSPData.translate.x - normalAttackSPData.size.x - 20.0f, 700.0f};
	UpdateOperationSprite(jumpSPData_, PlayCommand::GetJUMP());
	specialAttackSPData.translate = {skillIconSPData.translate.x, skillIconSPData.translate.y - skillIconSPData.size.y - 20.0f};
	UpdateOperationSprite(specialAttackSPData, PlayCommand::GetSPECIAL_ATTACK());

	auto updateKeyboardSprite = [](SpriteData& keyboardSpriteData, const SpriteData& iconSpriteData) {
		const float screenBottom = static_cast<float>(WinApp::kClientHeight) - kKeyboardDisplayBottomMargin;
		keyboardSpriteData.size = kKeyboardDisplaySize;
		keyboardSpriteData.translate = {
		    iconSpriteData.translate.x,
		    std::min(iconSpriteData.translate.y + kKeyboardDisplayIconOffsetY, screenBottom),
		};
		keyboardSpriteData.sprite->SetPosition(keyboardSpriteData.translate);
		keyboardSpriteData.sprite->SetScale(keyboardSpriteData.size);
		keyboardSpriteData.sprite->Update();
	};

	updateKeyboardSprite(keyboardSkillIconSPData, skillIconSPData);
	updateKeyboardSprite(keyboardJumpSPData, jumpSPData_);
	updateKeyboardSprite(keyboardNormalAttackSPData, normalAttackSPData);
	updateKeyboardSprite(keyboardSpecialAttackSPData, specialAttackSPData);
}
void AttackOperation::Draw() {
	// スキルアイコンを描画
	if (skillIconSPData.sprite) {
		skillIconSPData.sprite->Draw();
	}
	if (normalAttackSPData.sprite) {
		normalAttackSPData.sprite->Draw();
	}
	if (jumpSPData_.sprite) {
		jumpSPData_.sprite->Draw();
	}
	if (specialAttackSPData.sprite) {
		specialAttackSPData.sprite->Draw();
	}
	if (keyboardSkillIconSPData.sprite) {
		keyboardSkillIconSPData.sprite->Draw();
	}
	if (keyboardJumpSPData.sprite) {
		keyboardJumpSPData.sprite->Draw();
	}
	if (keyboardNormalAttackSPData.sprite) {
		keyboardNormalAttackSPData.sprite->Draw();
	}
	if (keyboardSpecialAttackSPData.sprite) {
		keyboardSpecialAttackSPData.sprite->Draw();
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
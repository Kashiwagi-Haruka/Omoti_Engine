#define NOMINMAX
#include "AttackOperation.h"
#include "PlayCommand/PlayCommand.h"
#include "Sprite/Sprite.h"
#include "TextureManager.h"
#include "WinApp.h"
#include "Input.h"
#include <algorithm>

namespace {
constexpr Vector2 kOperationIconBaseSize{90.0f, 90.0f};
constexpr float kPressedScaleRate = 1.15f;
constexpr float kScaleReturnRate = 0.25f;
constexpr Vector2 kKeyboardDisplaySize{100.0f, 100.0f};
constexpr float kKeyboardDisplayBottomMargin = 5.0f;
constexpr float kKeyboardDisplayIconOffsetY = 50.0f;
} // namespace
AttackOperation::AttackOperation() {
	// スキルアイコンのテクスチャハンドルを取得
	dashSPData_.sprite = std::make_unique<Sprite>();
	jumpSPData_.sprite = std::make_unique<Sprite>();
	skillIconSPData_.sprite = std::make_unique<Sprite>();
	normalAttackSPData_.sprite = std::make_unique<Sprite>();
	specialAttackSPData_.sprite = std::make_unique<Sprite>();

	keyboardDashSPData_.sprite = std::make_unique<Sprite>();
	keyboardSkillIconSPData_.sprite = std::make_unique<Sprite>();
	keyboardJumpSPData_.sprite = std::make_unique<Sprite>();
	keyboardNormalAttackSPData_.sprite = std::make_unique<Sprite>();
	keyboardSpecialAttackSPData_.sprite = std::make_unique<Sprite>();
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
}

void AttackOperation::Update() {
	skillIconSPData_.translate = {1260.0f, 700.0f};
	UpdateOperationSprite(skillIconSPData_, PlayCommand::GetSKILL_ATTACK());
	normalAttackSPData_.translate = {skillIconSPData_.translate.x - skillIconSPData_.size.x - 20.0f, 700.0f};
	UpdateOperationSprite(normalAttackSPData_, PlayCommand::GetNORMAL_ATTACK_PUSH());
	jumpSPData_.translate = {normalAttackSPData_.translate.x - normalAttackSPData_.size.x - 20.0f, 700.0f};
	UpdateOperationSprite(jumpSPData_, PlayCommand::GetJUMP());
	specialAttackSPData_.translate = {skillIconSPData_.translate.x, skillIconSPData_.translate.y - skillIconSPData_.size.y - 20.0f};
	UpdateOperationSprite(specialAttackSPData_, PlayCommand::GetSPECIAL_ATTACK());
	dashSPData_.translate = {specialAttackSPData_.translate.x - specialAttackSPData_.size.x - 20.0f, specialAttackSPData_.translate.y};
	UpdateOperationSprite(dashSPData_, PlayCommand::GetDASH());

	UpdateKeyboardSprite(keyboardSkillIconSPData_, skillIconSPData_);
	UpdateKeyboardSprite(keyboardJumpSPData_, jumpSPData_);
	UpdateKeyboardSprite(keyboardNormalAttackSPData_, normalAttackSPData_);
	UpdateKeyboardSprite(keyboardSpecialAttackSPData_, specialAttackSPData_);
	UpdateKeyboardSprite(keyboardDashSPData_, dashSPData_);
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
	if (keyboardDashSPData_.sprite) {
		keyboardDashSPData_.sprite->Draw();
	}
	if (keyboardSkillIconSPData_.sprite) {
		keyboardSkillIconSPData_.sprite->Draw();
	}
	if (keyboardJumpSPData_.sprite) {
		keyboardJumpSPData_.sprite->Draw();
	}
	if (keyboardNormalAttackSPData_.sprite) {
		keyboardNormalAttackSPData_.sprite->Draw();
	}
	if (keyboardSpecialAttackSPData_.sprite) {
		keyboardSpecialAttackSPData_.sprite->Draw();
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

void AttackOperation::UpdateKeyboardSprite(SpriteData& keyboardSpriteData, const SpriteData& iconSpriteData) {
	keyboardSpriteData.size = kKeyboardDisplaySize;
	keyboardSpriteData.translate = {
	    iconSpriteData.translate.x,
	    iconSpriteData.translate.y + kKeyboardDisplayIconOffsetY,
	};
	keyboardSpriteData.sprite->SetPosition(keyboardSpriteData.translate);
	keyboardSpriteData.sprite->SetScale(keyboardSpriteData.size);
	keyboardSpriteData.sprite->Update();
}
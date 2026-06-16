#include "AttackOperation.h"
#include "TextureManager.h"
#include "Sprite/Sprite.h"
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
	skillIconSPData.sprite->SetScale({90, 90});
	uint32_t jumpHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/skill.png");
	jumpSPData_.sprite->Initialize(jumpHandle);
	jumpSPData_.sprite->SetAnchorPoint({1.0f, 1.0f});
	jumpSPData_.sprite->SetScale({90, 90});
	uint32_t normalAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/skill.png");
	normalAttackSPData.sprite->Initialize(normalAttackHandle);
	normalAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	normalAttackSPData.sprite->SetScale({90, 90});
	uint32_t specialAttackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/AttackOperation/skill.png");
	specialAttackSPData.sprite->Initialize(specialAttackHandle);
	specialAttackSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	specialAttackSPData.sprite->SetScale({90, 90});
	uint32_t keyboardHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	keyboardSkillIconSPData.sprite->Initialize(keyboardHandle);
	keyboardJumpSPData.sprite->Initialize(keyboardHandle);
	keyboardNormalAttackSPData.sprite->Initialize(keyboardHandle);
	keyboardSpecialAttackSPData.sprite->Initialize(keyboardHandle);
}
void AttackOperation::Update() {
	skillIconSPData.translate = {1260.0f, 700.0f};
	skillIconSPData.sprite->SetPosition(skillIconSPData.translate);
	skillIconSPData.sprite->Update();
	normalAttackSPData.translate = {skillIconSPData.translate.x - skillIconSPData.size.x - 20.0f, 700.0f};
	normalAttackSPData.sprite->SetPosition(normalAttackSPData.translate);
	normalAttackSPData.sprite->Update();
	jumpSPData_.translate = {normalAttackSPData.translate.x - normalAttackSPData.size.x - 20.0f, 700.0f};
	jumpSPData_.sprite->SetPosition(jumpSPData_.translate);
	jumpSPData_.sprite->Update();
	specialAttackSPData.translate = {skillIconSPData.translate.x, skillIconSPData.translate.y - skillIconSPData.size.y - 20.0f};
	specialAttackSPData.sprite->SetPosition(specialAttackSPData.translate);
	specialAttackSPData.sprite->Update();

	keyboardSkillIconSPData.translate = {skillIconSPData.translate.x, skillIconSPData.translate.y+skillIconSPData.size.y-20.0f};
	keyboardSkillIconSPData.sprite->SetPosition(keyboardSkillIconSPData.translate);
	keyboardSkillIconSPData.sprite->Update();
	keyboardJumpSPData.translate = {jumpSPData_.translate.x, jumpSPData_.translate.y+jumpSPData_.size.y-20.0f};
	keyboardJumpSPData.sprite->SetPosition(keyboardJumpSPData.translate);
	keyboardJumpSPData.sprite->Update();
	keyboardNormalAttackSPData.translate = {normalAttackSPData.translate.x, normalAttackSPData.translate.y+normalAttackSPData.size.y-20.0f};
	keyboardNormalAttackSPData.sprite->SetPosition(keyboardNormalAttackSPData.translate);
	keyboardNormalAttackSPData.sprite->Update();
	keyboardSpecialAttackSPData.translate = {specialAttackSPData.translate.x, specialAttackSPData.translate.y+specialAttackSPData.size.y-20.0f};
	keyboardSpecialAttackSPData.sprite->SetPosition(keyboardSpecialAttackSPData.translate);
	keyboardSpecialAttackSPData.sprite->Update();
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
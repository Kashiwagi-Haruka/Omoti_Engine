#include "AttackOperation.h"
#include "TextureManager.h"
#include "Sprite/Sprite.h"
AttackOperation::AttackOperation() {
	// スキルアイコンのテクスチャハンドルを取得
	SkillIconSPData.handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/skill.png");
	SkillIconSPData.sprite = std::make_unique<Sprite>();
}
void AttackOperation::Initialize() {
	// スキルアイコンの初期化
	SkillIconSPData.sprite->Initialize(SkillIconSPData.handle);
	SkillIconSPData.sprite->SetAnchorPoint({1.0f, 1.0f});
	SkillIconSPData.sprite->SetScale({64, 64});
}
void AttackOperation::Update() {
	SkillIconSPData.translate = {1260, 700};
	SkillIconSPData.sprite->SetPosition(SkillIconSPData.translate);
	SkillIconSPData.sprite->Update();
}
void AttackOperation::Draw() {
	// スキルアイコンを描画
	if (SkillIconSPData.sprite) {
		SkillIconSPData.sprite->Draw();
	}
}
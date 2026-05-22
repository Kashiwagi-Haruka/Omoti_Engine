#include "PlayerSkill.h"
PlayerSkill::PlayerSkill() { sizuku_ = std::make_unique<SizukuSkill>(); }

void PlayerSkill::Initialize() { sizuku_->Initialize(); }
void PlayerSkill::Update() { sizuku_->Update(); }

void PlayerSkill::UpdateSpecialAttack(const Transform& playerTransform) { sizuku_->UpdateSpecialAttack(playerTransform); }

void PlayerSkill::StartAttack(const Transform& playerTransform) { sizuku_->StartAttack(playerTransform); }

void PlayerSkill::StartSpecialAttack(const Transform& playerTransform, int iceCount) { sizuku_->StartSpecialAttack(playerTransform, iceCount); }

void PlayerSkill::Draw() { sizuku_->Draw(); }

void PlayerSkill::DrawSpecialAttack() { sizuku_->DrawSpecialAttack(); }

bool PlayerSkill::IsSkillEnd() { return sizuku_->IsSkillEnd(); }
bool PlayerSkill::IsDamaging() const { return sizuku_->IsDamaging(); }
bool PlayerSkill::IsSpecialEnd() const { return sizuku_->IsSpecialEnd(); }
bool PlayerSkill::IsSpecialDamaging() const { return sizuku_->IsSpecialDamaging(); }
Vector3 PlayerSkill::GetDamagePosition() const { return sizuku_->GetDamagePosition(); }
Vector3 PlayerSkill::GetDamageScale() const { return sizuku_->GetDamageScale(); }
const std::vector<Transform>& PlayerSkill::GetSpecialIceFlowerTransforms() const { return sizuku_->GetSpecialIceFlowerTransforms(); }
int PlayerSkill::GetSkillDamageId() const { return sizuku_->GetSkillDamageId(); }


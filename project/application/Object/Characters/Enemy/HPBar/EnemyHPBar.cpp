#include "EnemyHPBar.h"
#include "Camera.h"
void EnemyHPBar::Initialize() {
	// HPバーの初期化処理
	Vector2 size = {5.0f, 0.5f}; // HPバーのサイズ
	hpBarPrimitive_ = std::make_unique<Primitive>();
	hpBarPrimitive_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/HPBar/Enemy/HP.png");
	hpBarPrimitive_->SetScale({size.x,size.y, 1.0f});
	hpBarPrimitive_->SetTranslate(position_);
	hpBarPrimitive_->SetUvAnchor({0.0f, 0.0f});
	hpBarPrimitive_->SetUvScale({0.5f, 1.0f, 1.0f});
	hpBarPrimitive_->SetCamera(camera_);
}
void EnemyHPBar::Update() {
	// HPバーの更新処理
	hpBarPrimitive_->SetCamera(camera_);
	hpBarPrimitive_->SetTranslate(position_);
	hpBarPrimitive_->Update();
}
void EnemyHPBar::Draw() {
	// HPバーの描画処理
	hpBarPrimitive_->Draw();
}
void EnemyHPBar::SetHP(int HP) {
	HP_ = HP;
}
void EnemyHPBar::SetMaxHP(int maxHP) {
	maxHP_ = maxHP;
}
void EnemyHPBar::SetPosition(const Vector3& position) { position_ = position; }
void EnemyHPBar::SetCamera(Camera* camera) { camera_ = camera; }
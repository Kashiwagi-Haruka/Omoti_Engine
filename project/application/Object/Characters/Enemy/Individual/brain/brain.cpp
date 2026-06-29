#include "brain.h"

brain::brain() {
	modelName_ = "Enemy";
	baseParameter_ = {30.0f, 8.0f, 5.0f};
	parameter_.level = 1;
	parameter_.HP = baseParameter_.HP;
	parameter_.Attack = baseParameter_.Attack;
	parameter_.Deffence = baseParameter_.Deffence;
	parameter_.Speed = 0.08f;
}

void brain::Initialize(Camera* camera, const Vector3& translate) { BaseEnemy::Initialize(camera, translate); }

void brain::Update() { BaseEnemy::Update(); }
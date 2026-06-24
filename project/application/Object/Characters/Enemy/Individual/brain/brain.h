#pragma once
#include "Object/Characters/Enemy/BaseEnemy.h"

class brain : public BaseEnemy {
public:
	brain();
	~brain() override = default;

	void Initialize(Camera* camera, const Vector3& translate) override;
	void Update() override;
};
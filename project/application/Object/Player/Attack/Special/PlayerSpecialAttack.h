#pragma once
#include "Object/Characters/Playable/Individual/Sizuku/SizukuSpecial.h"
#include "Object3d/Object3d.h"
#include "Transform.h"
#include <memory>
#include <vector>
class PlayerSpecialAttack {

	std::unique_ptr<SizukuSpecial> sizuku_;
	Camera* camera_ = nullptr;
	Transform transform_;

public:
	PlayerSpecialAttack();
	void Initialize();
	void Start();
	void End();
	void Update();
	void Draw();
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPlayerTransform(const Transform& playerTransform);
	bool IsEnd() { return sizuku_->isEnd(); };
	bool IsAnimationFinished() const { return sizuku_->IsAnimationFinished(); }
};

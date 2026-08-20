#pragma once

#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Transform.h"
#include <memory>

class Camera;

// Mei's special: an expanding fire field placed directly beneath her.
class MeiSpecial {
	std::unique_ptr<Primitive> fieldPlane_;
	Camera* camera_ = nullptr;
	Transform meiTransform_{};
	Transform fieldTransform_{};
	bool isStarted_ = false;
	bool isEnd_ = true;
	float elapsedTime_ = 0.0f;
	float meiHeight_ = 1.0f;
	int damageId_ = 0;

	static constexpr float kDuration_ = 2.0f;
	static constexpr float kFieldRadius_ = 12.0f;
	static constexpr float kGrowDuration_ = 0.65f;

public:
	MeiSpecial();
	void Initialize();
	void Start();
	void End();
	void Update();
	void Draw();

	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetMeiTransform(const Transform& transform) { meiTransform_ = transform; }
	void SetMeiHeight(float height) { meiHeight_ = height; }
	bool IsEnd() const { return isEnd_; }
	bool IsAnimationFinished() const { return isEnd_; }
	bool IsDamaging() const { return isStarted_; }
	Vector3 GetDamagePosition() const { return fieldTransform_.translate; }
	Vector3 GetDamageScale() const { return {fieldTransform_.scale.x, 3.0f, fieldTransform_.scale.y}; }
	int GetDamageId() const { return damageId_; }
};
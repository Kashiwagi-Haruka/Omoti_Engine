#pragma once
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Transform.h"
#include <memory>
#include <random>
#include <vector>

class Camera;

class SizukuSpecial {
	bool isStarted_ = false;
	bool isEnd_ = false;
	float elapsedTime_ = 0.0f;
	float animationTime_ = 0.0f;
	float animationTimeMax_ = 5.0f;
	float attackStartTime_ = 3.0f;
	float rainDuration_ = 3.0f;
	float flowerGrowTime_ = 0.65f;
	float sizukuHeight_ = 2.0f;
	float fieldSize_ = 50.0f;
	float flowerRadius_ = 9.0f;
	float rainRadius_ = 18.0f;
	float rainFallSpeed_ = 18.0f;
	int damageId_ = 0;

	std::unique_ptr<Primitive> fieldPlane_;
	std::unique_ptr<Object3d> skydomeObj_;
	std::unique_ptr<Object3d> iceFlower_;
	std::vector<std::unique_ptr<Object3d>> iceRains_;
	std::vector<Transform> iceRainTransforms_;
	Transform sizukuTransform_{};
	Transform fieldPlaneTransform_{};
	Transform skydomeTransform_{};
	Transform iceFlowerTransform_{};
	Camera* camera_ = nullptr;
	std::mt19937 randomEngine_{std::random_device{}()};

	void ResetRainDrop(size_t index, bool randomizeHeight);

public:
	SizukuSpecial();
	void Initialize();
	void Update();
	void Draw();
	void Start();
	void End();

	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetSizukuTransform(Transform transform) { sizukuTransform_ = transform; }
	void SetSizukuHeight(float height) { sizukuHeight_ = height; }
	bool isEnd() const { return isEnd_; }
	bool IsAnimationFinished() const { return animationTime_ >= animationTimeMax_; }
	bool IsFlowerDamaging() const { return isStarted_ && elapsedTime_ >= attackStartTime_ && elapsedTime_ < animationTimeMax_; }
	Vector3 GetDamagePosition() const { return iceFlowerTransform_.translate; }
	Vector3 GetDamageScale() const { return {flowerRadius_, 3.0f, flowerRadius_}; }
	int GetDamageId() const { return damageId_; }
};
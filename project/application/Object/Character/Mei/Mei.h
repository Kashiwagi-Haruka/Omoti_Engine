#pragma once

#include "Animation/AnimationManager.h"
#include "Animation/SkinCluster.h"
#include "Camera.h"
#include "Function.h"
#include "Object/Character/Base/CharacterBase.h"
#include "Object3d/Object3d.h"

#include <memory>
#include <optional>
#include <string>

class Mei : public CharacterBase {
	std::unique_ptr<Object3d> mei_;
	std::unique_ptr<Skeleton> meiSkeleton_{};
	SkinCluster meiSkinCluster_{};
	Animation::AnimationData blendedPoseAnimation_{};
	Matrix4x4 worldMatrix_{};
	Camera* camera_ = nullptr;

	const std::string animationGroupName_ = "sizuku";
	const float kAnimationBlendDuration_ = 0.3f;
	bool animationFinished_ = false;
	std::string desiredAnimationName_ = "Idle";

public:
	void Initialize();
	void SetAnimation(std::string name);
	void Update();
	void Draw();
	void SetCamera(Camera* camera);
	void SetTransform(Transform transform) { transform_ = transform; }
	std::optional<Matrix4x4> GetJointWorldMatrix(const std::string& jointName) const;
	bool IsAnimationFinished() const { return animationFinished_; }
	Object3d* GetObject3d() { return mei_.get(); }
};
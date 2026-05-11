#pragma once
#include "Animation/AnimationManager.h"
#include "Animation/SkinCluster.h"
#include "Camera.h"
#include "Function.h"
#include "Object/Characters/Base/CharacterBase.h"
#include "Object3d/Object3d.h"
#include <memory>
#include <string>
class Sizuku : public CharacterBase {

	bool isHave_;

	std::string name_ = "Sizuku";

	Matrix4x4 playerWorld;
	Camera* camera_;
	std::unique_ptr<Object3d> Sizuku_;
	std::unique_ptr<Skeleton> sizukuSkeleton_{};
	SkinCluster sizukuSkinCluster_{};
	Animation::AnimationData blendedPoseAnimation_{};
	const std::string animationGroupName_ = "sizuku";
	const float kAnimationBlendDuration_ = 0.3f;
	bool animationFinished_ = false;
	std::string desiredAnimationName = "Idle";

public:
	void Initialize();
	void SetAnimation(std::string Name);
	void Update();
	void Draw();
	void SetCamera(Camera* camera);
	void SetTransform(Transform transform) { transform_ = transform; }
	std::optional<Matrix4x4> GetJointWorldMatrix(const std::string& jointName) const;
	bool IsAnimationFinished() const { return animationFinished_; }
	Object3d* GetObject3d() { return Sizuku_.get(); }
};
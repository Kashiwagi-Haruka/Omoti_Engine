#pragma once

#include "Object/Characters/Enemy/Base/BaseEnemy.h"
#include "Object/Characters/Enemy/Base/Parameter/EnemyParameter.h"
#include "Animation/Animation.h"
#include "Animation/Skeleton.h"
#include "Animation/SkinCluster.h"

#include <vector>
#include <string>

class BrainStealer final : public BaseEnemy {
public:
	struct SpecificParameter {
		float playerChaseRange = 8.0f;
		float attackRange = 1.5f;
		float attackCooldown = 0.6f;
	};

	BrainStealer(const EnemyParameter& common, const SpecificParameter& specific);

	static const char* ParameterPath();
	static bool LoadParameter(EnemyParameter& outCommon, SpecificParameter& outSpecific, std::string& error);
	static bool SaveParameter(const EnemyParameter& common, const SpecificParameter& specific, std::string& error);

protected:
	void UpdateBehavior(const Vector3& housePosition, const Vector3& houseScale, const Vector3& playerPosition, bool isPlayerAlive, bool canMove) override;
	void InitializeModelAnimation(Object3d& object) override;
	void UpdateModelAnimation(float deltaTime, bool isMoving) override;

private:
	SpecificParameter specificParameter_;
	std::vector<Animation::AnimationData> animationClips_;
	std::unique_ptr<Skeleton> skeleton_;
	SkinCluster skinCluster_{};
	const Animation::AnimationData* currentClip_ = nullptr;
	float animationTime_ = 0.0f;
};
#include "Yuzuki.h"

#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

void Yuzuki::Initialize() {
	SetAttribute(Attribute::Thunder);
	SetWeaponTypes(WeaponTypes::GUN);
	yuzuki_ = std::make_unique<Object3d>();
	yuzuki_->SetModel("sizuku");
	yuzuki_->Initialize();

	AnimationManager::GetInstance()->LoadAnimationGroup(animationGroupName_, "Resources/3d", "sizuku");
	AnimationManager::GetInstance()->ResetPlayback(animationGroupName_, "Idle", true);
	if (const Animation::AnimationData* idleAnimation = AnimationManager::GetInstance()->FindAnimation(animationGroupName_, "Idle")) {
		yuzuki_->SetAnimation(idleAnimation, true);
	}

	if (Model* yuzukiModel = ModelManager::GetInstance()->FindModel("sizuku")) {
		yuzukiSkeleton_ = std::make_unique<Skeleton>(Skeleton().Create(yuzukiModel->GetModelData().rootnode));
		yuzukiSkinCluster_ = CreateSkinCluster(*yuzukiSkeleton_, *yuzukiModel);
		if (!yuzukiSkinCluster_.mappedPalette.empty()) {
			yuzuki_->SetSkinCluster(&yuzukiSkinCluster_);
		}
	}

	yuzuki_->SetShininess(20.0f);
	yuzuki_->SetOutlineWidth(2.0f);
	yuzuki_->SetOutlineColor({99.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f, 1.0f});
}

void Yuzuki::SetAnimation(std::string name) { desiredAnimationName_ = std::move(name); }

void Yuzuki::SetCamera(Camera* camera) { camera_ = camera; }

void Yuzuki::Update() {
	bool loopAnimation = true;
	if (desiredAnimationName_ == "Attack1" || desiredAnimationName_ == "Attack2" || desiredAnimationName_ == "Attack3" || desiredAnimationName_ == "Attack4" || desiredAnimationName_ == "FallAttack" ||
	    desiredAnimationName_ == "SkillAttack") {
		loopAnimation = false;
	}

	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();
	AnimationManager::PlaybackResult playbackResult{};
	if (AnimationManager::GetInstance()->UpdatePlayback(animationGroupName_, desiredAnimationName_, loopAnimation, deltaTime, kAnimationBlendDuration_, blendedPoseAnimation_, playbackResult)) {
		animationFinished_ = playbackResult.animationFinished;
		if (playbackResult.changedAnimation && playbackResult.currentAnimation) {
			yuzuki_->SetAnimation(playbackResult.currentAnimation, loopAnimation);
		}

		if (yuzukiSkeleton_ && playbackResult.animationToApply) {
			yuzukiSkeleton_->ApplyAnimation(*playbackResult.animationToApply, playbackResult.animationTime);
			yuzukiSkeleton_->Update();
			if (!yuzukiSkinCluster_.mappedPalette.empty()) {
				UpdateSkinCluster(yuzukiSkinCluster_, *yuzukiSkeleton_);
			}
		}
	}

	worldMatrix_ = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	yuzuki_->SetWorldMatrix(worldMatrix_);
	yuzuki_->SetCamera(camera_);
	yuzuki_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	yuzuki_->Update();
}

void Yuzuki::Draw() {
	yuzuki_->Draw();

#ifdef _DEBUG
	if (yuzukiSkeleton_) {
		yuzukiSkeleton_->SetObjectMatrix(worldMatrix_);
		Object3dCommon::GetInstance()->DrawCommonWireframeNoDepth();
	}
#endif
}

std::optional<Matrix4x4> Yuzuki::GetJointWorldMatrix(const std::string& jointName) const {
	if (!yuzukiSkeleton_) {
		return std::nullopt;
	}

	const auto jointIndex = yuzukiSkeleton_->FindJointIndex(jointName);
	if (!jointIndex.has_value()) {
		return std::nullopt;
	}

	const auto& joints = yuzukiSkeleton_->GetJoints();
	if (*jointIndex < 0 || static_cast<size_t>(*jointIndex) >= joints.size()) {
		return std::nullopt;
	}

	const Matrix4x4 world = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	return Function::Multiply(joints[*jointIndex].skeletonSpaceMatrix, world);
}
#include "Arte.h"

#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"

void Arte::Initialize() {
	SetAttribute(Attribute::None);
	SetWeaponTypes(WeaponTypes::NONE);
	arte_ = std::make_unique<Object3d>();
	arte_->SetModel("sizuku");
	arte_->Initialize();

	AnimationManager::GetInstance()->LoadAnimationGroup(animationGroupName_, "Resources/3d", "sizuku");
	AnimationManager::GetInstance()->ResetPlayback(animationGroupName_, "Idle", true);
	if (const Animation::AnimationData* idleAnimation = AnimationManager::GetInstance()->FindAnimation(animationGroupName_, "Idle")) {
		arte_->SetAnimation(idleAnimation, true);
	}

	if (Model* arteModel = ModelManager::GetInstance()->FindModel("sizuku")) {
		arteSkeleton_ = std::make_unique<Skeleton>(Skeleton().Create(arteModel->GetModelData().rootnode));
		arteSkinCluster_ = CreateSkinCluster(*arteSkeleton_, *arteModel);
		if (!arteSkinCluster_.mappedPalette.empty()) {
			arte_->SetSkinCluster(&arteSkinCluster_);
		}
	}

	arte_->SetShininess(20.0f);
	arte_->SetOutlineWidth(2.0f);
	arte_->SetOutlineColor({99.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f, 1.0f});
}

void Arte::SetAnimation(std::string name) { desiredAnimationName_ = std::move(name); }

void Arte::SetCamera(Camera* camera) { camera_ = camera; }

void Arte::Update() {
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
			arte_->SetAnimation(playbackResult.currentAnimation, loopAnimation);
		}

		if (arteSkeleton_ && playbackResult.animationToApply) {
			arteSkeleton_->ApplyAnimation(*playbackResult.animationToApply, playbackResult.animationTime);
			arteSkeleton_->Update();
			if (!arteSkinCluster_.mappedPalette.empty()) {
				UpdateSkinCluster(arteSkinCluster_, *arteSkeleton_);
			}
		}
	}

	worldMatrix_ = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	arte_->SetWorldMatrix(worldMatrix_);
	arte_->SetCamera(camera_);
	arte_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	arte_->Update();
}

void Arte::Draw() {
	arte_->Draw();

#ifdef _DEBUG
	if (arteSkeleton_) {
		arteSkeleton_->SetObjectMatrix(worldMatrix_);
		Object3dCommon::GetInstance()->DrawCommonWireframeNoDepth();
	}
#endif
}

std::optional<Matrix4x4> Arte::GetJointWorldMatrix(const std::string& jointName) const {
	if (!arteSkeleton_) {
		return std::nullopt;
	}

	const auto jointIndex = arteSkeleton_->FindJointIndex(jointName);
	if (!jointIndex.has_value()) {
		return std::nullopt;
	}

	const auto& joints = arteSkeleton_->GetJoints();
	if (*jointIndex < 0 || static_cast<size_t>(*jointIndex) >= joints.size()) {
		return std::nullopt;
	}

	const Matrix4x4 world = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	return Function::Multiply(joints[*jointIndex].skeletonSpaceMatrix, world);
}
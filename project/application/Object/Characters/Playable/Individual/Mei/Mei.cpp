#include "Mei.h"

#include "GameBase.h"
#include "Model/ModelManager.h"
#include "Engine/Texture/Data/Color.h"
#include "Object3d/Object3dCommon.h"

void Mei::Initialize() {
	SetAttribute(Attribute::Fire);
	SetWeaponTypes(WeaponTypes::SPEAR);
	mei_ = std::make_unique<Object3d>();
	mei_->SetModel("sizuku");
	mei_->Initialize();

	AnimationManager::GetInstance()->LoadAnimationGroup(animationGroupName_, "Resources/3d", "sizuku");
	AnimationManager::GetInstance()->ResetPlayback(animationGroupName_, "Idle", true);
	if (const Animation::AnimationData* idleAnimation = AnimationManager::GetInstance()->FindAnimation(animationGroupName_, "Idle")) {
		mei_->SetAnimation(idleAnimation, true);
	}

	if (Model* meiModel = ModelManager::GetInstance()->FindModel("sizuku")) {
		meiSkeleton_ = std::make_unique<Skeleton>(Skeleton().Create(meiModel->GetModelData().rootnode));
		meiSkinCluster_ = CreateSkinCluster(*meiSkeleton_, *meiModel);
		if (!meiSkinCluster_.mappedPalette.empty()) {
			mei_->SetSkinCluster(&meiSkinCluster_);
		}
	}

	mei_->SetShininess(20.0f);
	mei_->SetColor(Color::RGBAToVector4(255,106,145,255));
	mei_->SetOutlineWidth(2.0f);
	mei_->SetOutlineColor({99.0f / 255.0f, 48.0f / 255.0f, 48.0f / 255.0f, 1.0f});
}

void Mei::SetAnimation(std::string name) { desiredAnimationName_ = std::move(name); }

void Mei::SetCamera(Camera* camera) { camera_ = camera; }

void Mei::Update() {
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
			mei_->SetAnimation(playbackResult.currentAnimation, loopAnimation);
		}

		if (meiSkeleton_ && playbackResult.animationToApply) {
			meiSkeleton_->ApplyAnimation(*playbackResult.animationToApply, playbackResult.animationTime);
			meiSkeleton_->Update();
			if (!meiSkinCluster_.mappedPalette.empty()) {
				UpdateSkinCluster(meiSkinCluster_, *meiSkeleton_);
			}
		}
	}

	worldMatrix_ = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	mei_->SetWorldMatrix(worldMatrix_);
	mei_->SetCamera(camera_);
	mei_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	mei_->SetColor(Color::RGBAToVector4(255, 106, 145, 255));
	mei_->Update();
}

void Mei::Draw() {
	mei_->Draw();

#ifdef _DEBUG
	if (meiSkeleton_) {
		meiSkeleton_->SetObjectMatrix(worldMatrix_);
		Object3dCommon::GetInstance()->DrawCommonWireframeNoDepth();
	}
#endif
}

std::optional<Matrix4x4> Mei::GetJointWorldMatrix(const std::string& jointName) const {
	if (!meiSkeleton_) {
		return std::nullopt;
	}

	const auto jointIndex = meiSkeleton_->FindJointIndex(jointName);
	if (!jointIndex.has_value()) {
		return std::nullopt;
	}

	const auto& joints = meiSkeleton_->GetJoints();
	if (*jointIndex < 0 || static_cast<size_t>(*jointIndex) >= joints.size()) {
		return std::nullopt;
	}

	const Matrix4x4 world = Function::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	return Function::Multiply(joints[*jointIndex].skeletonSpaceMatrix, world);
}
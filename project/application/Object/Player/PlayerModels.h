#pragma once
#include "Camera.h"
#include "Function.h"
#include "Object/Characters/Playable/Base/PlayableBase.h"
#include "Object/Characters/Playable/Individual/Arte/Arte.h"
#include "Object/Characters/Playable/Individual/Mei/Mei.h"
#include "Object/Characters/Playable/Individual/Sizuku/Sizuku.h"
#include "Object/Characters/Playable/Individual/Yuzuki/Yuzuki.h"
#include "Transform.h"
#include <memory>
#include <optional>
#include <vector>
class PlayerModels {

public:
	enum StateM {
		idle,
		walk,
		attack1,
		attack2,
		attack3,
		attack4,
		fallingAttack,
		skillAttack,
		damage,

	};
	enum class CharacterType {
		Sizuku,
		Mei,
		Yuzuki,
		Arte,
	};

private:
	StateM state_;
	Camera* camera_;
	Transform player_;
	bool animationFinished_;
	std::unique_ptr<Sizuku> sizuku_;
	std::unique_ptr<Mei> mei_;
	std::unique_ptr<Yuzuki> yuzuki_;
	std::unique_ptr<Arte> arte_;
	PlayableBase* currentCharacter_ = nullptr;
	CharacterType currentCharacterType_ = CharacterType::Sizuku;

public:
	PlayerModels();
	~PlayerModels();
	void SetCamera(Camera* camera) { camera_ = camera; };
	void SetPlayerTransform(Transform player) { player_ = player; };
	void SetStateM(StateM state) { state_ = state; }
	void SetCharacterType(CharacterType characterType);
	void Initialize();
	void Update();
	void Draw();
	std::optional<Matrix4x4> GetJointWorldMatrix(const std::string& jointName) const;
	bool IsAttackAnimationFinished() const;
	Object3d* GetCharacterObject3d();
};
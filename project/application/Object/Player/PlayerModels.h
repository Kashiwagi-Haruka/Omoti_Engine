#pragma once
#include "Camera.h"
#include "Function.h"
#include "Object/Characters/Playable/Base/PlayableBase.h"
#include "Object/Characters/Playable/PlayableManager.h"
#include "Transform.h"
#include <optional>
#include <string>
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

private:
	StateM state_;
	Camera* camera_;
	Transform player_;
	bool animationFinished_;
	PlayableManager playableManager_;
	PlayableBase* currentCharacter_ = nullptr;

public:
	PlayerModels();
	~PlayerModels();
	void SetCamera(Camera* camera) { camera_ = camera; };
	void SetPlayerTransform(Transform player) { player_ = player; };
	void SetStateM(StateM state) { state_ = state; }
	void SetCharacterType(const std::string& characterName);
	void Initialize();
	void Update();
	void Draw();
	std::optional<Matrix4x4> GetJointWorldMatrix(const std::string& jointName) const;
	bool IsAttackAnimationFinished() const;
	Object3d* GetCharacterObject3d();
	Attribute GetCurrentAttribute() const;
};
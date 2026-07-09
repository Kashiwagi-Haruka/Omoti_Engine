#pragma once
#include "Object/Characters/Base/CharacterParameters.h"
#include "Transform.h"
#include "Vector3.h"
#include <memory>
#include <string>

class Camera;
class Object3d;

/// <summary>
/// 敵の基本クラス
/// </summary>
class BaseEnemy {
protected:
	BaseParameter baseParameter_{10.0f, 0.0f, 10.0f};
	Parameter parameter_{};
	Transform transform_{};
	Vector3 velocity_{};
	std::unique_ptr<Object3d> object_;
	Camera* camera_ = nullptr;
	bool isAlive_ = true;
	std::string modelName_ = "Enemy";

public:
	BaseEnemy();
	virtual ~BaseEnemy();

	virtual void Initialize(Camera* camera, const Vector3& translate);
	virtual void Update();
	virtual void Draw();

	void SetCamera(Camera* camera);
	void SetPosition(const Vector3& position);
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
	void SetModelName(const std::string& modelName) { modelName_ = modelName; }
	void SetBaseParameter(const BaseParameter& baseParameter) { baseParameter_ = baseParameter; }
	void SetParameter(const Parameter& parameter) { parameter_ = parameter; }
	void SetIsAlive(bool isAlive) { isAlive_ = isAlive; }

	const BaseParameter& GetBaseParameter() const { return baseParameter_; }
	const Parameter& GetParameter() const { return parameter_; }
	const Transform& GetTransform() const { return transform_; }
	Vector3 GetPosition() const { return transform_.translate; }
	Vector3 GetScale() const { return transform_.scale; }
	bool GetIsAlive() const { return isAlive_; }
};
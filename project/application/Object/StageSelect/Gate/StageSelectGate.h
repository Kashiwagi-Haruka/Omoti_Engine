#pragma once

#include "Engine/math/RigidBody.h"
#include "Object3d/Object3d.h"
#include "Primitive/Primitive.h"
#include <memory>
#include <string>

class Camera;

class StageSelectGate {
private:
	std::unique_ptr<Object3d> gate_;
	std::unique_ptr<Primitive> Portal_;
	std::unique_ptr<Primitive> StageNamePlate_;

	Vector3 position_{0.0f, 0.0f, 0.0f};

	// ポータルの表示サイズと判定の半径
	Vector3 portalScale_{4.0f, 4.0f, 1.0f};
	Vector3 portalHalfSize_{2.0f, 2.0f, 0.25f};

	AABB portalAABB_{};
	Camera* camera_ = nullptr;
	std::string stageName_ = "Stage1";

	void UpdateTransform();

public:
	StageSelectGate();
	~StageSelectGate() = default;

	void Initialize();
	void Update();
	void Draw();

	void SetStageName(const std::string& stageName);
	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetPosition(const Vector3& position);

	bool IsTouchingPlayer(const AABB& playerAABB) const;
};
#pragma once
#include "Transform.h"
#include <vector>

class Camera;

struct SizukuSpecialContext {
	const Transform& transform;
	Camera* camera;
	float height;
	float elapsedTime;
};

// 属性ごとの生成・更新・描画を所有する基底クラス。
class SizukuSpecialAttributeBase {
public:
	virtual ~SizukuSpecialAttributeBase() = default;
	virtual void Initialize() = 0;
	virtual void Start(const SizukuSpecialContext& context) = 0;
	virtual void End() {}
	virtual void Update(const SizukuSpecialContext& context, float deltaTime) = 0;
	virtual void Draw(Camera* camera) = 0;
	virtual float GetDuration() const = 0;
	virtual Vector3 GetDamagePosition() const = 0;
	virtual Vector3 GetDamageScale() const = 0;
	virtual const std::vector<Transform>& GetRainTransforms() const {
		static const std::vector<Transform> empty;
		return empty;
	}
};
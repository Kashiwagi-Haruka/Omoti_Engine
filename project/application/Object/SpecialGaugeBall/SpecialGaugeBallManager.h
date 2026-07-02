#pragma once
#include "SpecialGaugeBall.h"
#include "Vector3.h"
#include <memory>
#include <vector>

class Camera;

class SpecialGaugeBallManager {
public:
	void Initialize(Camera* camera);
	void Update(Camera* camera, const Vector3& movementLimitCenter, float movementLimitRadius);
	void Draw();
	void SpawnDrops(const Vector3& position, int count);
	void RemoveCollected();
	std::vector<std::unique_ptr<SpecialGaugeBall>>& SpecialGaugeBalls() { return SpecialGaugeBalls_; }

private:
	std::vector<std::unique_ptr<SpecialGaugeBall>> SpecialGaugeBalls_;
	Camera* camera_ = nullptr;
};
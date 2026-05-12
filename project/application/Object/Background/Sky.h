#pragma once
#include "Input.h"
#include "Engine/Texture/Mesh/SkyBox/SkyBox.h"
#include "Transform.h"
#include <memory>

class GameBase;
class Camera;

class Sky {

private:
	Transform transform_;
	std::unique_ptr<SkyBox> skyBox_ = nullptr;
	Camera* camera_;

public:
	Sky();
	~Sky() = default;
	void Initialize(Camera* camera);
	void Update();
	void Draw();
	void SetCamera(Camera* camera) { camera_ = camera; }
};

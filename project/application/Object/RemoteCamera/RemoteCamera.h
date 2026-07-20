#pragma once

#include "Camera.h"
#include "RenderTexture2D.h"
#include "Transform.h"
#include <cstdint>
#include <memory>

class Primitive;

// リモートカメラの映像を板ポリゴンへ表示する。
class RemoteCamera {
public:
	~RemoteCamera();

	void Initialize(uint32_t width, uint32_t height, Camera* displayCamera);
	void Update();
	bool BeginRender();
	void EndRender();
	void Draw();

	Camera* GetCamera() { return &camera_; }
	const Camera* GetCamera() const { return &camera_; }
	void SetCameraTransform(const Transform& transform) { camera_.SetTransform(transform); }
	void SetScreenTransform(const Transform& transform);

private:
	Camera camera_{};
	Camera* displayCamera_ = nullptr;
	RenderTexture2D renderTexture_{};
	std::unique_ptr<Primitive> screen_;
	Transform screenTransform_{};

	void UpdateScreenTransform();
};
#pragma once

#include "BaseScene.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "Light/CommonLight/DirectionalCommonLight.h"
#include "Object/Character/Sizuku/Sizuku.h"
#include "Object3d/Object3d.h"
#include "Text/Text.h"
#include "Transform.h"

#include <memory>

class SampleScene : public BaseScene {
private:
	std::unique_ptr<Object3d> fieldObj_ = nullptr;
	std::unique_ptr<Sizuku> sizukuObj_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	bool useDebugCamera_ = false;
	DirectionalCommonLight directionalLight_{};

	Transform cameraTransform_{};
	Transform fieldTransform_{};
	Transform sizukuTransform_{};
	Text sampleText_{};

	void DebugImgui();

public:
	SampleScene();
	~SampleScene() override = default;
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
};
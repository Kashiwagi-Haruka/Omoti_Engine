#pragma once
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Transform.h"
#include <vector>
#include <memory>
class Camera;
class SizukuSpecial{ 

	bool isStarted_ = false; // 開始フラグ
	bool isEnd_ = false;
	float duration_ = 0;     // 継続時間
	float durationMax_ = 1; // 最大継続時間(秒)
	float sizukuHeight_ = 0; // 雫の高さ
	float rainInterval_ = 0.5f; // 雨の降る間隔(秒)
	float rainTimer_ = 0;       // 雨のタイマー
	float fieldSize_ = 5.0f;    // フィールドのサイズ

	std::unique_ptr<Primitive> fieldPlane_;
	std::unique_ptr<Object3d> skydomeObj_;
	std::vector<std::unique_ptr<Object3d>> iceRains_;
	Transform sizukuTransform_;
	Transform fieldPlaneTransform_;
	Transform skydomeTransform_;
	Camera* camera_ = nullptr;

public:
	SizukuSpecial();
	void Initialize();
	void Update();
	void Draw();
	void Start();
	void End();

	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetSizukuTransform(Transform transform) { sizukuTransform_ = transform; }
	void SetSizukuHeight(float height) {sizukuHeight_ = height; }
	bool isEnd() { return isEnd_; }

};

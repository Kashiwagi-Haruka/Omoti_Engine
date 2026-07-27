#include "RemoteCamera.h"

#include "DirectXCommon.h"
#include "Function.h"
#include "Object3d/Object3dCommon.h"
#include "Primitive/Primitive.h"
#include "SrvManager/SrvManager.h"
#include "TextureManager.h"

RemoteCamera::~RemoteCamera() = default;

void RemoteCamera::Initialize(uint32_t width, uint32_t height, Camera* displayCamera) {
	if (width == 0 || height == 0) {
		return;
	}

	displayCamera_ = displayCamera;
	camera_.SetAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	camera_.Update();

	renderTexture_.Initialize(width, height, DXGI_FORMAT_R8G8B8A8_UNORM, {0.05f, 0.05f, 0.05f, 1.0f});
	if (!renderTexture_.IsReady()) {
		return;
	}

	screen_ = std::make_unique<Primitive>();
	screen_->Initialize(Primitive::Plane);
	screen_->SetTextureIndex(renderTexture_.GetSrvIndex());
	screen_->SetEnableLighting(false);
	screen_->SetCamera(displayCamera_);
	UpdateScreenTransform();
}

void RemoteCamera::Update() {
	camera_.Update();
	if (screen_) {
		screen_->SetCamera(displayCamera_);
		UpdateScreenTransform();
		screen_->Update();
	}
}

bool RemoteCamera::BeginRender() {
	if (!renderTexture_.IsReady()) {
		return false;
	}

	renderTexture_.BeginRender();
	// Some drawables fall back to Object3dCommon's default camera when they do
	// not own a camera.  Switch that fallback for the entire off-screen pass.
	Object3dCommon* const object3dCommon = Object3dCommon::GetInstance();
	previousDefaultCamera_ = object3dCommon->GetDefaultCamera();
	object3dCommon->SetDefaultCamera(&camera_);
	return true;
}
void RemoteCamera::RestoreRenderTarget() {
	if (!renderTexture_.IsReady()) {
		return;
	}

	renderTexture_.BindRenderTarget();
}
void RemoteCamera::EndRender() {
	if (!renderTexture_.IsReady()) {
		return;
	}

	renderTexture_.TransitionToShaderResource();
	DirectXCommon* const dxCommon = Object3dCommon::GetInstance()->GetDxCommon();
	dxCommon->ExecuteCommandListAndWait();
	TextureManager::GetInstance()->GetSrvManager()->PreDraw();
	dxCommon->SetMainRenderTarget();
	Object3dCommon::GetInstance()->SetDefaultCamera(previousDefaultCamera_);
	previousDefaultCamera_ = nullptr;
}

void RemoteCamera::Draw() {
	if (!screen_) {
		return;
	}
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::NoCull);
	screen_->Draw();
}

void RemoteCamera::SetScreenTransform(const Transform& transform) {
	screenTransform_ = transform;
	UpdateScreenTransform();
}

void RemoteCamera::UpdateScreenTransform() {
	if (!screen_) {
		return;
	}

	const Matrix4x4 localMatrix = Function::MakeAffineMatrix(screenTransform_.scale, screenTransform_.rotate, screenTransform_.translate);
	const Matrix4x4 displayCameraWorldMatrix = displayCamera_ ? displayCamera_->GetWorldMatrix() : Function::MakeIdentity4x4();
	screen_->SetWorldMatrix(Function::Multiply(localMatrix, displayCameraWorldMatrix));
}
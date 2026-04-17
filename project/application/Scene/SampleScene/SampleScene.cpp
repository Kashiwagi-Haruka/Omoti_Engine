#include "SampleScene.h"

#include "Input.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3dCommon.h"
#include "Sprite/SpriteCommon.h"
#include "Text/FreetypeManager/FreeTypeManager.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif // USE_IMGUI

#include <numbers>

SampleScene::SampleScene() {
	fieldObj_ = std::make_unique<Object3d>();
	sizukuObj_ = std::make_unique<Sizuku>();
	camera_ = std::make_unique<Camera>();
	debugCamera_ = std::make_unique<DebugCamera>();

	cameraTransform_ = {
	    .scale{1.0f, 1.0f, 1.0f  },
	    .rotate{0.0f, 0.0f, 0.0f  },
	    .translate{0.0f, 5.0f, -10.0f},
	};
	
	camera_->SetTransform(cameraTransform_);

	fieldTransform_ = {
	    .scale{1.0f, 1.0f, 1.0f},
	    .rotate{0.0f, 0.0f, 0.0f},
	    .translate{0.0f, 0.0f, 0.0f},
	};

	sizukuTransform_ = {
	    .scale{0.2f,  0.2f,                      0.2f },
	    .rotate{0.0f,  std::numbers::pi_v<float>, 0.0f },
	    .translate{-2.0f, 2.0f,                      -1.0f},
	};

	ModelManager::GetInstance()->LoadModel("Resources/3d", "terrain");
	ModelManager::GetInstance()->LoadGltfModel("Resources/3d", "sizuku");
}

void SampleScene::Initialize() {
	debugCamera_->Initialize();
	debugCamera_->SetTranslation(cameraTransform_.translate);

	fieldObj_->Initialize();
	fieldObj_->SetCamera(camera_.get());
	fieldObj_->SetModel("terrain");
	fieldObj_->SetTransform(fieldTransform_);

	sizukuObj_->Initialize();
	sizukuObj_->SetCamera(camera_.get());

	sizukuObj_->SetAnimation("Idle");
	sizukuObj_->SetTransform(sizukuTransform_);

	const uint32_t sampleFontHandle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Regular.ttf", 0);
	FreeTypeManager::SetPixelSizes(sampleFontHandle, 36, 36);
	sampleText_.Initialize(sampleFontHandle);
	sampleText_.SetSize({600.0f, 100.0f});
	sampleText_.SetString(U"SampleText");
	sampleText_.SetPosition({640.0f, 60.0f});
	sampleText_.SetAlign(TextAlign::Center);
	sampleText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	sampleText_.UpdateLayout(false);

	directionalLight_.color = {1.0f, 1.0f, 1.0f, 1.0f};
	directionalLight_.direction = {0.0f, -1.0f, 0.0f};
	directionalLight_.intensity = 1.0f;
	directionalLight_.shadowEnabled = 1;
}

void SampleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_F1)) {
		useDebugCamera_ = !useDebugCamera_;
	}

	DebugImgui();

	if (useDebugCamera_) {
		debugCamera_->Update();
		camera_->SetViewProjectionMatrix(debugCamera_->GetViewMatrix(), debugCamera_->GetProjectionMatrix());
	} else {
		camera_->SetTransform(cameraTransform_);
		camera_->Update();
	}

	Object3dCommon::GetInstance()->SetDirectionalLight(directionalLight_);
	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	fieldObj_->SetCamera(camera_.get());
	fieldObj_->SetTransform(fieldTransform_);
	fieldObj_->Update();

	sizukuObj_->SetCamera(camera_.get());
	sizukuObj_->SetTransform(sizukuTransform_);
	sizukuObj_->Update();
}

void SampleScene::DebugImgui() {
#ifdef USE_IMGUI
	if (ImGui::Begin("SampleScene")) {
		ImGui::Checkbox("Use Debug Camera (F1)", &useDebugCamera_);
		if (!useDebugCamera_) {
			ImGui::DragFloat3("Camera Rotate", &cameraTransform_.rotate.x, 0.01f);
			ImGui::DragFloat3("Camera Translate", &cameraTransform_.translate.x, 0.01f);
		}
		ImGui::Separator();
		ImGui::Text("Sizuku");
		ImGui::DragFloat3("Sizuku Rotate", &sizukuTransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Sizuku Translate", &sizukuTransform_.translate.x, 0.01f);
		ImGui::Separator();
		ImGui::Text("Directional Light");
		ImGui::DragFloat3("Light Direction", &directionalLight_.direction.x, 0.01f);
		ImGui::DragFloat("Light Intensity", &directionalLight_.intensity, 0.01f, 0.0f, 10.0f);
	}
	ImGui::End();
#endif // USE_IMGUI
}

void SampleScene::Draw() {

	Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());

	Object3dCommon::GetInstance()->DrawCommon();
	fieldObj_->Draw();

	Object3dCommon::GetInstance()->DrawCommonSkinning();
	sizukuObj_->Draw();

	Object3dCommon::GetInstance()->DrawCommonSkinningToonOutline();
	sizukuObj_->Draw();
	Object3dCommon::GetInstance()->EndOutlineDraw();

	SpriteCommon::GetInstance()->DrawCommon();
	sampleText_.Draw();
}

void SampleScene::Finalize() {}
#include "GameLoad.h"
#include "TextureManager.h"
#include "Model/ModelManager.h"
#include "Sprite.h"
#include "Model/Model.h"
#include "Sprite/SpriteCommon.h"
#include "WinApp.h"
#include <chrono>
#include <filesystem>
#include <stdexcept>

std::unique_ptr<GameLoad> GameLoad::instance_ = nullptr;

GameLoad* GameLoad::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = std::make_unique<GameLoad>();
	}
	return instance_.get();
}

GameLoad::GameLoad() {};
GameLoad::~GameLoad() {
	if (loadFuture_.valid()) {
		loadFuture_.wait();
		
	}
	
}
void GameLoad::Finalize() { instance_.reset(); }

void GameLoad::GameSceneLoad() {
	if (isLoading_ || isLoadComplete_) {
		return;
	}

	InitializeLoadSprites();

	requests_ = {
	    {"Resources/3d/iceFlower", "iceFlower"},
	    {"Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain"},
	    {"Resources/3d", "HitEffect"},
	    {"Resources/3d/HitIce", "HitIce"},
	    {"Resources/3d/CharacterDisplay", "CharacterDisplaySkydome"},
	    {"Resources/3d", "sizuku", true},
	    {"Resources/3d", "Enemy"},
	    {"Resources/3d", "WaterBoss", true},
	    {"Resources/3d", "EnemyStun"},
	    {"Resources/3d", "EnemyAttack"},
	    {"Resources/3d", "house"},
	    {"Resources/3d", "HPBar"},
	    {"Resources/3d", "HPFlame"},
	    {"Resources/3d", "FallingEffect"},
	    {"Resources/3d", "playerSkillUp"},
	    {"Resources/3d", "playerSkillUnder"},
	    {"Resources/3d/Key", "Key"},
	    {"Resources/3d", "playerSword"},
#ifdef USE_IMGUI
	    {"Resources/3d", "debugBox"},
#endif
	};

	nextRequest_ = 0;
	materialTextureHandles_.clear();
	loadProgressPercent_ = 0.0f;
	isLoadComplete_ = false;
	hasLoadError_ = false;
	loadError_.clear();
	isLoading_ = true;

}

void GameLoad::InitializeLoadSprites() {
	if (loadingSpinner_) {
		return;
		
	}
	auto* textures = TextureManager::GetInstance();
	illustrationHandle_ = textures->GetTextureIndexByfilePath("Resources/2d/Load/Load.png");
	const uint32_t white = textures->GetTextureIndexByfilePath(+kDefaultWhiteTexturePath);
	loadingSpinner_ = std::make_unique<Sprite>();
	loadingSpinner_->Initialize(illustrationHandle_);
	barBackground_ = std::make_unique<Sprite>();
	barBackground_->Initialize(white);
	barBackground_->SetColor({0.15f, 0.15f, 0.15f, 1.0f});
	barFill_ = std::make_unique<Sprite>();
	barFill_->Initialize(white);
	barFill_->SetColor({0.2f, 0.7f, 1.0f, 1.0f});
	const float width = static_cast<float>(WinApp::kClientWidth);
	const float height = static_cast<float>(WinApp::kClientHeight);
	// 左上基準で配置する
	loadingSpinner_->SetPosition({(width - 200.0f), height * 0.65f});
	loadingSpinner_->SetScale({100.0f, 100.0f});
	const Vector2 barPosition{(width - 600.0f) * 0.5f, height * 0.8f};
	barBackground_->SetPosition(barPosition);
	barBackground_->SetScale({600.0f, 24.0f});
	barFill_->SetPosition(barPosition);
	barFill_->SetScale({0.0f, 24.0f});
	loadingSpinner_->Update();
	barBackground_->Update();
	barFill_->Update();
	
}
void GameLoad::StartNextModelLoad() {
	const LoadRequest request = requests_.at(nextRequest_);

	loadFuture_ = std::async(std::launch::async, [request]() {
		namespace fs = std::filesystem;

		fs::path source = fs::path(request.directory) / (request.key + (request.isGltf ? ".gltf" : ".obj"));

		if (request.isGltf && !fs::exists(source)) {
			source = fs::path(request.directory) / (request.key + ".glb");
		}

		if (!fs::exists(source)) {
			throw std::runtime_error("Model file not found: " + source.string());
		}

		LoadedModel result;
		result.directory = source.parent_path().string();
		result.filename = source.filename().string();
		result.model = std::make_unique<Model>();

		if (request.isGltf) {
			result.model->LoadObjFileGltf(result.directory, result.filename);
		} else {
			result.model->LoadObjFileAssimp(result.directory, result.filename);
		}

		if (result.model->GetModelData().vertices.empty()) {
			throw std::runtime_error("Model has no vertices: " + source.string());
		}
		return result;
	});
}
void GameLoad::Update() {
	if (!isLoading_) {
		return;
	}

	auto* textures = TextureManager::GetInstance();
	auto* models = ModelManager::GetInstance();

	// ロード画面の画像が出せるようになってから開始
	if (!textures->IsTextureLoaded(illustrationHandle_)) {
		return;
	}

	try {
		if (loadFuture_.valid() && loadFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {

			LoadedModel loaded = loadFuture_.get();
			const LoadRequest& request = requests_.at(nextRequest_);

			// GPUリソース作成・テクスチャ登録はメインスレッド
			loaded.model->Initialize();

			materialTextureHandles_.push_back(loaded.model->GetModelData().material.textureIndex);

			models->RegisterLoadedModel(request.key, loaded.directory, loaded.filename, request.isGltf, std::move(loaded.model));

			++nextRequest_;
		}

		if (!loadFuture_.valid() && nextRequest_ < requests_.size()) {
			const LoadRequest& request = requests_.at(nextRequest_);

			if (Model* existing = models->FindModel(request.key)) {
				materialTextureHandles_.push_back(existing->GetModelData().material.textureIndex);
				++nextRequest_;
			} else {
				StartNextModelLoad();
			}
		}

		size_t readyTextures = 0;
		for (uint32_t handle : materialTextureHandles_) {
			if (textures->IsTextureLoaded(handle)) {
				++readyTextures;
			}
		}

		// 1モデルにつき「モデル登録」「テクスチャ準備」の2段階
		const size_t totalSteps = requests_.size() * 2;
		const size_t completedSteps = nextRequest_ + readyTextures;

		loadProgressPercent_ = totalSteps == 0 ? 100.0f : 100.0f * static_cast<float>(completedSteps) / static_cast<float>(totalSteps);

		if (nextRequest_ == requests_.size() && readyTextures == requests_.size()) {
			loadProgressPercent_ = 100.0f;
			isLoadComplete_ = true;
			isLoading_ = false;
		}
	} catch (const std::exception& error) {
		loadError_ = error.what();
		hasLoadError_ = true;
		isLoading_ = false;
		// 失敗時はisLoadComplete_をtrueにしない
		OutputDebugStringA(loadError_.c_str());
	}

	barFill_->SetScale({600.0f * (loadProgressPercent_ / 100.0f), 24.0f});

	loadingSpinner_->Update();
	barBackground_->Update();
	barFill_->Update();
}
void GameLoad::DrawLoadProgress() {
	if (!loadingSpinner_) {
		return;
		
	}
	SpriteCommon::GetInstance()->DrawCommon();
	// 未ロードのイラストは白い代替画像になるため、完了後に描画
	  if (TextureManager::GetInstance()->IsTextureLoaded(+illustrationHandle_)) {
		loadingSpinner_->Draw();
		
	}
	barBackground_->Draw();
	if (loadProgressPercent_ > 0.0f) {
		barFill_->Draw();
		
	}
}
#pragma once

#include <cstdint>
#include <future>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Sprite;
class Model;

class GameLoad {
	static std::unique_ptr<GameLoad> instance_;

	bool isLoadComplete_ = false;
	bool isLoading_ = false;
	float loadProgressPercent_ = 0.0f;

	struct LoadRequest {
		std::string directory;
		std::string key;
		bool isGltf = false;
	};

	struct TextureLoadRequest {
		std::string key;
		std::string filePath;
	};

	struct LoadedModel {
		std::unique_ptr<Model> model;
		std::string directory;
		std::string filename;
	};

	// モデルのロード管理
	std::vector<LoadRequest> requests_;
	size_t nextRequest_ = 0;
	std::future<LoadedModel> loadFuture_;
	std::vector<uint32_t> materialTextureHandles_;

	// 単独テクスチャのロード管理
	std::vector<TextureLoadRequest> textureRequests_;
	size_t nextTextureRequest_ = 0;
	std::unordered_map<std::string, uint32_t> textureHandles_;

	// ロード画面
	std::unique_ptr<Sprite> loadingSpinner_;
	std::unique_ptr<Sprite> barBackground_;
	std::unique_ptr<Sprite> barFill_;
	uint32_t illustrationHandle_ = 0;

	bool hasLoadError_ = false;
	std::string loadError_;

	void StartNextModelLoad();

public:
	static GameLoad* GetInstance();

	GameLoad();
	~GameLoad();

	void Finalize();

	void InitializeLoadSprites();
	void UpdateLoadSprites();
	void GameSceneLoad();
	void Update();
	void DrawLoadProgress();

	bool IsLoadComplete() const { return isLoadComplete_; }

	bool HasLoadError() const { return hasLoadError_; }

	const std::string& GetLoadError() const { return loadError_; }

	// 登録済みの名前から取得する。
	// 未登録名を指定した場合は std::out_of_range。
	uint32_t GetTextureHandle(const std::string& textureName) const;
};
#pragma once
#include <cstdint>
#include <string>
#include <memory>

class Sprite;

class GameLoad {

	static std::unique_ptr<GameLoad> instance_;
	bool isLoadComplete_ = false;
	bool isLoading_ = false;
	float loadProgressPercent_ = 0.0f;
	
	public:
	
	static GameLoad* GetInstance();
	GameLoad();
	~GameLoad();
	void Finalize();

	void GameSceneLoad();
	void DrawLoadProgress();
	uint32_t GetTextureHandle(const std::string& textureName) const;

};

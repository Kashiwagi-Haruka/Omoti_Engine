#include "GameLoad.h"
#include "TextureManager.h"
#include "Model/ModelManager.h"
#include "Sprite.h"

std::unique_ptr<GameLoad> GameLoad::instance_ = nullptr;

GameLoad* GameLoad::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = std::make_unique<GameLoad>();
	}
	return instance_.get();
}

GameLoad::GameLoad() {};
GameLoad::~GameLoad() {};
void GameLoad::Finalize() { instance_.reset(); }

void GameLoad::GameSceneLoad() {
	// ゲーム中のテクスチャやモデルのロード
	ModelManager::GetInstance()->LoadModel("Resources/3d/iceFlower", "iceFlower");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Character/Sizuku/Special/Rain", "sizukuSpecialRain");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "HitEffect");
	ModelManager::GetInstance()->LoadModel("Resources/3d/HitIce", "HitIce");
	ModelManager::GetInstance()->LoadModel("Resources/3d/CharacterDisplay", "CharacterDisplaySkydome");
	ModelManager::GetInstance()->LoadGltfModel("Resources/3d", "sizuku");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "Enemy");
	ModelManager::GetInstance()->LoadGltfModel("Resources/3d", "WaterBoss");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "EnemyStun");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "EnemyAttack"); // 鎌モデル
	ModelManager::GetInstance()->LoadModel("Resources/3d", "house");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "HPBar");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "HPFlame");
	ModelManager::GetInstance()->LoadGltfModel("Resources/3d", "WaterBoss");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "FallingEffect");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "playerSkillUp");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "playerSkillUnder");
	ModelManager::GetInstance()->LoadModel("Resources/3d/Key", "Key");
	ModelManager::GetInstance()->LoadModel("Resources/3d", "playerSword");
#ifdef USE_IMGUI
	ModelManager::GetInstance()->LoadModel("Resources/3d", "debugBox");
#endif // USE_IMGUI	
}

void GameLoad::DrawLoadProgress() {
	

}
#include "SceneFactory.h"

#include "Scene/SampleScene/SampleScene.h"
#include "Scene/Scenes/CharacterScene/CharacterScene.h"
#include "Scene/Scenes/GameOverScene/GameOverScene.h"
#include "Scene/Scenes/GameScene/GameScene.h"
#include "Scene/Scenes/ResultScene/ResultScene.h"
#include "Scene/Scenes/StageSelectScene/StageSelectScene.h"
#include "Scene/Scenes/TitleScene/TitleScene.h"
#include "Scene/Scenes/TutorialScene/TutorialScene.h"

#include <memory>

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	if (sceneName == "Title") {
		return std::make_unique<TitleScene>();
	}
	if (sceneName == "Game") {
		return std::make_unique<GameScene>();
	}
	if (sceneName == "Result") {
		return std::make_unique<ResultScene>();
	}
	if (sceneName == "GameOver") {
		return std::make_unique<GameOverScene>();
	}
	if (sceneName == "Sample") {
		return std::make_unique<SampleScene>();
	}
	if (sceneName == "Tutorial") {
		return std::make_unique<TutorialScene>();
	}
	if (sceneName == "StageSelect") {
		return std::make_unique<StageSelectScene>();
	}
	if (sceneName == "Character") {
		return std::make_unique<CharacterScene>();
	}

	return nullptr;
}

std::vector<std::string> SceneFactory::GetSceneNames() const { return {"Title", "Game", "Result", "GameOver", "Sample", "StageSelect", "Character"}; }
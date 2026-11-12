#pragma once

#include "BaseScene.h"

#include <array>
#include <cstddef>
#include <string>

#include <nlohmann/json.hpp>

class CharacterScene : public BaseScene {
public:
	void Initialize() override;
	void Update() override;
	void Draw() override;
	void Finalize() override;
	void DrawEditorUI() override;

private:
	struct CharacterData {
		std::string name;
		nlohmann::json lv1;
		nlohmann::json current;
		bool loaded = false;
		bool dirty = false;
	};

	void Load(CharacterData& data);
	void Save(CharacterData& data);
	void Recalculate(CharacterData& data);

	std::array<CharacterData, 4> characters_{};
	std::size_t selectedIndex_ = 0;
	bool initialized_ = false;
	std::string status_;
};
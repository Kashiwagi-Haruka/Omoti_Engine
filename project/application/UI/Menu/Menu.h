#pragma once
#include "Sprite.h"
#include <memory>
class Menu {
public:
    Menu();
    ~Menu();
    void Initialize();
    void Update();
    void Draw();

private:
    std::unique_ptr<Sprite> pauseSprite_;
	std::unique_ptr<Sprite> characterDisplaySprite_;
	std::unique_ptr<Sprite> teamSelectSprite_;
};
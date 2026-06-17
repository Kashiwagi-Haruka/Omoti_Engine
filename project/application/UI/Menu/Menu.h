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
	std::unique_ptr<Sprite> helpSprite_;

    std::unique_ptr<Sprite> pauseKeyboardSprite_;
	std::unique_ptr<Sprite> characterDisplayKeyboardSprite_;
	std::unique_ptr<Sprite> teamSelectKeyboardSprite_;
    std::unique_ptr<Sprite> helpKeyboardSprite_;

};
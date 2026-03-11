#include "OpenWorld.h"
#include "Camera.h"
#include "Object/Player/Player.h"

void OpenWorld::Initialize(Camera* camera) { (void)camera; }

void OpenWorld::Update(Camera* camera, Player* player) {
	(void)camera;
	(void)player;
}

void OpenWorld::Draw() {}

void OpenWorld::Finalize() {}
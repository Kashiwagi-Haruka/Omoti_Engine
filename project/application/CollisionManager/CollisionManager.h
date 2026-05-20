#pragma once
#include "Vector3.h"

class EnemyManager;
class ExpCubeManager;
class House;
class Player;
class Boss;

class CollisionManager {
public:
	bool HandleGameSceneCollisions(Player& player, EnemyManager& enemyManager, ExpCubeManager& expCubeManager, House& house, Boss* boss, Vector3* outHitEnemyPos = nullptr);
};
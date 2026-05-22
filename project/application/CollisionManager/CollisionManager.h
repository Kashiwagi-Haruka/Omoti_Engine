#pragma once
#include "Vector3.h"
#include "Engine/math/RigidBody.h"
#include <string>
#include <vector>
class EnemyManager;
class ExpCubeManager;
class House;
class Player;
class Boss;

class CollisionManager {
public:

	void AddColider(std::string name,AABB aabb);
	void AddColider(std::string name,std::vector<AABB> aabb);

	bool CheckCollision(std::string name, std::string name2);

	void Update();

	bool HandleGameSceneCollisions(Player& player, EnemyManager& enemyManager, ExpCubeManager& expCubeManager, House& house, Boss* boss, Vector3* outHitEnemyPos = nullptr);
};
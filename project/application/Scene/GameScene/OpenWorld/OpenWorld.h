#pragma once

class Camera;
class Player;

class OpenWorld {
public:
	OpenWorld() = default;
	~OpenWorld() = default;

	void Initialize(Camera* camera);
	void Update(Camera* camera, Player* player);
	void Draw();
	void Finalize();
};
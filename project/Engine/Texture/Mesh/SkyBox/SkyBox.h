#pragma once
#include <string>
class SkyBox {

	public:
	SkyBox();
	~SkyBox();
	void Initialize();
	void SetDDSTexture(const std::string& filePath);
	void Update();
	void Draw();

};

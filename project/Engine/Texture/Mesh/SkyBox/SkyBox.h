#pragma once
#include <string>
#include "Vector3.h"
class SkyBox {

	public:
	SkyBox();
	~SkyBox();
	void Initialize();
	void SetDDSTexture(const std::string& filePath);
	void SetScale(const Vector3& scale);
	void SetRotation(const Vector3& rotation);
	void Update();
	void Draw();

};

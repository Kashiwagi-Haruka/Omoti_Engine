#pragma once
#include "Camera.h"
#include <memory>
class SizukuSpecialCamera {

	enum class CHAPTAR{
		// 破片をくるくる 
		TURN,
		// 指パッチン
		FINGERSNAP,
		// 爆破
		ATTACK,
	};

	std::unique_ptr<Camera> camera_;


	public:
	
	void Initialize();
	void Update();

};

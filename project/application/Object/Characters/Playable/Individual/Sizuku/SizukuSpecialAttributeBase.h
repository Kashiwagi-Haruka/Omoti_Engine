#pragma once

class SizukuSpecial;

// 属性ごとの必殺技処理を差し替えるための基底クラス。
class SizukuSpecialAttributeBase {
public:
	virtual ~SizukuSpecialAttributeBase() = default;
	virtual void Start(SizukuSpecial& special) = 0;
	virtual void Update(SizukuSpecial& special, float deltaTime) = 0;
};
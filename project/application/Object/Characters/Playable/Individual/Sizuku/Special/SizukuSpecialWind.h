#pragma once
#include "SizukuSpecialAttributeBase.h"

// シズクの風属性必殺技を担当するクラス。
class SizukuSpecialWind final : public SizukuSpecialAttributeBase {
public:
	void Start(SizukuSpecial& special) override;
	void Update(SizukuSpecial& special, float deltaTime) override;
};
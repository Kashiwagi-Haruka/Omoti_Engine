#pragma once
#include "SizukuSpecialAttributeBase.h"

// シズクの量子属性必殺技を担当するクラス。
class SizukuSpecialQuantum final : public SizukuSpecialAttributeBase {
public:
	void Start(SizukuSpecial& special) override;
	void Update(SizukuSpecial& special, float deltaTime) override;
};
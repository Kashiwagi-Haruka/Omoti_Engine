#pragma once
#include "SizukuSpecialAttributeBase.h"

// シズクの虚数属性必殺技を担当するクラス。
class SizukuSpecialImaginary final : public SizukuSpecialAttributeBase {
public:
	void Start(SizukuSpecial& special) override;
	void Update(SizukuSpecial& special, float deltaTime) override;
};
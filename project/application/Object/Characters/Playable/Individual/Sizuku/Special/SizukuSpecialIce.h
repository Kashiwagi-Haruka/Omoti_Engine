#pragma once
#include "SizukuSpecialAttributeBase.h"

// シズクの氷属性必殺技を担当するクラス。
class SizukuSpecialIce final : public SizukuSpecialAttributeBase {
public:
	void Start(SizukuSpecial& special) override;
	void Update(SizukuSpecial& special, float deltaTime) override;
};
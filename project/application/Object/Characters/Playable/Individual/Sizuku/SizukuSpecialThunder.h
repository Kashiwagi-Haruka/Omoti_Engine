#include "SizukuSpecialAttributeBase.h"

// シズクの雷属性必殺技を担当するクラス。
class SizukuSpecialThunder final : public SizukuSpecialAttributeBase {
public:
	void Start(SizukuSpecial& special) override;
	void Update(SizukuSpecial& special, float deltaTime) override;
};
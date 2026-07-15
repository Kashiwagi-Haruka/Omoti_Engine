enum class EquipmentTypes{
	HEAD,//頭
	NECK,//首
	CHEST,//胸
	PANTS,//ズボン
	SHOES,//靴
};

enum class EquipUpParametersType{ 
	//HP実数値
	HPRealValue,
	//HPパーセント
	HPPercent,
	//攻撃実数値
	AttackRealValue,
	//攻撃パーセント
	AttackPercent,
	//防御実数値
	DiffenceRealValue,
	//防御パーセント
	DiffencePercent,
	// クリティカル率
	CriticalRate,
	// クリティカルダメージ
	CriticalDamage,
	// 属性親和度
	AttributeAffinity,

};
struct EquipParameters{
	EquipUpParametersType parameter;
	int upgreatCount = 0;
};
class BaseEquipment{
	EquipmentTypes equipmentType_;
	EquipParameters parameterFirst_;
	EquipParameters parameterSecond_;
	EquipParameters parameterThird_;
	EquipParameters parameterFour_;
};
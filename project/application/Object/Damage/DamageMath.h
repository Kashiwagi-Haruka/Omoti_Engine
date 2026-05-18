#pragma once
class DamageMath {

	int MaxDamage_ = 99999999;

	int damage_ = 0;

	public:
	void ResetDamage() { damage_ = 0; }
	void AddDamage(int amount);
	void SubtractDamage(int amount);
	
};

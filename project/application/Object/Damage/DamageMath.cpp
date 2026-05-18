#include "DamageMath.h"

void DamageMath::AddDamage(int amount) {
	damage_ += amount;
	if (damage_ > MaxDamage_) {
		damage_ = MaxDamage_;
	}
}
void DamageMath::SubtractDamage(int amount) {
	damage_ -= amount;
	if (damage_ < 0) {
		damage_ = 0;
	}
}
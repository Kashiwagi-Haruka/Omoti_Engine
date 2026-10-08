#include "SizukuSpecialThunderHitEffect.h"

SizukuSpecialThunderHitEffect::SizukuSpecialThunderHitEffect() { 
	for (size_t i = 0; i < thunderHitEffects_.size(); ++i) {
		thunderHitEffects_[i] = std::make_unique<Primitive>();	
	}
}
void SizukuSpecialThunderHitEffect::Initialize() {
	for (size_t i = 0; i < thunderHitEffects_.size(); ++i) {
		thunderHitEffects_[i]->Initialize(Primitive::PrimitiveName::Plane,"Resources/3d/Character/Sizuku/Special/Thunder/sizukuSpecialHitEffect");
	}
}
void SizukuSpecialThunderHitEffect::Update() { 
	for (int i = 0; i < thunderHitEffects_.size(); ++i) { 
		thunderHitEffects_[i]->Update(); 
	}
}
void SizukuSpecialThunderHitEffect::Draw() {
	for (size_t i = 0; i < thunderHitEffects_.size(); ++i) {
		thunderHitEffects_[i]->Draw();
	}
}
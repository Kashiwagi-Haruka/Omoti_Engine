#pragma once
#include "Primitive/Primitive.h"
#include <array>
#include <memory>
class SizukuSpecialThunderHitEffect {

	std::array<std::unique_ptr<Primitive>, 2> thunderHitEffects_;

	std::array<Transform, 2> HitTransforms_{};


	public:

	SizukuSpecialThunderHitEffect();
	~SizukuSpecialThunderHitEffect();
	void Initialize();
	void Update();
	void Draw();
	void SetTransform(const Transform& transform) {
		for (int i = 0; i < thunderHitEffects_.size(); ++i) {
			thunderHitEffects_[i]->SetTransform(transform);
		}
	}

};

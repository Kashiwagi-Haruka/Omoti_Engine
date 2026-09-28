#pragma once
#include "Engine/Texture/Mesh/Object3d/Object3d.h"
#include "Engine/Texture/Mesh/Primitive/Primitive.h"
#include "Object/Characters/Base/Attribute.h"
#include "ParticleEmitter.h"
#include "SizukuSpecialAttributeBase.h"
#include "Transform.h"
#include <memory>
#include <random>
#include <vector>

class Camera;
class SizukuSpecialFire;
class SizukuSpecialIce;
class SizukuSpecialWind;
class SizukuSpecialThunder;
class SizukuSpecialImaginary;
class SizukuSpecialQuantum;

class SizukuSpecial {
	friend class SizukuSpecialFire;
	friend class SizukuSpecialIce;
	friend class SizukuSpecialWind;
	friend class SizukuSpecialThunder;
	friend class SizukuSpecialImaginary;
	friend class SizukuSpecialQuantum;
	bool isStarted_ = false;
	bool isEnd_ = false;
	bool fireworkEmitted_ = false;
	bool attributeEffectEmitted_ = false;
	float elapsedTime_ = 0.0f;
	float animationTime_ = 0.0f;
	float animationTimeMax_ = 5.0f;
	float sizukuHeight_ = 2.0f;
	int damageId_ = 0;
	int rainDamageId_ = 0;
	Attribute attribute_ = Attribute::Ice;

	std::unique_ptr<Primitive> fieldPlane_;
	std::unique_ptr<Primitive> imaginaryFieldPlane_;
	std::unique_ptr<Object3d> skydomeObj_;
	std::unique_ptr<Object3d> iceFlower_;
	std::unique_ptr<Object3d> thunderProjectile_;
	std::vector<std::unique_ptr<Object3d>> iceRains_;
	std::vector<Transform> iceRainTransforms_;
	std::unique_ptr<ParticleEmitter> mainEmitter_;
	std::unique_ptr<ParticleEmitter> subEmitter_;
	std::unique_ptr<SizukuSpecialAttributeBase> fireSpecial_;
	std::unique_ptr<SizukuSpecialAttributeBase> iceSpecial_;
	std::unique_ptr<SizukuSpecialAttributeBase> windSpecial_;
	std::unique_ptr<SizukuSpecialAttributeBase> thunderSpecial_;
	std::unique_ptr<SizukuSpecialAttributeBase> imaginarySpecial_;
	std::unique_ptr<SizukuSpecialAttributeBase> quantumSpecial_;
	SizukuSpecialAttributeBase* activeSpecial_ = nullptr;
	Transform sizukuTransform_{};
	Transform fieldPlaneTransform_{};
	Transform imaginaryFieldPlaneTransform_{};
	Transform skydomeTransform_{};
	Transform iceFlowerTransform_{};
	Transform thunderProjectileTransform_{};
	Transform particleTransform_{};
	Vector3 damagePosition_{};
	Vector3 damageScale_{};
	Camera* camera_ = nullptr;
	std::mt19937 randomEngine_{std::random_device{}()};

	// 各属性の実装は SizukuSpecial<属性名>.cpp に分離する。
	void StartFireSpecial();
	void StartIceSpecial();
	void StartWindSpecial();
	void StartThunderSpecial();
	void StartImaginarySpecial();
	void StartQuantumSpecial();
	void UpdateFireSpecial(float deltaTime);
	void UpdateIceSpecial(float deltaTime);
	void UpdateWindSpecial(float deltaTime);
	void UpdateThunderSpecial(float deltaTime);
	void UpdateImaginarySpecial(float deltaTime);
	void UpdateQuantumSpecial(float deltaTime);
	void DrawIceSpecial();
	void DrawImaginarySpecial();
	void DrawParticleSpecial();
	void ResetIceRain(size_t index, bool randomizeHeight);
	void ConfigureEmitter(ParticleEmitter& emitter, const Vector4& color, uint32_t count, float speed, float life);

public:
	SizukuSpecial();
	void Initialize();
	void Update();
	void Draw();
	void Start();
	void End();

	void SetCamera(Camera* camera) { camera_ = camera; }
	void SetSizukuTransform(Transform transform) { sizukuTransform_ = transform; }
	void SetSizukuHeight(float height) { sizukuHeight_ = height; }
	void SetAttribute(Attribute attribute) { attribute_ = attribute; }
	Attribute GetAttribute() const { return attribute_; }
	bool isEnd() const { return isEnd_; }
	bool IsAnimationFinished() const { return animationTime_ >= animationTimeMax_; }
	bool IsFlowerDamaging() const;
	bool IsRainDamaging() const;
	Vector3 GetDamagePosition() const { return damagePosition_; }
	Vector3 GetDamageScale() const { return damageScale_; }
	int GetDamageId() const { return damageId_; }
	const std::vector<Transform>& GetRainDamageTransforms() const { return iceRainTransforms_; }
	Vector3 GetRainDamageScale() const { return {0.75f, 1.25f, 0.75f}; }
	int GetRainDamageId() const { return rainDamageId_; }
};
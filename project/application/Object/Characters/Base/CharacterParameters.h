#pragma once
#include <array>
#include "Object/Characters/Base/Attribute.h"
struct BaseParameter {
	// 基礎体力
	float HP; 
	// 基礎攻撃力
	float Attack; 
	// 基礎防御力
	float Deffence; 
};
struct Parameter {
	// キャラクターのレベル
	int level;
	// キャラクターのHP
	float HP;
	// キャラクターの攻撃力
	float Attack;
	// キャラクターの防御力
	float Deffence;
	// キャラクターの移動速度
	float Speed;
	// キャラクターのクリティカル率
	float CriticalRate;
	// キャラクターのクリティカルダメージ
	float CriticalDamage;
	// キャラクターの属性親和度
	float AttributeAffinity = 1.0f;
	// キャラクターの属性ダメージ率
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeDamageRate{};
	// キャラクターの属性耐性率
	std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)> AttributeResistanceRate{};
};

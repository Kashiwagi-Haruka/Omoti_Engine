#include "CharacterDisplayEquip.h"
#include "Function.h"
#include "Object3d/Object3dCommon.h"
#include <cmath>
#include <numbers>
void CharacterDisplayEquip::Initialize() { 

	for (int i = 0; i < 5; i++) {
		viewEquipment_[i] = std::make_unique<Primitive>();
		viewEquipment_[i]->Initialize(Primitive::PrimitiveName::Circle);
		viewEquipment_[i]->SetEnableLighting(false);
		viewEquipmentTransform_[i].translate = {std::sinf(std::numbers::pi_v<float> * (1.0f*i / 5.0f)) * distance_, 0.0f, std::cosf(std::numbers::pi_v<float> * (1.0f*i / 5.0f)) * distance_};
	}



}
void CharacterDisplayEquip::Update() {
	for (int i = 0; i < 5; i++) {
		viewEquipment_[i]->SetTranslate(viewEquipmentTransform_[i].translate + centerTranslate_);
		viewEquipment_[i]->Update();
	}

}
void CharacterDisplayEquip::Draw() {
	Object3dCommon::GetInstance()->DrawCommon();
	for (int i = 0; i < 5; i++) {
		viewEquipment_[i]->Draw();
	}
}
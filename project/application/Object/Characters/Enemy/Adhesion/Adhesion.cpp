#include "Adhesion.h"
Adhesion::Adhesion() { 
	preAttributePlane_ = std::make_unique<Primitive>(); 
	AttributePlane_ = std::make_unique<Primitive>();
}

void Adhesion::Initialize() {
	
	preAttributePlane_->Initialize(Primitive::PrimitiveName::Plane);
	preAttributePlane_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	AttributePlane_->Initialize(Primitive::PrimitiveName::Plane);
	AttributePlane_->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
}
#include "Particles.h"
#include "Function.h"
#include "ParticleManager.h"
#include "imgui.h"

Particles::Particles() {

	ParticleManager::GetInstance()->CreateParticleGroup("player", "Resources/2d/defaultParticle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("leaf", "Resources/2d/leaf.png");
	ParticleManager::GetInstance()->CreateParticleGroup("goal", "Resources/2d/goalParticle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("screenEffect", "Resources/2d/defaultParticle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("Arrow", "Resources/2d/ArrowParticle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("skill", "Resources/2d/ArrowParticle.png");
	ParticleManager::GetInstance()->CreateParticleGroup("playerSwitch", "Resources/2d/defaultParticle.png");

	particleArrow = std::make_unique<ParticleEmitter>("Arrow");
	particleArrow->SetTransform(
	    Transform{
	        {1,  1, 1 },
	        {0,  0, 0 },
	        {25, 0, 25},
    });
	particleArrow->SetFrequency(1.0f);
	particleArrow->SetCount(5);
	particleArrow->SetAcceleration(Vector3{0, 0, 0});
	particleArrow->SetAreaMin(Vector3{-1, -1, -1});
	particleArrow->SetAreaMax(Vector3{1, 1, 1});
	particleSwitch = std::make_unique<ParticleEmitter>("playerSwitch");
	particleSwitch->SetTransform(
	    Transform{
	        {0.3f, 0.3f, 0.3f},
            {0,    0,    0   },
            {0,    0,    0   }
    });
	particleSwitch->SetFrequency(0.0f);
	particleSwitch->SetCount(18);
	particleSwitch->SetAcceleration(Vector3{0, 0.02f, 0});
	particleSwitch->SetAreaMin(Vector3{-0.35f, -0.2f, -0.35f});
	particleSwitch->SetAreaMax(Vector3{0.35f, 0.4f, 0.35f});
	isgoal = false;
}

Particles::~Particles() { ParticleManager::GetInstance()->Clear(); }

constexpr float kParticlePosScale = 5.0f;

void Particles::Update() {

	playerEmitterTransform.scale = {0.1f, 0.1f, 1.0f};
	playerEmitterTransform.rotate = {0, 0, 0};

	playerEmitterTransform.translate = {playerPos_.x * kParticlePosScale, (playerPos_.y - 0.9f) * kParticlePosScale, playerPos_.z};
	if (particleArrow) {
		particleArrow->Update(playerEmitterTransform);
	}
}
void Particles::Draw() {

	if (particleArrow) {
		particleArrow->Draw();
	}
	if (particleSwitch) {
		particleSwitch->Draw();
	}
}
void Particles::SetPlayerPos(Vector3 playerPos) { playerPos_ = playerPos; }
void Particles::SetCameraPos(Vector3 cameraPos) { cameraPos_ = cameraPos; }
void Particles::SetGoalPos(Vector3 goalPos) { goalPos_ = goalPos; }
void Particles::EditSingleEmitter(ParticleEmitter* e) {
#ifdef USE_IMGUI
	ImGui::PushID(e);

	// ============================================
	//  Transform Editor
	// ============================================
	if (ImGui::TreeNode("Transform")) {

		Transform t = e->GetTransformRef();

		// --- Scale ---
		ImGui::Text("Scale");
		ImGui::DragFloat3("##scale", &t.scale.x, 0.01f);

		// --- Rotate ---
		ImGui::Text("Rotate");
		ImGui::DragFloat3("##rotate", &t.rotate.x, 0.01f);

		// --- Translate ---
		ImGui::Text("Translate");
		ImGui::DragFloat3("##translate", &t.translate.x, 0.01f);

		ImGui::TreePop();
	}

	// ============================================
	//  Frequency
	// ============================================
	float freq = e->GetFrequency();
	if (ImGui::DragFloat("Frequency##freq", &freq, 0.01f, 0.0f, 10.0f)) {
		e->SetFrequency(freq);
	}

	// ============================================
	//  Emit Count
	// ============================================
	int cnt = (int)e->GetCount();
	if (ImGui::DragInt("Emit Count##cnt", &cnt, 1, 1, 1000)) {
		e->SetCount((uint32_t)cnt);
	}

	// ============================================
	//  Acceleration
	// ============================================
	Vector3 acc = e->GetAcceleration();
	if (ImGui::DragFloat3("Acceleration##acc", &acc.x, 0.01f)) {
		e->SetAcceleration(acc);
	}

	// ============================================
	//  Area Min / Max
	// ============================================
	Vector3 amin = e->GetAreaMin();
	if (ImGui::DragFloat3("Area Min##amin", &amin.x, 0.01f)) {
		e->SetAreaMin(amin);
	}

	Vector3 amax = e->GetAreaMax();
	if (ImGui::DragFloat3("Area Max##amax", &amax.x, 0.01f)) {
		e->SetAreaMax(amax);
	}

	if (ImGui::Button("Emit Now##emit")) {
		e->Emit();
	}
	float life = e->GetLife();
	if (ImGui::DragFloat("Life##life", &life, 1.0f, 0.0f, 10000.0f)) {
		e->SetLife(life);
	}

	ImGui::PopID();
#endif // USE_IMGUI
}
void Particles::EmitPlayerSwitchEffect(const Vector3& playerPos) {
	if (!particleSwitch) {
		return;
	}

	Transform switchTransform = particleSwitch->GetTransformRef();
	switchTransform.translate = {playerPos.x * kParticlePosScale, (playerPos.y - 0.9f) * kParticlePosScale, playerPos.z};
	particleSwitch->SetTransform(switchTransform);
	particleSwitch->Emit();
}
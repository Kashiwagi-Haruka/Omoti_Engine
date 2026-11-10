#include "QuestUI.h"
#include "Object/Load/GameLoad.h"

QuestUI::QuestUI() { questBGSprite_ = std::make_unique<Sprite>(); }
QuestUI::~QuestUI() {}

void QuestUI::Initialize() {
	uint32_t handle = GameLoad::GetInstance()->GetTextureHandle("white2x2");
	questBGSprite_->Initialize(handle);
}
void QuestUI::Update() { questBGSprite_->Update(); }
void QuestUI::Draw() { questBGSprite_->Draw(); }
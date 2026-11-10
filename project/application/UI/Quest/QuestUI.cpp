#include "QuestUI.h"
#include "TextureManager.h"
QuestUI::QuestUI() { questBGSprite_ = std::make_unique<Sprite>(); }
QuestUI::~QuestUI() {}

void QuestUI::Initialize() {
	uint32_t handle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/white2x2.png");
	questBGSprite_->Initialize(handle);
}
void QuestUI::Update() { questBGSprite_->Update(); }
void QuestUI::Draw() { questBGSprite_->Draw(); }
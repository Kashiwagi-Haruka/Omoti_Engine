#include "Pause.h"
#include "GameBase.h"
#include "Data/Color.h"
#include "Engine/Texture/Mesh/Object3d/Object3dCommon.h"
#include "Engine/Texture/Sprite/SpriteCommon.h"
#include "Function.h"
#include "Input.h"
#include "PlayCommand/PlayCommand.h"
#include "Text/FreetypeManager/FreeTypeManager.h"
#include "TextureManager.h"
namespace {
constexpr float kTransitionSpeed = 0.06f;
constexpr float kHiddenOffsetX = 1280.0f;
static const int kMenuCount = 4;
std::u32string menuStrings[kMenuCount] = {U"再開", U"設定", U"ライセンス", U"タイトル"};
constexpr std::array<Pause::Action, kMenuCount> kMenuActions = {
    Pause::Action::kResume,
    Pause::Action::kOptions,
    Pause::Action::kLicense,
    Pause::Action::kTitle,
};
} // namespace

Pause::Pause() {
	const uint32_t selectTextureHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Pause/Select.png");

	BG_ = std::make_unique<Primitive>();
	camera_ = std::make_unique<Camera>();

	BG_->Initialize(Primitive::PrimitiveName::Plane, "Resources/2d/Pause/Pause.png");
	BG_->SetCamera(camera_.get());
	BG_->SetColor({0.9f,0.9f,0.9f,1.0f});
	selectRotation_[0] = 0.0f;
	selectRotation_[1] = 1.0f;
	selectRotation_[2] = 2.0f;
	selectScale_[0] = 160.0f;
	selectScale_[1] = 180.0f;
	selectScale_[2] = 200.0f;
	for (int i = 0; i < 3; ++i) {
		selectSprites_[i] = std::make_unique<Sprite>();
		selectSprites_[i]->Initialize(selectTextureHandle);
		selectSprites_[i]->SetScale({selectScale_[i], selectScale_[i]});
		selectSprites_[i]->SetAnchorPoint({0.5f, 0.5f});
		selectSprites_[i]->SetRotation(selectRotation_[i]);
	}
	selectSprites_[0]->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
	selectSprites_[1]->SetColor({0.0f, 1.0f, 0.0f, 0.5f});
	selectSprites_[2]->SetColor({0.0f, 0.0f, 1.0f, 0.5f});
	
	titleBackBG_ = std::make_unique<Sprite>();

}

void Pause::Initialize() {

	menuTexts_.clear();

	startTime = 0.0f;
	isPause_ = false;
	isStart_ = false;
	isEnd_ = false;
	isActive_ = false;
	isTitleBack_ = false;
	action_ = Action::kNone;
	selectIndex_ = 1;

	camera_->SetTransform({
	    .scale{1.0f, 1.0f, 1.0f },
	    .rotate{0.0f, 0.0f, 0.0f },
	    .translate{0.0f, 0.0f, -8.0f},
	});
	camera_->Update();

	BG_->SetTransform({
	    .scale{1280.0f * 0.1f, 720.0f*0.1f, 1.0f },
	    .rotate{0.0f,  0.0f,  0.0f},
	    .translate{0.0f,  0.0f,  10.0f},
	});
	BG_->SetEnableLighting(false);
	uint32_t titleBackHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/Pause/TitleBack.png");
	titleBackBG_->Initialize(titleBackHandle);
	titleBackBG_->SetPosition({GameBase::GetInstance()->GetScreenHalfWidth(), GameBase::GetInstance()->GetScreenHalfHeight()});
	titleBackBG_->SetAnchorPoint({0.5f, 0.5f});

	pauseFontHandle_ = FreeTypeManager::CreateFace("Resources/Font/JF-Dot-jiskan24-2000.ttf", 0);
	FreeTypeManager::SetPixelSizes(pauseFontHandle_, 72, 72);
	pauseText_.Initialize(pauseFontHandle_);
	pauseText_.SetSize({1280.0f, 200.0f});
	pauseText_.SetString(U"ポーズ");
	pauseText_.SetPosition({640.0f, 90.0f});
	pauseText_.SetAlign(TextAlign::Center);
	pauseText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	pauseText_.UpdateLayout(false);
	FreeTypeManager::SetPixelSizes(pauseFontHandle_, 48, 48);

	
	for (int i = 0; i < kMenuCount; ++i) {
		Text text;
		text.Initialize(pauseFontHandle_);
		text.SetSize({1280.0f, 100.0f});
		text.SetString(menuStrings[i]);
		text.SetPosition({640.0f, 200.0f + i * 100.0f});
		text.SetAlign(TextAlign::Center);
		text.SetColor({1.0f, 1.0f, 1.0f, 1.0f});
		text.UpdateLayout(false);
		menuTexts_.push_back(text);
	}
}

void Pause::Start() { 
	if (!isActive_ && !isStart_ ) {
		action_ = Action::kNone; 
		Object3dCommon::GetInstance()->SetFullScreenGrayscaleEnabled(false);
		isPause_ = true;
		startTime = 0.0f;
		isStart_ = true;
		isEnd_ = false;
	}
}
void Pause::End() {
	if (isActive_ && !isEnd_) {
		startTime = 0.0f;
		isEnd_ = true;
		isStart_ = false;
		Input::GetInstance()->SetIsCursorStability(true);
		Input::GetInstance()->SetIsCursorVisible(false);
	}
}

void Pause::Update() {


	if (!isPause_) {
		return;
	} else {
		if (PlayCommand::GetInstance()->GetPause()) {
			End();
		}	
	}
	float offsetX = kHiddenOffsetX;
	if (isStart_) {
		startTime += kTransitionSpeed;
		offsetX = Function::Lerp(kHiddenOffsetX, 0.0f, startTime);
		if (startTime >= 1.0f) {
			startTime = 1.0f;
			isStart_ = false;
			isActive_ = true;
			offsetX = 0.0f;
			Input::GetInstance()->SetIsCursorStability(false);
			Input::GetInstance()->SetIsCursorVisible(true);
		}
	} else if (isEnd_) {
		startTime += kTransitionSpeed;
		offsetX = Function::Lerp(0.0f, kHiddenOffsetX, startTime);
		if (startTime >= 1.0f) {
			startTime = 1.0f;
			isPause_ = false;
			isEnd_ = false;
			isActive_ = false;
			offsetX = kHiddenOffsetX;
		}
	} else if (isActive_) {
		offsetX = 0.0f;
		Input::GetInstance()->SetIsCursorStability(false);
		Input::GetInstance()->SetIsCursorVisible(true);
	}

	if (isActive_) {
		
		if (PlayCommand::GetInstance()->GetUpUI() && !PlayCommand::GetInstance()->GetDownUI()) {
			selectIndex_ = (selectIndex_ + kMenuCount - 1) % kMenuCount;
		} else if (PlayCommand::GetInstance()->GetDownUI() && !PlayCommand::GetInstance()->GetUpUI()) {
			selectIndex_ = (selectIndex_ + 1) % kMenuCount;
		}

		if (PlayCommand::GetInstance()->GetDESIDE()) {
			action_ = kMenuActions[selectIndex_];
		} else if (PlayCommand::GetInstance()->GetESCAPE()) {
			action_ = Action::kResume;
		}
		switch (action_) {
		case Pause::Action::kNone:
			break;
		case Pause::Action::kResume:
			break;
		case Pause::Action::kOptions:
			break;
		case Pause::Action::kLicense:
			break;
		case Pause::Action::kTitle:
			if (isTitleBackVisit_) {
				isTitleBack_ = true;
				isTitleBackVisit_ = false;
				break;
			}
			isTitleBackVisit_ = true;
			break;
		default:
			break;
		}
	}
	SelectSpriteUpdate();
	if (!menuTexts_.empty()) {
		const Vector2 selectPosition = {menuTexts_[selectIndex_].GetPosition().x + offsetX, menuTexts_[selectIndex_].GetPosition().y};
		for (auto& selectSprite : selectSprites_) {
			selectSprite->SetPosition(selectPosition);
			selectSprite->Update();
		}
	}
	CurrentCharacterUpdate();
	BG_->Update();
	pauseText_.Update(false);
	for (auto& menuText : menuTexts_) {
		menuText.Update(false);
	}
}
void Pause::Draw() {

	if (!isPause_) {
		return;
	}
	Object3dCommon::GetInstance()->SetFullScreenGrayscaleEnabled(false);
	camera_->Update();
	BG_->SetCamera(camera_.get());

	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	Camera* previousDefaultCamera = object3dCommon->GetDefaultCamera();
	object3dCommon->SetDefaultCamera(camera_.get());
	if (currentCharacterObj_) {
		currentCharacterObj_->SetCamera(camera_.get());
		currentCharacterObj_->SetTransform({
		    .scale{2.0f,  2.0f, 2.0f},
		    .rotate{0.0f,  0.2f, 0.0f},
		    .translate{-2.0f, 0.0f, 0.0f},
		});
		currentCharacterObj_->Update();
	}

	object3dCommon->DrawCommon();
	BG_->Draw();
	if (currentCharacterObj_) {
		object3dCommon->DrawCommon(Object3dCommon::DrawCommonType::MaterialColorOnlySkinning);
		currentCharacterObj_->Draw();
	}

	pauseText_.Draw();
	for (auto& menuText : menuTexts_) {
		menuText.Draw();
	}
	SpriteCommon::GetInstance()->DrawCommon();
	SpriteCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAdd);
	for (int i = 0; i < 3; ++i) {
		selectSprites_[i]->Draw();
	}
	SpriteCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);

	object3dCommon->SetDefaultCamera(previousDefaultCamera);
}

void Pause::CurrentCharacterUpdate() {
	if (currentCharacterObj_) {
		switch (currentAttribute) {
		case Attribute::None:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(190, 190, 190, 255));
			break;
		case Attribute::Fire:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(225, 75, 65, 255));
			break;
		case Attribute::Ice:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(130, 190, 220, 255));
			break;
		case Attribute::Wind:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(130, 235, 170, 255));
			break;
		case Attribute::Thunder:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(200, 100, 200, 255));
			break;
		case Attribute::Imaginary:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(240, 210, 60, 255));
			break;
		case Attribute::Quantum:
			currentCharacterObj_->SetColor(Color::RGBAToVector4(55, 80, 160, 255));
			break;
		default:
			break;
		}
	}
}
void Pause::SelectSpriteUpdate() {
	for (int i = 0; i < 3; ++i) {
		selectRotation_[i] += 0.025f;
		selectScale_[i] += 1.0f * std::cos(selectRotation_[i]);
		selectSprites_[i]->SetRotation(std::sinf(selectRotation_[i]));
		selectSprites_[i]->SetScale({selectScale_[i], selectScale_[i]});
	}
}

bool Pause::IsTitleBack()const{

	return isTitleBack_;
}
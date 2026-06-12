#include "Pause.h"
#include "Data/Color.h"
#include "Engine/Texture/Mesh/Object3d/Object3dCommon.h"
#include "Engine/Texture/Sprite/SpriteCommon.h"
#include "Function.h"
#include "Input.h"
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
}

void Pause::Initialize() {

	menuTexts_.clear();

	startTime = 0.0f;
	isStart_ = false;
	isEnd_ = false;
	isActive_ = false;
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

Pause::Action Pause::ConsumeAction() {
	Action action = action_;
	action_ = Action::kNone;
	return action;
}

void Pause::Update(bool isPause) {

	action_ = Action::kNone;

	if (isPause && !isActive_ && !isStart_) {
		startTime = 0.0f;
		isStart_ = true;
		isEnd_ = false;
	}

	if (!isPause && isActive_ && !isEnd_) {
		startTime = 0.0f;
		isEnd_ = true;
		isStart_ = false;
		Input::GetInstance()->SetIsCursorStability(true);
		Input::GetInstance()->SetIsCursorVisible(false);
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
		bool moveUp = Input::GetInstance()->TriggerKey(DIK_W) || Input::GetInstance()->TriggerKey(DIK_UP) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonUp);
		bool moveDown = Input::GetInstance()->TriggerKey(DIK_S) || Input::GetInstance()->TriggerKey(DIK_DOWN) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonDown);

		if (moveUp && !moveDown) {
			selectIndex_ = (selectIndex_ + kMenuCount - 1) % kMenuCount;
		} else if (moveDown && !moveUp) {
			selectIndex_ = (selectIndex_ + 1) % kMenuCount;
		}

		bool confirm = Input::GetInstance()->TriggerKey(DIK_RETURN) || Input::GetInstance()->TriggerKey(DIK_SPACE) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonA);
		bool cancel = Input::GetInstance()->TriggerKey(DIK_ESCAPE) || Input::GetInstance()->TriggerButton(Input::PadButton::kButtonStart) ||
		              Input::GetInstance()->TriggerButton(Input::PadButton::kButtonB);

		if (confirm) {
			action_ = kMenuActions[selectIndex_];
		} else if (cancel) {
			action_ = Action::kResume;
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

	if (!IsVisible()) {
		return;
	}
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
		object3dCommon->DrawCommonMaterialColorOnlySkinning();
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
#define NOMINMAX
#include "TutorialScene.h"
#include "GameBase.h"
#include "AudioManager/BGMManager/BGMManager.h"
#include "CameraController/CameraController.h"
#include "Input.h"
#include "Object/Background/Sky.h"
#include "Object/Field/Field.h"
#include "Object/Player/Player.h"
#include "Object3d/Object3dCommon.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "Sprite/SpriteCommon.h"
#include "Text/FreetypeManager/FreeTypeManager.h"
#include "TextureManager.h"
#include "TutorialUI/TutorialUI.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>

namespace {
constexpr float kPi = 3.14159265358979323846f;
constexpr float kAttributeIconDistance = 150.0f;
constexpr float kAttributeIconSize = 64.0f;
constexpr float kAttributeStickDeadZone = 0.45f;

constexpr std::array<Attribute, 6> kAttributes = {
    Attribute::Fire, Attribute::Ice, Attribute::Wind, Attribute::Thunder, Attribute::Imaginary, Attribute::Quantum,
};

constexpr std::array<const char*, 6> kAttributeTexturePaths = {
    "Resources/2d/Attribute/Fire.png",    "Resources/2d/Attribute/Ice.png",       "Resources/2d/Attribute/Wind.png",
    "Resources/2d/Attribute/Thunder.png", "Resources/2d/Attribute/Imaginary.png", "Resources/2d/Attribute/Quantum.png",
};

std::u32string ToText(int value) {
	const std::string text = std::to_string(value);
	return std::u32string(text.begin(), text.end());
}
} // namespace

TutorialScene::TutorialScene() = default;
TutorialScene::~TutorialScene() = default;

TutorialScene::TutorialStep TutorialScene::GetCurrentStep() const { return static_cast<TutorialStep>(currentStepIndex_); }

void TutorialScene::Initialize() {
	cameraController_ = std::make_unique<CameraController>();
	skyDome_ = std::make_unique<Sky>();
	player_ = std::make_unique<Player>();
	field_ = std::make_unique<Field>();
	tutorialUI_ = std::make_unique<TutorialUI>();

	cameraController_->Initialize();

	auto* camera = cameraController_->GetCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(camera);

	skyDome_->Initialize(camera);
	player_->Initialize(camera);

	// 属性変更に対応しているキャラクターを使う。
	player_->SetCharacterType("Sizuku");

	field_->Initialize(camera);
	tutorialUI_->Initialize();

	const auto controlHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/option.png");

	controlSprite_ = std::make_unique<Sprite>();
	controlSprite_->Initialize(controlHandle);
	controlSprite_->SetScale({700.0f, 100.0f});
	controlSprite_->SetPosition({10.0f, 600.0f});
	controlSprite_->Update();

	const auto fontHandle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(fontHandle, 24, 24);

	guideText_.Initialize(fontHandle);
	guideText_.SetSize({580.0f, 260.0f});
	guideText_.SetPosition({
	    GameBase::GetInstance()->GetScreenWidth() - 600.0f,
	    120.0f,
	});
	guideText_.SetAlign(TextAlign::Left);
	guideText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	InitializeAttributeMenu();

	currentStepIndex_ = static_cast<int>(TutorialStep::kMove);
	skillUseCount_ = 0;
	isTutorialComplete_ = false;
	isPaused_ = false;
	wasSkipKeyHeld_ = false;
	pauseOpened_ = false;
	isAttributeMenuOpen_ = false;
	skipHoldTimer_ = 0.0f;
	displayedGuide_.clear();

	BeginStep();
	UpdateGuide();

	// 既存の説明画像は使わず、進捗バーなどだけ表示する。
	tutorialUI_->Update(0.0f, 0.0f, -1, false);

	Input::GetInstance()->SetIsCursorStability(true);
	Input::GetInstance()->SetIsCursorVisible(false);
}

void TutorialScene::InitializeAttributeMenu() {
	const Vector2 center{
	    GameBase::GetInstance()->GetScreenWidth() * 0.5f,
	    GameBase::GetInstance()->GetScreenHeight() * 0.5f,
	};

	selectedAttributeIndex_ = 1;

	for (int i = 0; i < kAttributeCount; ++i) {
		attributeSprites_[i] = std::make_unique<Sprite>();

		const auto handle = TextureManager::GetInstance()->GetTextureIndexByfilePath(kAttributeTexturePaths[i]);

		auto& sprite = attributeSprites_[i];
		sprite->Initialize(handle);

		const float angle = 2.0f * kPi * static_cast<float>(i) / static_cast<float>(kAttributeCount);

		sprite->SetPosition({
		    center.x + std::sin(angle) * kAttributeIconDistance,
		    center.y - std::cos(angle) * kAttributeIconDistance,
		});

		sprite->SetAnchorPoint({0.5f, 0.5f});
		sprite->SetScale({kAttributeIconSize, kAttributeIconSize});
		sprite->Update();
	}
}

void TutorialScene::BeginStep() {
	stepTimer_ = 0.0f;
	stepActionTimer_ = 0.0f;
	currentStepProgress_ = 0.0f;

	switch (GetCurrentStep()) {
	case TutorialStep::kSkill:
		skillUseCount_ = 0;
		break;

	case TutorialStep::kAttributeChange:
		// このステップに入った時点の属性と比較する。
		stepStartAttribute_ = player_->GetCurrentAttribute();

		for (int i = 0; i < kAttributeCount; ++i) {
			if (kAttributes[i] == stepStartAttribute_) {
				selectedAttributeIndex_ = i;
				break;
			}
		}
		break;

	case TutorialStep::kPause:
		pauseOpened_ = false;
		break;

	default:
		break;
	}
}

void TutorialScene::AdvanceStep() {
	if (GetCurrentStep() == TutorialStep::kComplete) {
		currentStepProgress_ = 1.0f;
		isTutorialComplete_ = true;
		return;
	}

	++currentStepIndex_;
	BeginStep();
}

void TutorialScene::UpdateAttributeMenu() {
	if (!isAttributeMenuOpen_) {
		return;
	}

	auto* input = Input::GetInstance();
	bool hasSelectionInput = false;

	const auto stick = input->GetJoyStickLXY();
	const float stickLengthSquared = stick.x * stick.x + stick.y * stick.y;

	if (stickLengthSquared >= kAttributeStickDeadZone * kAttributeStickDeadZone) {
		float angle = std::atan2(stick.x, stick.y);

		if (angle < 0.0f) {
			angle += 2.0f * kPi;
		}

		const float sector = 2.0f * kPi / static_cast<float>(kAttributeCount);

		selectedAttributeIndex_ = static_cast<int>((angle + sector * 0.5f) / sector) % kAttributeCount;

		hasSelectionInput = true;
	}

	const Vector2 mousePosition{
	    input->GetMouseX(),
	    input->GetMouseY(),
	};

	for (int i = 0; i < kAttributeCount; ++i) {
		const auto iconPosition = attributeSprites_[i]->GetPosition();
		const float halfSize = kAttributeIconSize * 0.5f;

		if (std::abs(mousePosition.x - iconPosition.x) <= halfSize && std::abs(mousePosition.y - iconPosition.y) <= halfSize) {
			selectedAttributeIndex_ = i;
			hasSelectionInput = true;
			break;
		}
	}

	if (hasSelectionInput) {
		player_->SetCurrentAttribute(kAttributes[selectedAttributeIndex_]);
	}

	for (int i = 0; i < kAttributeCount; ++i) {
		const bool selected = i == selectedAttributeIndex_;
		const float size = selected ? kAttributeIconSize * 1.25f : kAttributeIconSize;

		attributeSprites_[i]->SetScale({size, size});
		attributeSprites_[i]->SetColor(selected ? Vector4{1.0f, 1.0f, 1.0f, 1.0f} : Vector4{0.65f, 0.65f, 0.65f, 0.85f});

		attributeSprites_[i]->Update();
	}
}

void TutorialScene::UpdateTutorial(float deltaTime, bool skillStarted, bool specialStarted) {
	auto* input = Input::GetInstance();
	bool progressed = false;

	switch (GetCurrentStep()) {
	case TutorialStep::kMove: {
		const auto stick = input->GetJoyStickLXY();

		const bool moving = input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || stick.x != 0.0f || stick.y != 0.0f;

		if (moving) {
			stepActionTimer_ += deltaTime;
		}

		currentStepProgress_ = stepActionTimer_ / kMoveActionDuration;
		progressed = stepActionTimer_ >= kMoveActionDuration;
		break;
	}

	case TutorialStep::kLook: {
		const auto mouse = input->GetMouseMove();
		const auto stick = input->GetJoyStickRXY();

		const bool altHeld = input->PushKey(DIK_LMENU) || input->PushKey(DIK_RMENU);

		const bool looking = (!altHeld && (mouse.x != 0.0f || mouse.y != 0.0f)) || stick.x != 0.0f || stick.y != 0.0f;

		if (looking) {
			stepActionTimer_ += deltaTime;
		}

		currentStepProgress_ = stepActionTimer_ / kLookActionDuration;
		progressed = stepActionTimer_ >= kLookActionDuration;
		break;
	}

	case TutorialStep::kAttack:
		progressed = player_->GetComboStep() > 0;
		currentStepProgress_ = progressed ? 1.0f : 0.0f;
		break;

	case TutorialStep::kSkill:
		if (skillStarted) {
			++skillUseCount_;
		}

		currentStepProgress_ = static_cast<float>(skillUseCount_) / static_cast<float>(kSkillUseTargetCount);

		progressed = skillUseCount_ >= kSkillUseTargetCount;
		break;

	case TutorialStep::kSpecial:
		// キー入力だけでなく、実際に発動したことを判定。
		progressed = specialStarted;
		currentStepProgress_ = progressed ? 1.0f : 0.0f;
		break;

	case TutorialStep::kAttributeChange:
		progressed = player_->GetCurrentAttribute() != stepStartAttribute_;
		currentStepProgress_ = progressed ? 1.0f : 0.0f;
		break;

	case TutorialStep::kPause:
		// ポーズ中はこの関数を呼ばない。
		// ポーズを開いてから再開すると達成。
		progressed = pauseOpened_;
		currentStepProgress_ = progressed ? 1.0f : 0.0f;
		break;

	case TutorialStep::kComplete:
		stepTimer_ += deltaTime;
		currentStepProgress_ = stepTimer_ / kCompleteDuration;
		progressed = stepTimer_ >= kCompleteDuration;
		break;
	}

	currentStepProgress_ = std::clamp(currentStepProgress_, 0.0f, 1.0f);

	if (progressed) {
		AdvanceStep();
	}
}

void TutorialScene::UpdateGuide() {
	std::u32string text = U"チュートリアル " + ToText(currentStepIndex_ + 1) + U" / " + ToText(kStepCount) + U"\n";

	switch (GetCurrentStep()) {
	case TutorialStep::kMove:
		text += U"WASD / 左スティックで移動しよう\n";
		text += U"合計2秒間、移動してみよう";
		break;

	case TutorialStep::kLook:
		text += U"マウス / 右スティックで視点を動かそう\n";
		text += U"合計2秒間、視点を動かしてみよう";
		break;

	case TutorialStep::kAttack:
		text += U"左クリック / Xボタンで攻撃しよう";
		break;

	case TutorialStep::kSkill:
		text += U"E / RBでスキルを3回使おう\n";
		text += U"発動回数 " + ToText(skillUseCount_) + U" / " + ToText(kSkillUseTargetCount);
		break;

	case TutorialStep::kSpecial:
		text += U"Q / Yボタンで必殺技を使おう";

		if (player_->GetSpecialAttackCooldownRemaining() > 0.0f) {
			text += U"\nクールタイムが終わるまで待とう";
		}
		break;

	case TutorialStep::kAttributeChange:
		text += U"今と違う属性に変更しよう\n";
		text += U"Yキーを押しながらマウスで選択\n";
		text += U"パッド：LTを押しながら左スティック";
		break;

	case TutorialStep::kPause:
		text += pauseOpened_ ? U"もう一度押してゲームを再開しよう" : U"Esc / STARTでポーズしてみよう";
		break;

	case TutorialStep::kComplete:
		text += U"チュートリアル完了！\n";
		text += U"まもなくゲームを開始します";
		break;
	}

	if (isPaused_) {
		text += U"\nポーズ中：Esc / START / Pで再開";
	}

	text += U"\nPを1秒長押しでスキップ";

	if (text != displayedGuide_) {
		displayedGuide_ = text;
		guideText_.SetString(text);
		guideText_.UpdateLayout(false);
	}

	guideText_.Update(false);
}

void TutorialScene::Update() {
	constexpr float deltaTime = 1.0f / 60.0f;
	auto* input = Input::GetInstance();

	BGMManager::GetInstance()->Play(BGMManager::BGMType::Tutorial);

	const bool skipKeyHeld = input->PushKey(DIK_P);

	bool togglePause = input->TriggerKey(DIK_ESCAPE) || input->TriggerButton(Input::PadButton::kButtonStart);

	if (skipKeyHeld) {
		skipHoldTimer_ += deltaTime;

		if (skipHoldTimer_ >= kSkipHoldDuration) {
			SceneManager::GetInstance()->ChangeScene("Game");
			return;
		}
	} else {
		if (wasSkipKeyHeld_ && skipHoldTimer_ < kSkipHoldDuration) {
			togglePause = true;
		}

		skipHoldTimer_ = 0.0f;
	}

	wasSkipKeyHeld_ = skipKeyHeld;

	if (togglePause) {
		isPaused_ = !isPaused_;

		if (GetCurrentStep() == TutorialStep::kPause && isPaused_) {
			pauseOpened_ = true;
		}
	}

	isAttributeMenuOpen_ = !isPaused_ && GetCurrentStep() == TutorialStep::kAttributeChange && player_->IsSizuku() && (input->PushKey(DIK_Y) || input->PushLeftTrigger());

	const bool altHeld = input->PushKey(DIK_LMENU) || input->PushKey(DIK_RMENU);

	const bool showCursor = isPaused_ || altHeld || isAttributeMenuOpen_;

	input->SetIsCursorStability(!showCursor);
	input->SetIsCursorVisible(showCursor);

	if (!isPaused_) {
		// 属性選択中はカメラ操作とプレイヤー操作を止める。
		if (!isAttributeMenuOpen_) {
			cameraController_->SetPlayerPos(player_->GetPosition());
			cameraController_->Update();
		}

		auto* camera = cameraController_->GetCamera();

		player_->SetCamera(camera);
		skyDome_->SetCamera(camera);
		field_->SetCamera(camera);

		skyDome_->Update();
		field_->Update();

		const bool wasSkillAttacking = player_->GetIsSkillAttack();
		const bool wasSpecialAttacking = player_->GetIsSpecialAttack();

		if (!isAttributeMenuOpen_) {
			player_->Update();
		}

		const bool skillStarted = !wasSkillAttacking && player_->GetIsSkillAttack();

		const bool specialStarted = !wasSpecialAttacking && player_->GetIsSpecialAttack();

		UpdateAttributeMenu();

		UpdateTutorial(deltaTime, skillStarted, specialStarted);

		// 属性変更達成後は選択メニューを閉じる。
		if (GetCurrentStep() != TutorialStep::kAttributeChange) {
			isAttributeMenuOpen_ = false;
			input->SetIsCursorStability(!altHeld);
			input->SetIsCursorVisible(altHeld);
		}

		if (isTutorialComplete_) {
			SceneManager::GetInstance()->ChangeScene("Game");
			return;
		}
	}

	const float tutorialProgress = (static_cast<float>(currentStepIndex_) + currentStepProgress_) / static_cast<float>(kStepCount);

	const float skipProgress = std::clamp(skipHoldTimer_ / kSkipHoldDuration, 0.0f, 1.0f);

	// -1 で古い説明画像を非表示にする。
	// 進捗バー・アイコン・スキップ表示はそのまま使う。
	tutorialUI_->Update(tutorialProgress, skipProgress, -1, skipHoldTimer_ > 0.0f);

	UpdateGuide();
}

void TutorialScene::Draw() {
	Object3dCommon::GetInstance()->DrawCommon();

	skyDome_->Draw();
	field_->Draw();
	player_->Draw();

	SpriteCommon::GetInstance()->DrawCommon();

	controlSprite_->Draw();
	tutorialUI_->Draw();

	if (isAttributeMenuOpen_) {
		for (const auto& sprite : attributeSprites_) {
			sprite->Draw();
		}
	}

	// フォント描画へ切り替わるので最後に描く。
	guideText_.Draw();
}

void TutorialScene::Finalize() {
	Input::GetInstance()->SetIsCursorStability(false);
	Input::GetInstance()->SetIsCursorVisible(true);
}
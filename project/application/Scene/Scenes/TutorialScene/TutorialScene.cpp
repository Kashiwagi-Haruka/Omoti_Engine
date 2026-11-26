#define NOMINMAX
#include "TutorialScene.h"

#include "AudioManager/BGMManager/BGMManager.h"
#include "CameraController/CameraController.h"
#include "GameBase.h"
#include "Input.h"
#include "Object/Background/Sky.h"
#include "Object/Field/Field.h"
#include "Object/Load/GameLoad.h"
#include "Object/Player/Player.h"
#include "Object3d/Object3dCommon.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "Sprite/SpriteCommon.h"
#include "Text/FreetypeManager/FreeTypeManager.h"
#include "TextureManager.h"
#include "TutorialUI/TutorialUI.h"
#include "UI/UIManager/UIManager.h"
#include "Object3d/Object3d.h"
#include "RigidBody.h"
#include "Object/Damage/Damage.h"
#include "Object/Damage/DamageMath.h"
#include <algorithm>
#include <array>
#include <cmath>
#include "Object/Characters/Enemy/Adhesion/Adhesion.h"
namespace {

constexpr float kPi = 3.14159265358979323846f;
constexpr float kAttributeIconDistance = 150.0f;
constexpr float kAttributeIconSize = 64.0f;
constexpr float kAttributeStickDeadZone = 0.45f;
constexpr BaseParameter kTrainingDummyBase{};
constexpr Parameter kTrainingDummyParameter{};
constexpr std::array<Attribute, 6> kAttributes = {
    Attribute::Fire, Attribute::Ice, Attribute::Wind, Attribute::Thunder, Attribute::Imaginary, Attribute::Quantum,
};

constexpr std::array<const char*, 6> kAttributeTexturePaths = {
    "Resources/2d/Attribute/Fire.png",    "Resources/2d/Attribute/Ice.png",       "Resources/2d/Attribute/Wind.png",
    "Resources/2d/Attribute/Thunder.png", "Resources/2d/Attribute/Imaginary.png", "Resources/2d/Attribute/Quantum.png",
};
constexpr Vector3 kTrainingDummyPosition{-5.0f, 1.5f, 1.0f};
constexpr Vector3 kTrainingDummyScale{1.5f, 1.5f, 1.5f};

AABB MakeTrainingAabb(const Vector3& center, const Vector3& halfSize) {
	return {
	    {center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z},
	    {center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z},
	};
}
} // namespace

TutorialScene::TutorialScene() = default;
TutorialScene::~TutorialScene() = default;

void TutorialScene::Initialize() {
	cameraController_ = std::make_unique<CameraController>();
	skyDome_ = std::make_unique<Sky>();
	player_ = std::make_unique<Player>();
	field_ = std::make_unique<Field>();
	tutorialUI_ = std::make_unique<TutorialUI>();
	uiManager_ = std::make_unique<UIManager>();

	cameraController_->Initialize();
	auto* camera = cameraController_->GetCamera();
	Object3dCommon::GetInstance()->SetDefaultCamera(camera);

	skyDome_->Initialize(camera);
	player_->Initialize(camera);
	player_->SetCharacterType("Sizuku");
	field_->Initialize(camera);

	trainingDummy_ = std::make_unique<Object3d>();
	trainingDummy_->Initialize();
	trainingDummy_->SetModel("Enemy");
	trainingDummy_->SetCamera(camera);
	trainingDummy_->SetTransform({
	    .scale = kTrainingDummyScale,
	    .rotate = {0.0f, 0.0f, 0.0f},
	    .translate = kTrainingDummyPosition,
	});
	trainingDummy_->Update();

	trainingDamage_ = std::make_unique<Damage>();
	trainingDamage_->Initialize(camera);
	trainingDamage_->SetPosition({
	    kTrainingDummyPosition.x,
	    kTrainingDummyPosition.y + kTrainingDummyScale.y + 0.5f,
	    kTrainingDummyPosition.z,
	});
	const Transform trainingDummyTransform{
	    .scale = kTrainingDummyScale,
	    .rotate = {0.0f, 0.0f, 0.0f},
	    .translate = kTrainingDummyPosition,
	};

	trainingDummy_ = std::make_unique<Object3d>();
	trainingDummy_->Initialize();
	trainingDummy_->SetModel("Enemy");
	trainingDummy_->SetCamera(camera);
	trainingDummy_->SetTransform(trainingDummyTransform);
	trainingDummy_->Update();

	trainingAdhesion_ = std::make_unique<Adhesion>();
	trainingAdhesion_->Initialize();
	trainingAdhesion_->SetTransform(trainingDummyTransform);
	trainingAdhesion_->SetCamera(camera);

	trainingDamage_ = std::make_unique<Damage>();
	trainingDamage_->Initialize(camera);
	trainingDamage_->SetPosition({
	    kTrainingDummyPosition.x,
	    kTrainingDummyPosition.y + 3.0f,
	    kTrainingDummyPosition.z,
	});
	tutorialUI_->Initialize();
	uiManager_->Initialize();

	const Vector2 guideSize{580.0f, 260.0f};
	const Vector2 guidePosition{
	    GameBase::GetInstance()->GetScreenWidth() - 600.0f,
	    120.0f,
	};
	constexpr float kGuideWindowPadding = 16.0f;
	constexpr float kGuideTextInset = 20.0f;

	const auto guideWindowHandle = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/2d/tuatorial/TutorialWindow.png");

	guideWindowSprite_ = std::make_unique<Sprite>();
	guideWindowSprite_->Initialize(guideWindowHandle);
	guideWindowSprite_->SetAnchorPoint({0.0f, 0.0f});
	guideWindowSprite_->SetPosition({
	    guidePosition.x - kGuideWindowPadding,
	    guidePosition.y - kGuideWindowPadding,
	});
	guideWindowSprite_->SetScale({
	    guideSize.x + 2.0f * kGuideWindowPadding,
	    guideSize.y + 2.0f * kGuideWindowPadding,
	});
	guideWindowSprite_->Update();

	const auto fontHandle = FreeTypeManager::CreateFace("Resources/Font/irohakakuC-Medium.ttf", 0);
	FreeTypeManager::SetPixelSizes(fontHandle, 24, 24);
	progressWindow_.Initialize(fontHandle);
	constexpr float kGuideTextOffsetX = 30.0f;

	guideText_.Initialize(fontHandle);
	guideText_.SetSize(guideSize);
	guideText_.SetPosition({
	    guidePosition.x + kGuideTextOffsetX,
	    guidePosition.y + 60.0f,
	});
	guideText_.SetAlign(TextAlign::Left);
	guideText_.SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	InitializeAttributeMenu();

	isPaused_ = false;
	wasSkipKeyHeld_ = false;
	isAttributeMenuOpen_ = false;
	skipHoldTimer_ = 0.0f;
	displayedGuide_.clear();
	trainingHitText_.clear();

	activeSwordComboStep_ = 0;
	swordAttackHit_ = false;
	lastSkillDamageId_.reset();
	lastSpecialDamageId_.reset();
	lastRainDamageId_.reset();

	TutorialFrame frame;
	frame.attribute = player_->GetCurrentAttribute();
	tutorialFlow_.Reset(frame);

	UpdateGuide(frame);
	tutorialUI_->Update(tutorialFlow_.Progress(), 0.0f, -1, false);

	auto* input = Input::GetInstance();
	input->SetIsCursorStability(true);
	input->SetIsCursorVisible(false);
}

void TutorialScene::InitializeAttributeMenu() {
	const Vector2 center{
	    GameBase::GetInstance()->GetScreenWidth() * 0.5f,
	    GameBase::GetInstance()->GetScreenHeight() * 0.5f,
	};

	for (int i = 0; i < kAttributeCount; ++i) {
		const auto handle = TextureManager::GetInstance()->GetTextureIndexByfilePath(kAttributeTexturePaths[i]);

		auto& sprite = attributeSprites_[i];
		sprite = std::make_unique<Sprite>();
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
void TutorialScene::UpdateTrainingDummy(bool normalAttackStarted) {
	const AABB dummyBounds = MakeTrainingAabb(kTrainingDummyPosition, kTrainingDummyScale);

	const auto overlapsDummy = [&dummyBounds](const Vector3& center, const Vector3& halfSize) { return RigidBody::isCollision(MakeTrainingAabb(center, halfSize), dummyBounds); };

	auto* sword = player_->GetSword();
	auto* skill = player_->GetSkill();
	auto* special = player_->GetSpecial();

	const bool swordActive = sword->IsAttacking();
	const int comboStep = swordActive ? sword->GetComboStep() : 0;
	if (!swordActive || comboStep != activeSwordComboStep_ || normalAttackStarted) {
		swordAttackHit_ = false;
	}
	activeSwordComboStep_ = comboStep;

	const float swordHalfSize = sword->GetHitSize();
	const bool swordHit = swordActive && overlapsDummy(sword->GetPosition(), {swordHalfSize, swordHalfSize, swordHalfSize});
	const bool skillHit = skill->IsDamaging() && overlapsDummy(skill->GetDamagePosition(), skill->GetDamageScale());
	const bool specialHit = special->IsDamaging() && overlapsDummy(special->GetDamagePosition(), special->GetDamageScale());

	bool rainHit = false;
	if (special->IsRainDamaging()) {
		const Vector3 rainHalfSize = special->GetRainDamageScale();
		for (const auto& rain : special->GetRainDamageTransforms()) {
			if (overlapsDummy(rain.translate, rainHalfSize)) {
				rainHit = true;
				break;
			}
		}
	}

	bool newHit = swordHit && !swordAttackHit_;
	swordAttackHit_ = swordAttackHit_ || swordHit;

	if (skillHit && lastSkillDamageId_ != skill->GetSkillDamageId()) {
		lastSkillDamageId_ = skill->GetSkillDamageId();
		newHit = true;
	}
	if (specialHit && lastSpecialDamageId_ != special->GetDamageId()) {
		lastSpecialDamageId_ = special->GetDamageId();
		newHit = true;
	}
	if (rainHit && lastRainDamageId_ != special->GetRainDamageId()) {
		lastRainDamageId_ = special->GetRainDamageId();
		newHit = true;
	}

	if (newHit) {
		ShowTrainingHit();
	}

	const bool overlapping = swordHit || skillHit || specialHit || rainHit;
	trainingDummy_->SetColor(overlapping ? Vector4{1.0f, 0.35f, 0.35f, 1.0f} : Vector4{1.0f, 1.0f, 1.0f, 1.0f});
	trainingDummy_->Update();
}

void TutorialScene::ShowTrainingHit() {
	const Attribute attribute = player_->GetCurrentAttribute();
	bool isCritical = false;
	const int damage =
	    DamageMath::CalculatePlayerToEnemyDamage(player_->GetCurrentBaseParameter(), player_->GetCurrentCombatParameter(), kTrainingDummyBase, kTrainingDummyParameter, attribute, &isCritical);

	trainingAdhesion_->AddAttribute(attribute);

	trainingDamage_->SetAttribute(attribute);
	trainingDamage_->SetIsCritical(isCritical);
	trainingDamage_->SetDamageValue(damage);
}
std::optional<int> TutorialScene::FindHoveredAttributeIndex() const {
	auto* input = Input::GetInstance();
	const Vector2 mousePosition{input->GetMouseX(), input->GetMouseY()};
	const float halfSize = kAttributeIconSize * 0.5f;

	for (int i = 0; i < kAttributeCount; ++i) {
		const auto iconPosition = attributeSprites_[i]->GetPosition();
		if (std::abs(mousePosition.x - iconPosition.x) <= halfSize && std::abs(mousePosition.y - iconPosition.y) <= halfSize) {
			return i;
		}
	}

	const auto stick = input->GetJoyStickLXY();
	const float stickLengthSquared = stick.x * stick.x + stick.y * stick.y;
	if (stickLengthSquared < kAttributeStickDeadZone * kAttributeStickDeadZone) {
		return std::nullopt;
	}

	float angle = std::atan2(stick.x, stick.y);
	if (angle < 0.0f) {
		angle += 2.0f * kPi;
	}

	const float sector = 2.0f * kPi / static_cast<float>(kAttributeCount);
	return static_cast<int>((angle + sector * 0.5f) / sector) % kAttributeCount;
}

bool TutorialScene::UpdateAttributeMenu(bool wasOpen) {
	if (isPaused_ || tutorialFlow_.CurrentStep() != TutorialFlow::Step::AttributeChange || !player_->IsSizuku() || (!wasOpen && !isAttributeMenuOpen_)) {
		return false;
	}

	const auto hoveredIndex = FindHoveredAttributeIndex();
	const Attribute currentAttribute = player_->GetCurrentAttribute();

	if (!isAttributeMenuOpen_) {
		if (!hoveredIndex || kAttributes[*hoveredIndex] == currentAttribute) {
			return false;
		}

		player_->SetCurrentAttribute(kAttributes[*hoveredIndex]);
		return player_->GetCurrentAttribute() == kAttributes[*hoveredIndex];
	}

	for (int i = 0; i < kAttributeCount; ++i) {
		const bool hovered = hoveredIndex && *hoveredIndex == i;
		const bool equipped = kAttributes[i] == currentAttribute;
		const float size = hovered ? kAttributeIconSize * 1.25f : kAttributeIconSize;

		auto& sprite = attributeSprites_[i];
		sprite->SetScale({size, size});
		sprite->SetColor(hovered || equipped ? Vector4{1.0f, 1.0f, 1.0f, 1.0f} : Vector4{0.65f, 0.65f, 0.65f, 0.85f});
		sprite->Update();
	}

	return false;
}

void TutorialScene::UpdateGuide(const TutorialFrame& frame) {
	progressWindow_.Update(tutorialFlow_.CurrentStep(), tutorialFlow_.CurrentStepProgress());
	std::u32string text = tutorialFlow_.Guide(frame);

	if (frame.isPaused) {
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
	using Step = TutorialFlow::Step;

	auto* input = Input::GetInstance();

	BGMManager::GetInstance()->Play(BGMManager::BGMType::Tutorial);

	const bool skipKeyHeld = input->PushKey(DIK_P);
	bool togglePause = input->TriggerKey(DIK_ESCAPE) || input->TriggerButton(Input::PadButton::kButtonStart);

	if (skipKeyHeld) {
		skipHoldTimer_ += GameBase::GetInstance()->GetDeltaTime();
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
	}

	const Step previousStep = tutorialFlow_.CurrentStep();
	const bool wasAttributeMenuOpen = isAttributeMenuOpen_;
	isAttributeMenuOpen_ = !isPaused_ && previousStep == Step::AttributeChange && player_->IsSizuku() && (input->PushKey(DIK_Y) || input->PushLeftTrigger());

	TutorialFrame frame;
	frame.deltaTime = GameBase::GetInstance()->GetDeltaTime();
	frame.isPaused = isPaused_;

	// カーソルの固定を戻す前に、離したフレームの位置で確定する。
	frame.attributeChangeConfirmed = UpdateAttributeMenu(wasAttributeMenuOpen);

	const bool altHeld = input->PushKey(DIK_LMENU) || input->PushKey(DIK_RMENU);
	const bool showCursor = isPaused_ || altHeld || isAttributeMenuOpen_;
	input->SetIsCursorStability(!showCursor);
	input->SetIsCursorVisible(showCursor);

	if (!isPaused_) {
		// メニューを閉じるフレームも選択操作として扱う。
		const bool canControlPlayer = !isAttributeMenuOpen_ && !wasAttributeMenuOpen;
		if (canControlPlayer) {
			cameraController_->SetPlayerPos(player_->GetPosition());
			cameraController_->Update();
		}

		auto* camera = cameraController_->GetCamera();
		player_->SetCamera(camera);
		skyDome_->SetCamera(camera);
		field_->SetCamera(camera);
		// カメラ設定部分
		trainingDummy_->SetCamera(camera);
		trainingAdhesion_->SetCamera(camera);
		trainingDamage_->SetCamera(camera);

		skyDome_->Update();
		field_->Update();

		if (canControlPlayer) {
			const bool wasSkillAttacking = player_->GetIsSkillAttack();
			const bool wasSpecialAttacking = player_->GetIsSpecialAttack();

			player_->Update();

			frame.normalAttackStarted = player_->DidStartNormalAttack();
			frame.skillStarted = !wasSkillAttacking && player_->GetIsSkillAttack();
			frame.specialStarted = !wasSpecialAttacking && player_->GetIsSpecialAttack();
		}

		// 選択中は攻撃の状態も止まっているため、命中の確定を進めない。
		// 命中処理と表示更新の部分
		if (canControlPlayer) {
			UpdateTrainingDummy(frame.normalAttackStarted);
		} else {
			trainingDummy_->Update();
		}

		trainingAdhesion_->Update();
		trainingDamage_->Update();

		const auto leftStick = input->GetJoyStickLXY();
		frame.moving = input->PushKey(DIK_W) || input->PushKey(DIK_A) || input->PushKey(DIK_S) || input->PushKey(DIK_D) || leftStick.x != 0.0f || leftStick.y != 0.0f;

		const auto mouse = input->GetMouseMove();
		const auto rightStick = input->GetJoyStickRXY();
		frame.looking = (!altHeld && (mouse.x != 0.0f || mouse.y != 0.0f)) || rightStick.x != 0.0f || rightStick.y != 0.0f;
	}

	frame.attribute = player_->GetCurrentAttribute();
	frame.specialOnCooldown = player_->GetSpecialAttackCooldownRemaining() > 0.0f;
	tutorialFlow_.Update(frame);

	if (tutorialFlow_.IsFinished()) {
		SceneManager::GetInstance()->ChangeScene("Game");
		return;
	}

	const float skipProgress = std::clamp(skipHoldTimer_ / kSkipHoldDuration, 0.0f, 1.0f);
	tutorialUI_->Update(tutorialFlow_.Progress(), skipProgress, -1, skipHoldTimer_ > 0.0f);

	UpdateGuide(frame);
	uiManager_->Update();
}
void TutorialScene::Draw() {
	Object3dCommon::GetInstance()->DrawCommon();

	skyDome_->Draw();
	field_->Draw();
	trainingDummy_->Draw();
	player_->Draw();
	trainingAdhesion_->Draw();
	trainingDamage_->Draw();

	SpriteCommon::GetInstance()->DrawCommon();

	tutorialUI_->Draw();
	guideWindowSprite_->Draw();
	progressWindow_.DrawBackground();

	if (isAttributeMenuOpen_) {
		for (const auto& sprite : attributeSprites_) {
			sprite->Draw();
		}
	}

	uiManager_->Draw();
	guideText_.Draw();
	progressWindow_.DrawText();
}
void TutorialScene::Finalize() {
	auto* input = Input::GetInstance();
	input->SetIsCursorStability(false);
	input->SetIsCursorVisible(true);
}
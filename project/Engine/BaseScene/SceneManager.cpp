#include "SceneManager.h"
#include "Engine/Editor/EditorTool/Hierarchy/Hierarchy.h"
#include "Engine/Texture/Data/Color.h"
#include "GameBase.h"
#include "Sprite/SpriteCommon.h"
#include "TextureManager.h"
#include <algorithm>
#include <cassert>
#include <utility>

// 共有インスタンスの実体。
std::unique_ptr<SceneManager> SceneManager::instance_ = nullptr;

// 初回アクセス時に生成し、それ以降は同じインスタンスを返す。
SceneManager* SceneManager::GetInstance() {
	if (!instance_) {
		instance_ = std::make_unique<SceneManager>();
	}
	return instance_.get();
}

// 再初期化はここでは実行せず、Updateで処理するために予約する。
void SceneManager::RequestReinitializeCurrentScene() { isSceneReinitializeRequested_ = true; }

// 現在のシーンを終了し、管理しているシーンとインスタンスを解放する。
void SceneManager::Finalize() {

	if (scene_) {
		scene_->Finalize();
		scene_.reset();
	}

	// 切り替え待ちのシーンは、Initialize前なので所有権の解放のみ行う。
	nextscene_.reset();
	currentSceneName_.clear();
	nextSceneName_.clear();

	// ★ Singleton の instance を解放
	// この呼び出しで自身も破棄されるため、以降はメンバーへアクセスしない。
	instance_.reset();
}

// シーンの再初期化、フェード遷移、シーン本体の更新を行う。
void SceneManager::Update() {
	// フェードをフレーム数ではなく、経過秒数で進める。
	const float deltaTime = GameBase::GetInstance()->GetDeltaTime();

	// シーン再初期化
	if (isSceneReinitializeRequested_ && scene_) {
		// インスタンスは維持し、終了処理と初期化を順に呼び出す。
		scene_->Finalize();
		scene_->Initialize();
		isSceneReinitializeRequested_ = false;

#ifdef USE_IMGUI
		// エディタープレビュー中は、再初期化直後にシーンを一度更新する。
		Hierarchy* hierarchy = Hierarchy::GetInstance();
		if (hierarchy && hierarchy->IsEditorPreviewActive()) {
			scene_->Update();
		}
#endif
	}

	// シーン切り替え（全シーン共通フェード）
	// 現在のシーンがある場合は、まず画面を暗転させる。
	if (nextscene_ && scene_ && transitionState_ == TransitionState::None) {
		transitionState_ = TransitionState::FadingOut;
		transitionTimer_ = 0.0f;
	}

	// 初回など、現在のシーンがない場合はそのまま新しいシーンを開始する。
	if (nextscene_ && !scene_) {
		// 次のシーンの所有権を、現在のシーンへ移す。
		scene_ = std::move(nextscene_);
		currentSceneName_ = nextSceneName_;
		nextSceneName_.clear();

		// シーンへ管理元を渡してから初期化する。
		scene_->SetSceneManager(this);
		scene_->Initialize();

		// 新しいシーンは、暗転状態から徐々に表示する。
		transitionState_ = TransitionState::FadingIn;
		transitionTimer_ = 0.0f;
		isSceneReinitializeRequested_ = false;
	}

	if (transitionState_ == TransitionState::FadingOut) {
		// 黒いスプライトの不透明度を上げるため、経過時間を進める。
		transitionTimer_ += deltaTime;

		if (transitionTimer_ >= fadeOutDuration_) {
			// 完全に暗転したタイミングで、古いシーンを終了する。
			if (scene_) {
				scene_->Finalize();
			}

			// 新しいシーンへ所有権を移し、名前を更新する。
			scene_ = std::move(nextscene_);
			currentSceneName_ = nextSceneName_;
			nextSceneName_.clear();

			// 新しいシーンの初期化。
			scene_->SetSceneManager(this);
			scene_->Initialize();

			// 暗転解除へ移行し、計測時間をリセットする。
			transitionState_ = TransitionState::FadingIn;
			transitionTimer_ = 0.0f;
			isSceneReinitializeRequested_ = false;
		}
	} else if (transitionState_ == TransitionState::FadingIn) {
		// 黒いスプライトの不透明度を下げるため、経過時間を進める。
		transitionTimer_ += deltaTime;

		if (transitionTimer_ >= fadeInDuration_) {
			// フェードインが終わったら通常状態へ戻す。
			transitionState_ = TransitionState::None;
			transitionTimer_ = 0.0f;
		}
	}

	// フェード処理とは別に、現在のシーンの更新を行う。
	if (scene_) {
#ifdef USE_IMGUI
		Hierarchy* hierarchy = Hierarchy::GetInstance();

		if (hierarchy && hierarchy->IsEditorPreviewActive()) {
			// プレビュー中は、エディター用の更新処理を呼ぶ。
			hierarchy->UpdateEditorPreview();
		} else if (!hierarchy || (hierarchy->IsPlayMode() && !hierarchy->IsPaused())) {
			// エディターがない場合、または再生中かつ非一時停止の場合に更新する。
			scene_->Update();
		}
#else
		// ImGuiを使用しない構成では、通常どおりシーンを更新する。
		scene_->Update();
#endif
	}
}

// 現在のシーンが存在する場合に、その描画処理を呼び出す。
void SceneManager::Draw() {
	if (scene_) {
		scene_->Draw();
	}
}

// シーンの上に重ねる黒いフェード用スプライトを描画する。
void SceneManager::DrawOverlay() {
	const float alpha = GetFadeAlpha();

	// 完全に透明な場合は描画しない。
	if (alpha <= 0.0f) {
		return;
	}

	// 初回のみ、フェード用スプライトを生成する。
	EnsureFadeSprite();

	// スプライト用の共通描画設定と、アルファ合成を設定する。
	SpriteCommon::GetInstance()->DrawCommon();
	SpriteCommon::GetInstance()->SetBlendMode(BlendMode::kBlendModeAlpha);

	// 画面全体を覆う黒いスプライトの不透明度を更新する。
	fadeSprite_->SetColor({0.0f, 0.0f, 0.0f, alpha});
	fadeSprite_->SetScale({GameBase::GetInstance()->GetScreenWidth(), GameBase::GetInstance()->GetScreenHeight()});
	fadeSprite_->SetPosition({0.0f, 0.0f});
	fadeSprite_->Update();
	fadeSprite_->Draw();
}

// シーン名からインスタンスを生成し、次のシーンとして予約する。
void SceneManager::ChangeScene(const std::string& sceneName) {

	// シーン生成には、事前にファクトリーを設定しておく必要がある。
	assert(sceneFactory_);

	if (!sceneFactory_ || sceneName.empty()) {
		return;
	}

	// 切り替え予約がなく、すでに同じシーンを実行中なら何もしない。
	if (sceneName == currentSceneName_ && !nextscene_) {
		return;
	}

	// シーンの具体的な生成方法はファクトリーへ委譲する。
	std::unique_ptr<BaseScene> requestedScene = sceneFactory_->CreateScene(sceneName);

	// 未対応の名前などで生成できなかった場合は、現在の状態を維持する。
	if (!requestedScene) {
		return;
	}

	// 生成したシーンの所有権を、切り替え待ちのシーンへ移す。
	// すでに予約がある場合は、その予約を置き換える。
	nextscene_ = std::move(requestedScene);
	nextSceneName_ = sceneName;

#ifdef USE_IMGUI
	// エディターへ、シーン切り替えが要求されたことを通知する。
	if (Hierarchy* hierarchy = Hierarchy::GetInstance()) {
		hierarchy->OnSceneChangeRequested(sceneName);
	}
#endif
}

// フェード用スプライトを、必要になった時点で一度だけ生成する。
void SceneManager::EnsureFadeSprite() {
	if (fadeSprite_) {
		return;
	}

	// 白いテクスチャを黒く着色し、不透明度を変えることでフェードを表現する。
	fadeTextureHandle_ = TextureManager::GetInstance()->GetTextureIndexByfilePath("Resources/TD3_3102/2d/white2x2.png");

	fadeSprite_ = std::make_unique<Sprite>();
	fadeSprite_->Initialize(fadeTextureHandle_);
	fadeSprite_->SetColor({0, 0, 0, 1});
}

// 現在のフェード状態に対応する不透明度を求める。
float SceneManager::GetFadeAlpha() const {
	if (transitionState_ == TransitionState::FadingOut) {
		// フェードアウトでは、透明から不透明へ変化させる。
		return std::clamp(transitionTimer_ / fadeOutDuration_, 0.0f, 1.0f);
	}

	if (transitionState_ == TransitionState::FadingIn) {
		// フェードインでは、不透明から透明へ変化させる。
		return std::clamp(1.0f - (transitionTimer_ / fadeInDuration_), 0.0f, 1.0f);
	}

	// 遷移中でなければ透明にする。
	return 0.0f;
}

// ファクトリーが生成できるシーン名の一覧を取得する。
std::vector<std::string> SceneManager::GetSceneNames() const {
	// ファクトリーが未設定なら空の一覧を返す。
	if (!sceneFactory_) {
		return {};
	}

	return sceneFactory_->GetSceneNames();
}
#pragma once
#include "AbstractSceneFactory.h"
#include "BaseScene.h"
#include "Sprite.h"
#include <memory>
#include <string>
#include <vector>

// 現在のシーンと次のシーンを管理する。
// シーンの更新・描画に加え、再初期化とフェード付きの切り替えを行う。
class SceneManager {

	// ゲーム全体で共有するインスタンス。
	static std::unique_ptr<SceneManager> instance_;

	// 現在実行しているシーンと、切り替え待ちのシーン。
	// unique_ptrにより、それぞれのシーンの所有権を管理する。
	std::unique_ptr<BaseScene> scene_ = nullptr;
	std::unique_ptr<BaseScene> nextscene_ = nullptr;

	// 現在のシーン名と、切り替え先のシーン名。
	std::string currentSceneName_;
	std::string nextSceneName_;

	// シーン生成を委譲するファクトリー。
	// 所有権は持たないため、使用中は呼び出し側で生存させる。
	AbstractSceneFactory* sceneFactory_ = nullptr;

	// 次の更新時に、現在のシーンを再初期化するための要求フラグ。
	bool isSceneReinitializeRequested_ = false;

	// フェードの進行状態。
	// None：遷移なし、FadingOut：暗転中、FadingIn：暗転解除中。
	enum class TransitionState { None, FadingOut, FadingIn };
	TransitionState transitionState_ = TransitionState::None;

	// 現在のフェード段階で経過した秒数。
	float transitionTimer_ = 0.0f;

	// フェードアウトとフェードインにかける時間。
	const float fadeOutDuration_ = 0.35f;
	const float fadeInDuration_ = 0.35f;

	// 画面全体を黒く覆うフェード用スプライトと、そのテクスチャ番号。
	std::unique_ptr<Sprite> fadeSprite_ = nullptr;
	uint32_t fadeTextureHandle_ = 0;

	// フェード用スプライトが未生成の場合に生成する。
	void EnsureFadeSprite();

	// フェード状態と経過時間から、不透明度を0～1で求める。
	float GetFadeAlpha() const;

public:
	// シーンの生成に使用するファクトリーを設定する。
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

	// 指定した名前のシーンを生成し、切り替え先として予約する。
	// 実際の切り替えと初期化はUpdateで行う。
	void ChangeScene(const std::string& sceneName);

	// 現在のシーンの再初期化を要求する。
	void RequestReinitializeCurrentScene();

	// 再初期化要求、シーン遷移、現在のシーンの更新を処理する。
	void Update();

	// 現在のシーンを描画する。
	void Draw();

	// シーンに重ねるフェード用スプライトを描画する。
	void DrawOverlay();

	// シーンを終了し、共有インスタンスを解放する。
	void Finalize();

	// 現在のシーン名を読み取り専用の参照で取得する。
	const std::string& GetCurrentSceneName() const { return currentSceneName_; }

	// ファクトリーが生成できるシーン名の一覧を取得する。
	std::vector<std::string> GetSceneNames() const;

	// 共有インスタンスを取得する。
	// 初回の呼び出し時に生成する。
	static SceneManager* GetInstance();
};
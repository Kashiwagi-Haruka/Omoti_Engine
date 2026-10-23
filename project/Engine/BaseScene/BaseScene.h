#pragma once

//SceneManagerを前方宣言する
class SceneManager;

// 各シーンが継承する基底クラス。
// 初期化・更新・描画・終了処理を共通のインターフェースとして定義する。
class BaseScene {

	// このシーンを管理するSceneManagerへのポインター。
	SceneManager* sceneManager_ = nullptr;

protected:
	// シーンの終了状態。
	// 派生クラスから変更でき、trueにすることで終了状態を示す。
	bool isSceneEnd_ = false;

public:
	// 基底クラスのポインター経由で破棄した場合にも、
	// 派生クラスのデストラクターが呼ばれるようにする。
	virtual ~BaseScene() = default;

	// シーン開始時の初期化処理。
	// 純粋仮想関数なので、派生クラスで実装する。
	virtual void Initialize() = 0;

	// 毎フレームの更新処理。
	// 入力に応じた処理や、オブジェクトの状態更新などを実装する。
	virtual void Update() = 0;

	// シーンの描画処理。
	virtual void Draw() = 0;

	// シーン終了時の処理。
	// シーン内で使用しているリソースの解放などを実装する。
	virtual void Finalize() = 0;

	// このシーンを管理するSceneManagerを設定する。
	// virtualのため、派生クラスで必要に応じてオーバーライドできる。
	virtual void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

	// シーンが終了状態かどうかを取得する。
	virtual bool GetIsSceneEnd() const{ return isSceneEnd_; };
};
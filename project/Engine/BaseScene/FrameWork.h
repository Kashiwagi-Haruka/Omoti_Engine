#pragma once
#include "AbstractSceneFactory.h"
#include <memory>

// アプリケーションの実行手順を定義する基底クラス。
// 初期化 → 更新・描画の繰り返し → 終了処理の流れを管理する。
// Drawが純粋仮想関数のため、このクラス自体はインスタンス化できない。
class FrameWork {

	// シーンを生成するファクトリーの所有権を保持する。
	// アクセス指定を省略しているためprivateとなる。
	// 現在の実装では、このメンバーを使用する処理はない。
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

protected:
	// メインループの終了要求。
	// 派生クラスからtrueにすることで、ループの終了を要求できる。
	bool endRequest_ = false;

public:
	// 基底クラスのポインター経由で破棄した場合にも、
	// 派生クラスのデストラクターが呼ばれるようにする。
	virtual ~FrameWork() = default;

	// 実行開始時の初期化処理。
	// 派生クラスで必要に応じてオーバーライドする。
	virtual void Initialize();

	// メインループ終了後の終了処理。
	// リソースの解放などを派生クラスで実装する。
	virtual void Finalize();

	// 毎フレームの更新処理。
	// 入力処理、ゲーム状態の更新、終了要求の設定などを行う。
	virtual void Update();

	// 毎フレームの描画処理。
	// 純粋仮想関数なので、派生クラスで実装する。
	virtual void Draw() = 0;

	// 終了が要求されているかを取得する。
	// 派生クラスでオーバーライドし、終了条件を変更することもできる。
	virtual bool IsEndRequest() { return endRequest_; }

	// 初期化からメインループ、終了処理までを順番に実行する。
	void Run();
};
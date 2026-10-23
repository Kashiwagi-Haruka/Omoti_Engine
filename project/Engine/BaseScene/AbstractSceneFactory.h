#pragma once

#include "BaseScene.h"
#include <memory>
#include <string>
#include <vector>

// シーンを生成するファクトリーの基底クラス。
// シーンの生成方法と、生成可能なシーン名の取得方法を共通化する。
// 純粋仮想関数を持つため、このクラス自体はインスタンス化できない。
class AbstractSceneFactory {

public:
	// 基底クラスのポインター経由で破棄した場合にも、
	// 派生クラスのデストラクターが呼ばれるようにする。
	virtual ~AbstractSceneFactory() = default;

	// 指定した名前に対応するシーンを生成する。
	// シーン名と生成するクラスの対応は、派生クラスで実装する。
	// 戻り値のunique_ptrが、生成したシーンの所有権を呼び出し側へ渡す。
	// 未対応の名前を受け取った場合の処理も、派生クラス側で定義する。
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;

	// このファクトリーが生成できるシーン名の一覧を取得する。
	// constにより、この関数内では通常のメンバー変数を変更しない。
	// 一覧は値として返すため、取得側で変更しても内部状態には影響しない。
	virtual std::vector<std::string> GetSceneNames() const = 0;
};
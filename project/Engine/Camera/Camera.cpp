#include "Camera.h"
#include "Engine/Editor/EditorTool/Hierarchy/Hierarchy.h"
#include "Engine/Loadfile/JSON/JsonManager.h"
#include "Function.h"
#include "GameBase.h"
#include <cmath>

namespace {

// カメラTransformからビュー行列を作成する補助関数
// カメラの移動・回転を打ち消す変換を組み合わせる。
// スケールはビュー行列に反映しない。
Matrix4x4 MakeCameraViewMatrix(const Transform& transform) {
	const Matrix4x4 inverseTranslate = Function::MakeTranslateMatrix(-transform.translate.x, -transform.translate.y, -transform.translate.z);
	const Matrix4x4 inverseRotateY = Function::MakeRotateYMatrix(-transform.rotate.y);
	const Matrix4x4 inverseRotateX = Function::MakeRotateXMatrix(-transform.rotate.x);
	const Matrix4x4 inverseRotateZ = Function::MakeRotateZMatrix(-transform.rotate.z);

	// 逆平行移動 → Y軸の逆回転 → X軸の逆回転 → Z軸の逆回転の順に合成する。
	return Function::Multiply(Function::Multiply(Function::Multiply(inverseTranslate, inverseRotateY), inverseRotateX), inverseRotateZ);
}

// 外部から受け取ったワールド行列をもとに、ビュー行列を作成する。
Matrix4x4 MakeCameraViewMatrix(const Matrix4x4& worldMatrix) {
	Matrix4x4 rotationMatrix = Function::MakeIdentity4x4();

	// 各軸の長さを1に揃え、軸ごとのスケール成分を取り除く。
	for (int row = 0; row < 3; ++row) {
		const float axisLength = std::sqrt(worldMatrix.m[row][0] * worldMatrix.m[row][0] + worldMatrix.m[row][1] * worldMatrix.m[row][1] + worldMatrix.m[row][2] * worldMatrix.m[row][2]);

		// 長さが0の場合は、0で割らずに初期値を維持する。
		if (axisLength == 0.0f) {
			continue;
		}

		rotationMatrix.m[row][0] = worldMatrix.m[row][0] / axisLength;
		rotationMatrix.m[row][1] = worldMatrix.m[row][1] / axisLength;
		rotationMatrix.m[row][2] = worldMatrix.m[row][2] / axisLength;
	}

	// ワールド行列の位置を打ち消す平行移動を作成する。
	const Matrix4x4 inverseTranslate = Function::MakeTranslateMatrix(-worldMatrix.m[3][0], -worldMatrix.m[3][1], -worldMatrix.m[3][2]);

	// 正規直交な回転行列では、転置行列が逆行列になる。
	const Matrix4x4 inverseRotate = Function::Transpose(rotationMatrix);

	return Function::Multiply(inverseTranslate, inverseRotate);
}

} // namespace

// 次に生成されるカメラへ割り当てるエディター用番号。
int Camera::nextEditorId_ = 0;

// Transformと投影設定を初期化し、各行列を作成する。
// 初期視野角は0.45πラジアンで、アスペクト比には画面サイズを使用する。
Camera::Camera()
    : transform_({
          {1.0f, 1.0f, 1.0f},
          {0.0f, 0.0f, 0.0f},
          {0.0f, 0.0f, 0.0f}
}),
      fovY(0.45f * 3.14159265f), aspectRatio(float(GameBase::GetInstance()->GetScreenWidth()) / float(GameBase::GetInstance()->GetScreenHeight())), nearZ(0.1f), farZ(10000.0f),
      worldMatrix_(Function::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, transform_.rotate, transform_.translate)), viewMatrix_(MakeCameraViewMatrix(transform_)),
      projectionMatrix_(Function::MakePerspectiveFovMatrix(fovY, aspectRatio, nearZ, farZ)), viewProjectionMatrix_(Function::Multiply(viewMatrix_, projectionMatrix_)) {

	// 生成順に識別番号を割り当てる。
	editorId_ = nextEditorId_++;

	// 最初に生成されたカメラだけ、保存済み設定の読み込みを試みる。
	if (editorId_ == 0) {
		LoadEditorData();
	}

	// エディターから参照できるよう、自身を登録する。
	Hierarchy::GetInstance()->RegisterCamera(this);
}

// 破棄されるカメラを、エディターの管理対象から取り除く。
Camera::~Camera() { Hierarchy::GetInstance()->UnregisterCamera(this); }

// 保持しているTransformと投影設定から、行列を再計算する。
void Camera::Update() {
	// 現在のTransformと投影設定をもとに各行列を更新
	// カメラのスケールには、常に等倍を使用する。
	worldMatrix_ = Function::MakeAffineMatrix({1.0f, 1.0f, 1.0f}, transform_.rotate, transform_.translate);
	viewMatrix_ = MakeCameraViewMatrix(transform_);
	projectionMatrix_ = Function::MakePerspectiveFovMatrix(fovY, aspectRatio, nearZ, farZ);
	viewProjectionMatrix_ = Function::Multiply(viewMatrix_, projectionMatrix_);
}

// 追加しました。
//  外部で計算したワールド行列から、ビュー行列と投影行列を更新する。
//  transform_の位置・回転には、渡された行列の内容を反映しない。
void Camera::UpdateViewProjection(const Matrix4x4& worldMatrix) {
	// 現在のTransformと投影設定をもとに各行列を更新
	// この関数では、ビュー行列の計算に引数のworldMatrixを使用する。
	worldMatrix_ = worldMatrix;
	viewMatrix_ = MakeCameraViewMatrix(worldMatrix);
	projectionMatrix_ = Function::MakePerspectiveFovMatrix(fovY, aspectRatio, nearZ, farZ);
	viewProjectionMatrix_ = Function::Multiply(viewMatrix_, projectionMatrix_);
}

// 保存済みのカメラ設定を読み込む。
// 項目が存在し、対応する型・要素数を満たす場合に値を反映する。
bool Camera::LoadEditorData() {
	JsonManager* jsonManager = JsonManager::GetInstance();

	if (!jsonManager->LoadJson("cameraEditor.json")) {
		return false;
	}

	const nlohmann::json& root = jsonManager->GetData();

	// camera項目がオブジェクトとして存在することを確認する。
	if (!root.contains("camera") || !root["camera"].is_object()) {
		return false;
	}

	const nlohmann::json& cameraJson = root["camera"];

	// スケールは3要素の配列から読み込む。
	if (cameraJson.contains("scale") && cameraJson["scale"].is_array() && cameraJson["scale"].size() == 3) {
		transform_.scale = {
		    cameraJson["scale"][0].get<float>(),
		    cameraJson["scale"][1].get<float>(),
		    cameraJson["scale"][2].get<float>(),
		};
	}

	// 各軸の回転角を読み込む。
	if (cameraJson.contains("rotate") && cameraJson["rotate"].is_array() && cameraJson["rotate"].size() == 3) {
		transform_.rotate = {
		    cameraJson["rotate"][0].get<float>(),
		    cameraJson["rotate"][1].get<float>(),
		    cameraJson["rotate"][2].get<float>(),
		};
	}

	// カメラの位置を読み込む。
	if (cameraJson.contains("translate") && cameraJson["translate"].is_array() && cameraJson["translate"].size() == 3) {
		transform_.translate = {
		    cameraJson["translate"][0].get<float>(),
		    cameraJson["translate"][1].get<float>(),
		    cameraJson["translate"][2].get<float>(),
		};
	}

	// 投影設定は、数値として保存されている項目だけを反映する。
	if (cameraJson.contains("fovY") && cameraJson["fovY"].is_number()) {
		fovY = cameraJson["fovY"].get<float>();
	}

	if (cameraJson.contains("aspectRatio") && cameraJson["aspectRatio"].is_number()) {
		aspectRatio = cameraJson["aspectRatio"].get<float>();
	}

	if (cameraJson.contains("nearZ") && cameraJson["nearZ"].is_number()) {
		nearZ = cameraJson["nearZ"].get<float>();
	}

	if (cameraJson.contains("farZ") && cameraJson["farZ"].is_number()) {
		farZ = cameraJson["farZ"].get<float>();
	}

	// 読み込んだ設定を、描画用の各行列へ反映する。
	Update();
	return true;
}

// 現在のTransformと投影設定をJSONファイルへ保存する。
// 行列そのものは保存せず、再計算に使用する設定値を保存する。
bool Camera::SaveEditorData() const {
	JsonManager* jsonManager = JsonManager::GetInstance();

	nlohmann::json root;
	root["camera"] = {
	    {"scale",       {transform_.scale.x, transform_.scale.y, transform_.scale.z}            },
	    {"rotate",      {transform_.rotate.x, transform_.rotate.y, transform_.rotate.z}         },
	    {"translate",   {transform_.translate.x, transform_.translate.y, transform_.translate.z}},
	    {"fovY",        fovY	                                                                },
	    {"aspectRatio", aspectRatio	                                                         },
	    {"nearZ",       nearZ	                                                               },
	    {"farZ",        farZ	                                                                },
	};

	jsonManager->SetData(root);
	return jsonManager->SaveJson("cameraEditor.json");
}

// 外部で作成されたビュー行列と投影行列を採用する。
// ビュー行列の逆行列から、ワールド行列とカメラ位置も更新する。
void Camera::SetViewProjectionMatrix(const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix) {
	// 受け取った行列を採用し、派生行列と位置情報を同期
	viewMatrix_ = viewMatrix;
	projectionMatrix_ = projectionMatrix;
	viewProjectionMatrix_ = Function::Multiply(viewMatrix_, projectionMatrix_);

	worldMatrix_ = Function::Inverse(viewMatrix_);
	transform_.translate = {worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]};
}
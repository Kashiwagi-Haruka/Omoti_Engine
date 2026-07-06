#pragma once
#include "Camera.h"
#include "Object3d/Object3d.h"
#include "Transform.h"
#include "Background/CharacterDisplaySkyDome.h"
#include <memory>
#include "Object/Characters/Playable/Individual/Sizuku/Sizuku.h"
#include <numbers>
#include "Light/CommonLight/DirectionalCommonLight.h"
/// <summary>
/// キャラクター画面
/// </summary>
class CharacterDisplay {

	/// <summary>
	/// 項目
	/// </summary>
	enum class Menu { 
		WEAPON, // 武器
		EQUIP,  // 装備
		SKILLTREE, // スキルツリー
		REINFORCEMENT, // 強化
		PROFILE,       // プロフィール
	};

	std::unique_ptr<Sizuku> sizukuObject_ = nullptr;
	std::unique_ptr<CharacterDisplaySkyDome> skyDome_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	Transform characterTransform_ = {
	    .scale{1.0f, 1.0f, 1.0f},
	    .rotate{0.0f,  std::numbers::pi_v<float>,  0.0f },
	    .translate{0.0f,  2.0f, 0.0f },
	};
	Transform cameraTransform_ = {
	    .scale{1.0f, 1.0f, 1.0f  },
	    .rotate{0.1f, 0.0f, 0.0f  },
	    .translate{0.0f, 2.0f, -5.0f},
	};
	float rotateSpeed_ = 0.01f;
	bool isActive_ = true;
	DirectionalCommonLight directionalLight{.color{1,1,1,},.direction{0,-1,1.0f},.intensity{0.5f}};

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// アクティブ状態を設定する。
	/// </summary>
	/// <param name="isActive"></param>
	void SetActive(bool isActive) { isActive_ = isActive; }

	/// <summary>
	/// アクティブ状態を取得する。
	/// </summary>
	/// <returns></returns>
	bool IsActive() const { return isActive_; }

	/// <summary>
	/// キャラクターのトランスフォームを設定する。
	/// </summary>
	/// <param name="transform"></param>
	void SetCharacterTransform(const Transform& transform) { characterTransform_ = transform; }

	/// <summary>
	/// キャラクターのトランスフォームを取得する。
	/// </summary>
	/// <returns></returns>
	const Transform& GetCharacterTransform() const { return characterTransform_; }

	/// <summary>
	/// カメラのトランスフォームを設定する。
	/// </summary>
	/// <param name="transform"></param>
	void SetCameraTransform(const Transform& transform) { cameraTransform_ = transform; }

	/// <summary>
	/// カメラのトランスフォームを取得する。
	/// </summary>
	/// <returns></returns>
	const Transform& GetCameraTransform() const { return cameraTransform_; }
};
#pragma once
#include "Background/CharacterDisplaySkyDome.h"
#include "Camera.h"
#include "CharacterDisplayMenuType.h"
#include "Icon/CharacterDisplayIcon.h"
#include "Menu/CharacterDisplayMenuText.h"
#include "Menu/Equip/CharacterDisplayEquip.h"
#include "Menu/Profile/CharacterDisplayProfile.h"
#include "Menu/Reinforcement/CharacterDisplayReinforcement.h"
#include "Menu/Skilltree/CharacterDisplaySkilltree.h"
#include "Menu/Status/CharacterDisplayStatus.h"
#include "Menu/Weapon/CharacterDisplayWeapon.h"
#include "Object/Characters/Playable/PlayableManager.h"
#include "Object3d/Object3d.h"
#include "Text/Text.h"
#include "Transform.h"
#include <cstdint>
#include <memory>
#include <string>

#include "Light/CommonLight/DirectionalCommonLight.h"
#include <numbers>
#include <vector>
/// <summary>
/// キャラクター画面
/// </summary>
class Team;

class CharacterDisplay {

	PlayableManager playableManager_;
	PlayableBase* currentPlayable_ = nullptr;
	std::unique_ptr<CharacterDisplaySkyDome> skyDome_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<CharacterDisplayMenuText> menuText_ = nullptr;
	std::unique_ptr<CharacterDisplayIcon> characterDisplayIcon_ = nullptr;
	Transform characterTransform_ = {
	    .scale{1.0f, 1.0f,                      1.0f},
	    .rotate{0.0f, std::numbers::pi_v<float>, 0.0f},
	    .translate{0.0f, 2.0f,                      0.0f},
	};
	Transform cameraTransform_ = {
	    .scale{1.0f, 1.0f, 1.0f },
	    .rotate{0.1f, 0.0f, 0.0f },
	    .translate{0.0f, 2.0f, -5.0f},
	};
	float rotateSpeed_ = 0.01f;
	bool isActive_ = true;
	DirectionalCommonLight directionalLight{
	    .color{
	           1, 1,
	           1, },
	    .direction{0, -1, 1.0f},
	    .intensity{0.5f}
    };

	CharacterDisplayMenuType selectMenuType_ = CharacterDisplayMenuType::STATUS;

	std::unique_ptr<CharacterDisplayStatus> status_;
	std::unique_ptr<CharacterDisplayWeapon> weapon_;
	std::unique_ptr<CharacterDisplayEquip> equip_;
	std::unique_ptr<CharacterDisplaySkilltree> skilltree_;
	std::unique_ptr<CharacterDisplayReinforcement> reinforcement_;
	std::unique_ptr<CharacterDisplayProfile> profile_;

	std::vector<std::string> ownedPlayableNames_{};
	std::vector<std::u32string> ownedDisplayNames_{};
	Text characterNameText_{};
	Text characterSwitchGuideText_{};
	std::unique_ptr<Sprite> characterSwitchLeftGuideSprite_{};
	std::unique_ptr<Sprite> characterSwitchRightGuideSprite_{};
	uint32_t characterNameFontHandle_ = 0;
	size_t selectedCharacterIndex_ = 0;

	void RebuildOwnedCharacters(const Team& team);
	void ChangeDisplayedCharacter(size_t characterIndex);

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const Team& team);

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
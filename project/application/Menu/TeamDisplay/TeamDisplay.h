#pragma once
#include "Camera.h"
#include "Object3d/Object3d.h"
#include "Primitive/Primitive.h"
#include "Sprite.h"
#include "Text/Text.h"
#include "Vector2.h"
#include "Vector3.h"
#include <array>
#include <memory>
#include <vector>

class Team;

/// <summary>
/// チーム編成画面
/// </summary>
class TeamDisplay {
public:
	/// チーム編成画面とメンバー情報に必要なリソースを初期化する。
	void Initialize(const Team& team);
	/// チーム編成画面の表示用リソースを解放する。
	void Unload();
	/// チーム編成画面の入力を更新する。
	void Update(Team& team);
	/// パーティ編成UIを描画する。
	void Draw(const Team& team);

private:
	/// チームに編成できる最大メンバー数。
	static constexpr int kMaxMembersCount = 4;

	/// パーティ編成画面内のボタン、スロット、ドラッグ操作を更新する。
	void UpdatePartyUI(Team& team);
	/// チーム画面の4人分のモデル表示と空きスロット表示を描画する。
	void DrawTeamDisplayMembers(const Team& team);

	/// 指定座標が矩形範囲内に含まれているかを判定する。
	bool IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const;

	/// 単色UI描画に使用する白テクスチャのハンドル。
	uint32_t whiteTextureHandle_ = 0;
	/// 所持キャラクターのアイコンテクスチャハンドル一覧。
	std::vector<uint32_t> ownedCharacterIconHandles_{};
	/// チーム画面に表示するキャラクターモデル。
	std::array<std::unique_ptr<Object3d>, kMaxMembersCount> teamDisplayModels_{};
	/// チーム画面の未編成スロットに表示するNoMember板ポリゴン。
	std::array<std::unique_ptr<Primitive>, kMaxMembersCount> noMemberPlanes_{};

	/// パーティ編成画面に表示するチームスロット背景。
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamSlotSprites_{};
	/// パーティ編成画面の各スロットに表示するメンバーアイコン。
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamMemberIcons_{};
	/// 編成確定ボタンに表示するテキスト。
	Text confirmButtonText_{};
	/// 編成移行テキスト
	Text formationTransitionText_{};
	/// チーム編成テキスト
	Text teamFormationText_{};

	/// 各チームスロットの画面上の配置座標。
	std::array<Vector2, kMaxMembersCount> slotPositions_{};
	/// 所持キャラクターアイコン一覧の画面上の配置座標。
	std::array<Vector2, kMaxMembersCount> inventoryIconPositions_{};
	/// 所持キャラクター一覧に表示するアイコンスプライト。
	std::vector<std::unique_ptr<Sprite>> inventoryIcons_{};
	/// 所持キャラクター一覧パネルの背景スプライト。
	std::unique_ptr<Sprite> inventoryBg_;
	/// 編成内容を確定するボタンのスプライト。
	std::unique_ptr<Sprite> confirmButton_;
	/// 選択中キャラクターを大きく表示するプレビュースプライト。
	std::unique_ptr<Sprite> candidatePreview_;
	/// ドラッグ中のキャラクターアイコンを表示するスプライト。
	std::unique_ptr<Sprite> draggingIcon_;
	/// 所持キャラクター一覧の選択位置を示すマーカー。
	std::unique_ptr<Sprite> inventorySelectionMarker_;
	/// ゲーム中HUDで現在操作中のメンバーを示すマーカー。
	std::unique_ptr<Sprite> memberSelectionMarker_;
	/// チーム編成画面背景板ポリ
	std::unique_ptr<Primitive> teamBackgroundPlane_;
	/// チーム画面用のカメラ
	std::unique_ptr<Camera> camera_;

	/// チームスロット背景の表示サイズ。
	Vector2 slotSize_{80.0f, 80.0f};
	/// 所持キャラクター一覧パネルの左上座標。
	Vector2 inventoryPanelPos_{70.0f, 170.0f};
	/// 所持キャラクター一覧パネルの表示サイズ。
	Vector2 inventoryPanelSize_{360.0f, 420.0f};
	/// キャラクターアイコンの表示サイズ。
	Vector2 iconSize_{64.0f, 64.0f};
	/// 編成確定ボタンの左上座標。
	Vector2 confirmPos_{930.0f, 580.0f};
	/// 編成確定ボタンの表示サイズ。
	Vector2 confirmSize_{220.0f, 70.0f};
	/// 選択中キャラクタープレビューの左上座標。
	Vector2 previewPos_{700.0f, 380.0f};
	/// 選択中キャラクタープレビューの表示サイズ。
	Vector2 previewSize_{128.0f, 128.0f};
	/// チーム画面に表示する4人分のワールド座標。
	std::array<Vector3, kMaxMembersCount> teamDisplayPositions_{
	    Vector3{-9.0f, 0.0f, -0.8f},
        Vector3{-3.0f, 0.0f, -0.8f},
        Vector3{3.0f,  0.0f, -0.8f},
        Vector3{9.0f,  0.0f, -0.8f}
    };
	/// チーム画面に表示するキャラクターモデルの拡大率。
	Vector3 teamDisplayModelScale_{1.2f, 1.2f, 1.2f};
	/// チーム画面に表示するNoMember板ポリゴンの拡大率。
	Vector3 noMemberPlaneScale_{4.0f, 4.0f, 1.0f};

	/// 現在選択されているチームスロット番号。
	int selectedSlotIndex_ = -1;
	/// 現在選択されている所持キャラクター一覧のインデックス。
	int selectedInventoryIndex_ = 0;
	/// マウスカーソルが重なっている所持キャラクター一覧のインデックス。
	int hoveredInventoryIndex_ = -1;
	/// ドロップ先候補になっているチームスロット番号。
	int dropTargetSlotIndex_ = -1;
	/// ドラッグ中の所持キャラクター一覧インデックス。
	int draggingInventoryIndex_ = -1;
	/// 編成候補のキャラクターが選択済みかどうかを示すフラグ。
	bool isCandidateSelected_ = false;
	/// 所持キャラクターアイコンをドラッグ中かどうかを示すフラグ。
	bool isDraggingInventoryIcon_ = false;
	/// ゲーム中メンバー選択表示が有効かどうかを示すフラグ。
	bool isMemberSelectionActive_ = false;
	/// HUDテキスト描画に使用するフォントハンドル。
	uint32_t hudFontHandle_ = 0;
};
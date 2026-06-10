#pragma once
#include "Object/Characters/Playable/Base/PlayableBase.h"
#include "Object3d/Object3d.h"
#include "Sprite.h"
#include "Primitive/Primitive.h"
#include "Text/Text.h"
#include "Vector2.h"
#include <array>
#include <memory>
#include <string>
#include <vector>
#include "Camera.h"

class Team {
public:
	/// チーム編成画面とメンバー情報に必要なリソースを初期化する。
	void Initialize();
	/// パーティ画面の開閉状態に応じて、チーム操作とUI入力を更新する。
	void Update(bool isPartyOpen);
	/// パーティ編成UIを描画する。
	void Draw();
	/// ゲーム中に表示するメンバーリストHUDを描画する。
	void DrawInGameMemberList();

	/// 指定したスロットにメンバーが設定されているかを返す。
	bool GetHasMember(int slotIndex) const;
	/// 現在操作対象になっているチームスロット番号を返す。
	int GetActiveSlot() const { return activeSlotIndex_; }
	/// キャラクター切り替え要求を取得し、取得後にフラグをリセットする。
	bool ConsumeCharacterSwitchTriggered();
	/// 現在操作中のキャラクター名を文字列で返す。
	std::string GetActiveCharacterName() const;

private:
	/// チームに編成できる最大メンバー数。
	static constexpr int kMaxMembersCount = 4;

	/// パーティ編成画面内のボタン、スロット、ドラッグ操作を更新する。
	void UpdatePartyUI();
	/// 指定キャラクターを指定スロットへ割り当てる。
	void AssignCharacterToSlot(int slotIndex, int characterIndex);

	/// 指定座標が矩形範囲内に含まれているかを判定する。
	bool IsInsideRect(const Vector2& point, const Vector2& pos, const Vector2& size) const;

	/// 単色UI描画に使用する白テクスチャのハンドル。
	uint32_t whiteTextureHandle_ = 0;
	/// 所持キャラクターのアイコンテクスチャハンドル一覧。
	std::vector<uint32_t> ownedCharacterIconHandles_{};
	/// 所持しているプレイアブルキャラクター実体の一覧。
	std::vector<std::unique_ptr<PlayableBase>> ownedCharacters_{};
	/// チームメンバー表示用3Dモデルの一覧。
	std::vector<std::unique_ptr<Object3d>> teamMemberModels_{};

	/// パーティ編成画面に表示するチームスロット背景。
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamSlotSprites_{};
	/// パーティ編成画面の各スロットに表示するメンバーアイコン。
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> teamMemberIcons_{};
	/// ゲーム中HUDに表示するメンバーアイコン。
	std::array<std::unique_ptr<Sprite>, kMaxMembersCount> inGameMemberIcons_{};
	/// ゲーム中HUDに表示するメンバー名テキスト。
	std::array<Text, kMaxMembersCount> inGameMemberNameTexts_{};
	/// 編成確定ボタンに表示するテキスト。
	Text confirmButtonText_{};
	/// 編成移行テキスト
	Text formationTransitionText_{};
	/// 各チームスロットに割り当てられている所持キャラクターのインデックス。
	std::array<int, kMaxMembersCount> teamMemberCharacterIndices_{};
	/// 各チームスロットが使用中かどうかを示すフラグ。
	std::array<bool, kMaxMembersCount> occupiedSlots_{};
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
	/// 現在操作対象になっているチームスロット番号。
	int activeSlotIndex_ = 0;
	/// 操作キャラクターの切り替えが発生したことを通知するフラグ。
	bool characterSwitchTriggered_ = false;
	/// HUDテキスト描画に使用するフォントハンドル。
	uint32_t hudFontHandle_ = 0;

	/// 所持キャラクターのインデックスから表示名を取得する。
	const std::u32string& GetCharacterNameByIndex(int characterIndex) const;
};
#pragma once
#include "Object/Characters/Playable/Base/PlayableBase.h"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class Team {
public:
	static constexpr int kMaxMembersCount = 4;

	/// チーム情報とチーム画面を初期化する。
	void Initialize();
	/// チームのショートカット入力を更新する。
	void Update();
	/// 指定したスロットにメンバーが設定されているかを返す。
	bool GetHasMember(int slotIndex) const;
	/// 現在操作対象になっているチームスロット番号を返す。
	int GetActiveSlot() const { return activeSlotIndex_; }
	/// 指定スロットに割り当てられている所持キャラクターのインデックスを返す。
	int GetMemberCharacterIndex(int slotIndex) const;
	/// 所持キャラクターのアイコンテクスチャハンドルを返す。
	uint32_t GetOwnedCharacterIconHandle(int characterIndex) const;
	/// 所持キャラクター数を返す。
	int GetOwnedCharacterCount() const { return static_cast<int>(ownedCharacterIconHandles_.size()); }
	/// 指定キャラクターを指定スロットへ割り当てる。
	void AssignCharacterToSlot(int slotIndex, int characterIndex);
	/// 現在操作対象のチームスロットを変更する。
	void SetActiveSlot(int slotIndex);
	/// キャラクター切り替え要求を取得し、取得後にフラグをリセットする。
	bool ConsumeCharacterSwitchTriggered();
	/// 現在操作中のキャラクター名を文字列で返す。
	std::string GetActiveCharacterName() const;
	/// 現在操作中のキャラクターにダメージを与える。
	void DamageActiveCharacter(int amount);
	/// 現在操作中のキャラクターが生存しているかを返す。
	bool GetIsActiveCharacterAlive() const;
	/// 指定スロットのキャラクターが生存しているかを返す。
	bool GetIsMemberAlive(int slotIndex) const;
	/// チーム内の全メンバーが戦闘不能かを返す。
	bool GetAreAllMembersDead() const;
	/// 現在スロットの次にいる生存キャラクターへ切り替える。
	bool SwitchToNextAliveMember();
	/// 現在操作中キャラクターの現在HPを返す。
	int GetActiveCharacterHP() const;
	/// 現在操作中キャラクターの最大HPを返す。
	int GetActiveCharacterHPMax() const;
	/// 指定スロットの現在HPを返す。
	int GetMemberHP(int slotIndex) const;
	/// 指定スロットの最大HPを返す。
	int GetMemberHPMax(int slotIndex) const;
	/// 所持キャラクターのインデックスから表示名を取得する。
	const std::u32string& GetCharacterNameByIndex(int characterIndex) const;
	/// 所持キャラクターのインデックスからプレイアブル識別名を取得する。
	std::string GetPlayableNameByIndex(int characterIndex) const;

private:
	/// 所持しているプレイアブルキャラクター実体の一覧。
	std::vector<std::unique_ptr<PlayableBase>> ownedCharacters_{};
	/// 所持キャラクターのアイコンテクスチャハンドル一覧。
	std::vector<uint32_t> ownedCharacterIconHandles_{};
	/// 所持キャラクターから取得した表示名一覧。
	std::vector<std::u32string> ownedCharacterDisplayNames_{};
	/// 各チームスロットに割り当てられている所持キャラクターのインデックス。
	std::array<int, kMaxMembersCount> teamMemberCharacterIndices_{};
	/// 各チームスロットが使用中かどうかを示すフラグ。
	std::array<bool, kMaxMembersCount> occupiedSlots_{};
	/// 各チームスロットに割り当てられているキャラクターの現在HP。
	std::array<int, kMaxMembersCount> memberHP_{};
	/// 各チームスロットに割り当てられているキャラクターの最大HP。
	std::array<int, kMaxMembersCount> memberHPMax_{};
	/// 現在操作対象になっているチームスロット番号。
	int activeSlotIndex_ = 0;
	/// 操作キャラクターの切り替えが発生したことを通知するフラグ。
	bool characterSwitchTriggered_ = false;
};
#pragma once
#include "Object/Player/PlayerParameters.h"
#include "Sprite.h"
#include "UI/AttackOperation/AttackOperation.h"
#include "UI/DashGauge/DashGauge.h"
#include "UI/HPBar/HPBarUI.h"
#include "UI/Menu/Menu.h"
#include "UI/PadMenu/PadMenu.h"
#include "UI/Team/TeamUI.h"
#include "UI/Tower/TowerUI.h"
#include "UI/Attribute/AttributeUI.h"

class Team;
class Player;
class UIManager {

	// プレイヤーパラメータの保持。
	Parameters parameters_;

	// カーソルスプライト
	std::unique_ptr<Sprite> cursolSprite_;
	// コントローラースプライト
	std::unique_ptr<Sprite> controllerSprite_;
	// プレイヤーのHPバーUI
	std::unique_ptr<HPBarUI> hpBarUI_;
	// 攻撃の操作UI
	std::unique_ptr<AttackOperation> attackOperationUI_;
	// ダッシュゲージUI
	std::unique_ptr<DashGauge> dashGaugeUI_;
	// 拠点UI
	std::unique_ptr<TowerUI> towerUI_;
	// メニューUI
	std::unique_ptr<Menu> menuUI_;
	// チームUI
	std::unique_ptr<TeamUI> teamUI_;
	// パッドメニューUI
	std::unique_ptr<PadMenu> padMenuUI_;
	// 属性メニューUI
	std::unique_ptr<AttributeUI> attributeUI_;

	// チーム
	Team* team_ = nullptr;
	// プレイヤー
	Player* player_;

public:
	// 生成時にUIリソースを読み込む。
	UIManager();
	// 破棄時にリソースを解放する。
	~UIManager();
	// UI表示の初期化を行う。
	void Initialize();
	// UI状態を更新する。
	void Update();
	// UIを描画する。
	void Draw();
	// チーム情報を受け取る
	void SetTeam(Team* team);
	// プレイヤー情報を受けとる
	void SetPlayer(Player* player);

	// 現在のプレイヤーHPを設定する。
	void SetPlayerHP(int HP);
	// プレイヤーHP最大値を設定する。
	void SetPlayerHPMax(int HPMax);
	// プレイヤーパラメータを設定する。
	void SetPlayerParameters(Parameters parameters);
	// 現在のダッシュゲージを設定する。
	void SetPlayerDashGauge(float dashGauge, float dashGaugeMax, bool isDashUIView);
	// 必殺技のクールタイムを受け取る
	void SetSpecialAttackCooldown(float remainingSeconds);
};
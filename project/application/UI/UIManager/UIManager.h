#pragma once
#include "Object/Player/PlayerParameters.h"
#include "Sprite.h"
#include "UI/AttackOperation/AttackOperation.h"
#include "UI/DashGauge/DashGauge.h"
#include "UI/HPBar/HPBarUI.h"
#include "UI/Menu/Menu.h"
#include "UI/Team/TeamUI.h"
#include "UI/Tower/TowerUI.h"
#include "UI/PadMenu/PadMenu.h"

class GameBase;
class UIManager {

	// プレイヤーパラメータの保持領域。
	Parameters parameters_;

	std::unique_ptr<Sprite> cursolSprite_;
	std::unique_ptr<Sprite> controllerSprite_;

	std::unique_ptr<HPBarUI> hpBarUI_;
	std::unique_ptr<AttackOperation> attackOperationUI_;
	std::unique_ptr<DashGauge> dashGaugeUI_;
	std::unique_ptr<TowerUI> towerUI_;
	std::unique_ptr<Menu> menuUI_;
	std::unique_ptr<TeamUI> teamUI_;
	std::unique_ptr<PadMenu> padMenuUI_;
	Team* team_ = nullptr;

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
	void SetTeam(Team* team);

	// 現在のプレイヤーHPを設定する。
	void SetPlayerHP(int HP);
	// プレイヤーHP最大値を設定する。
	void SetPlayerHPMax(int HPMax);
	// プレイヤーパラメータを設定する。
	void SetPlayerParameters(Parameters parameters);
	// 現在のダッシュゲージを設定する。
	void SetPlayerDashGauge(float dashGauge, float dashGaugeMax, bool isDashUIView);
	void SetSpecialAttackCooldown(float remainingSeconds);
};

#pragma once

#include <vector>
#include <wrl.h>
#include <xapofx.h>
#include <xaudio2.h>
#include <xaudio2fx.h>

// 複数の音声をサブミックスボイスへ集約し、共通のエフェクトを適用する。
// ソースボイス → サブミックスボイス → マスターボイスの順で音声を送る。
class AudioMixer {
public:
	// 対応しているエフェクトの種類。
	enum class EffectType {
		Reverb,
		Echo,
		Equalizer,
		Limiter,
	};

	// エフェクト1つ分の設定。
	// typeで指定した種類に対応するパラメーターを使用する。
	struct EffectSettings {
		// 使用するエフェクトの種類と有効状態。
		EffectType type = EffectType::Reverb;
		bool enabled = true;

		// 各エフェクトに渡すパラメーター。
		XAUDIO2FX_REVERB_PARAMETERS reverb{};
		FXECHO_PARAMETERS echo{};
		FXEQ_PARAMETERS equalizer{};
		FXMASTERINGLIMITER_PARAMETERS limiter{};

		// 各エフェクトの初期パラメーターを設定する。
		EffectSettings();
	};

	AudioMixer() = default;

	// デストラクターからFinalizeは呼ばれないため、終了時に明示的に呼び出す。
	~AudioMixer() = default;

	// マスターボイスのチャンネル数とサンプルレートに合わせて、
	// サブミックスボイスを作成する。
	void Initialize(IXAudio2* xAudio2, IXAudio2MasteringVoice* masterVoice);

	// エフェクトの参照とサブミックスボイスを解放する。
	void Finalize();

	// サブミックスボイスが作成済みかを取得する。
	bool IsReady() const { return mixerVoice_ != nullptr; }

	// ソースボイスの出力先として設定する送信先情報を取得する。
	// 初期化されていなければnullptrを返す。
	const XAUDIO2_VOICE_SENDS* GetOutputVoiceSends() const;

	// エフェクト一覧を置き換え、チェーンを再構築する。
	void SetEffects(const std::vector<EffectSettings>& effects);

	// エフェクトを一覧の末尾へ追加する。
	void AddEffect(const EffectSettings& effect);

	// 指定位置のエフェクトを一覧から削除する。
	void RemoveEffect(size_t index);

	// エフェクト一覧を空にし、適用中のチェーンを取り外す。
	void ClearEffects();

	// エフェクト設定の一覧をコピーして取得する。
	std::vector<EffectSettings> GetEffects() const;

	// エフェクトの種類を表示用の名前へ変換する。
	static const char* GetEffectTypeName(EffectType type);

private:
	// 有効な設定からエフェクトを生成し、サブミックスボイスへ適用する。
	void RebuildEffectChain();

	// 複数の音声を受け取り、共通のエフェクト処理を行うボイス。
	IXAudio2SubmixVoice* mixerVoice_ = nullptr;

	// エフェクトの設定一覧。並び順がチェーンの処理順になる。
	std::vector<EffectSettings> effects_;

	// 生成した音声処理オブジェクトのCOM参照を保持する。
	std::vector<Microsoft::WRL::ComPtr<IUnknown>> effectInstances_;

	// ソースボイスからサブミックスボイスへ音声を送るための設定。
	XAUDIO2_SEND_DESCRIPTOR sendDescriptor_{};
	XAUDIO2_VOICE_SENDS sendList_{};
};
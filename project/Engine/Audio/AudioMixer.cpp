#include "AudioMixer.h"

#include <assert.h>
#include <combaseapi.h>

// COM関連の処理で使用するライブラリ。
#pragma comment(lib, "ole32.lib")

// 各エフェクトの初期パラメーターを設定する。
// 実際に使用するパラメーターは、typeで指定した種類によって決まる。
AudioMixer::EffectSettings::EffectSettings() {
	type = EffectType::Reverb;
	enabled = true;

	// 変化を感じやすいよう、既定は広い空間系のリバーブにする
	const XAUDIO2FX_REVERB_I3DL2_PARAMETERS preset = XAUDIO2FX_I3DL2_PRESET_AUDITORIUM;

	// I3DL2形式のプリセットを、XAudio2のリバーブ用パラメーターへ変換する。
	ReverbConvertI3DL2ToNative(&preset, &reverb);
	reverb.WetDryMix = 65.0f;

	// 既定値を強めて、ON 時に違いが出やすくする
	echo.WetDryMix = 60.0f;
	echo.Feedback = 65.0f;
	echo.Delay = 800.0f;

	// 4つの帯域について、中心周波数・ゲイン・帯域幅を設定する。
	equalizer.FrequencyCenter0 = 800.0f;
	equalizer.Gain0 = 4.5f;
	equalizer.Bandwidth0 = 18.0f;

	equalizer.FrequencyCenter1 = 2000.0f;
	equalizer.Gain1 = -5.0f;
	equalizer.Bandwidth1 = 18.0f;

	equalizer.FrequencyCenter2 = 8000.0f;
	equalizer.Gain2 = 6.0f;
	equalizer.Bandwidth2 = 18.0f;

	equalizer.FrequencyCenter3 = 12000.0f;
	equalizer.Gain3 = 3.0f;
	equalizer.Bandwidth3 = 18.0f;

	// リミッターにはAPIで定義された既定値を使用する。
	limiter.Release = FXMASTERINGLIMITER_DEFAULT_RELEASE;
	limiter.Loudness = FXMASTERINGLIMITER_DEFAULT_LOUDNESS;
}

// 音声を集約するサブミックスボイスと、そこへ送信するための情報を作成する。
void AudioMixer::Initialize(IXAudio2* xAudio2, IXAudio2MasteringVoice* masterVoice) {
	if (!xAudio2 || !masterVoice) {
		return;
	}

	// 初期化済みなら、重複して作成しない。
	if (mixerVoice_) {
		return;
	}

	// 出力先に合わせるため、マスターボイスの音声形式を取得する。
	XAUDIO2_VOICE_DETAILS masterVoiceDetails{};
	masterVoice->GetVoiceDetails(&masterVoiceDetails);

	// 送信先をnullptrにすると、既定でマスターボイスへ出力される。
	HRESULT hr = xAudio2->CreateSubmixVoice(&mixerVoice_, masterVoiceDetails.InputChannels, masterVoiceDetails.InputSampleRate, 0, 0, nullptr, nullptr);
	assert(SUCCEEDED(hr));

	// ソースボイス側から、このサブミックスボイスを出力先に指定できるようにする。
	sendDescriptor_.Flags = 0;
	sendDescriptor_.pOutputVoice = mixerVoice_;
	sendList_.SendCount = 1;
	sendList_.pSends = &sendDescriptor_;

	// 初期化前に設定されていたエフェクトがあれば適用する。
	RebuildEffectChain();
}

// 保持しているCOM参照とボイスを解放し、送信先情報を初期状態へ戻す。
void AudioMixer::Finalize() {
	effectInstances_.clear();

	if (mixerVoice_) {
		mixerVoice_->DestroyVoice();
		mixerVoice_ = nullptr;
	}

	sendDescriptor_ = {};
	sendList_ = {};
}

// ソースボイス作成時などに使用する、ミキサーへの送信先情報を返す。
const XAUDIO2_VOICE_SENDS* AudioMixer::GetOutputVoiceSends() const {
	if (!mixerVoice_) {
		return nullptr;
	}
	return &sendList_;
}

// 一覧全体を置き換え、その内容でエフェクトチェーンを作り直す。
void AudioMixer::SetEffects(const std::vector<EffectSettings>& effects) {
	effects_ = effects;
	RebuildEffectChain();
}

// 一覧の末尾へ追加する。
// この関数では、同じ種類のエフェクトがすでに存在するかは確認しない。
void AudioMixer::AddEffect(const EffectSettings& effect) {
	effects_.push_back(effect);
	RebuildEffectChain();
}

// 指定した位置の設定を削除する。
// 範囲外のインデックスが渡された場合は何もしない。
void AudioMixer::RemoveEffect(size_t index) {
	if (index >= effects_.size()) {
		return;
	}

	effects_.erase(effects_.begin() + index);
	RebuildEffectChain();
}

// 設定一覧を空にし、ボイスからエフェクトチェーンを取り外す。
void AudioMixer::ClearEffects() {
	effects_.clear();
	RebuildEffectChain();
}

// 設定一覧を値として返す。
// 取得した一覧を変更しても、ミキサー内部の設定は変更されない。
std::vector<AudioMixer::EffectSettings> AudioMixer::GetEffects() const { return effects_; }

// エディターなどで表示するための名前を返す。
const char* AudioMixer::GetEffectTypeName(EffectType type) {
	switch (type) {
	case EffectType::Reverb:
		return "Reverb";
	case EffectType::Echo:
		return "Echo";
	case EffectType::Equalizer:
		return "EQ";
	case EffectType::Limiter:
		return "Limiter";
	default:
		return "Unknown";
	}
}

// 設定一覧から、有効なエフェクトだけを生成して順番に適用する。
void AudioMixer::RebuildEffectChain() {
	// 初期化前なら設定だけを保持し、実際の適用は行わない。
	if (!mixerVoice_) {
		return;
	}

	XAUDIO2_VOICE_DETAILS voiceDetails{};
	mixerVoice_->GetVoiceDetails(&voiceDetails);

	// 各エフェクトの出力チャンネル数を、ボイスの入力チャンネル数に合わせる。
	const UINT32 outputChannels = voiceDetails.InputChannels > 0 ? voiceDetails.InputChannels : 1;

	// このクラスが保持している、以前のエフェクトへのCOM参照を解放する。
	effectInstances_.clear();

	// APIへ渡すディスクリプタと、実際に適用する設定の対応を保持する。
	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> descriptors;
	std::vector<const EffectSettings*> appliedEffects;
	descriptors.reserve(effects_.size());
	effectInstances_.reserve(effects_.size());
	appliedEffects.reserve(effects_.size());

	for (const auto& effect : effects_) {
		// 無効な設定は、一覧に残したまま適用対象から外す。
		if (!effect.enabled) {
			continue;
		}

		// 種類に対応する音声処理オブジェクトを生成する。
		Microsoft::WRL::ComPtr<IUnknown> xapo;
		HRESULT hr = S_OK;

		switch (effect.type) {
		case EffectType::Reverb:
			hr = XAudio2CreateReverb(&xapo, 0);
			break;
		case EffectType::Echo:
			hr = CreateFX(__uuidof(FXEcho), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		case EffectType::Equalizer:
			hr = CreateFX(__uuidof(FXEQ), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		case EffectType::Limiter:
			hr = CreateFX(__uuidof(FXMasteringLimiter), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		default:
			hr = E_FAIL;
			break;
		}

		// 生成に失敗したエフェクトは追加せず、次の設定へ進む。
		if (FAILED(hr) || !xapo) {
			continue;
		}

		// COM参照を保持し、チェーンに登録する情報を追加する。
		effectInstances_.push_back(xapo);
		descriptors.push_back({xapo.Get(), TRUE, outputChannels});

		// チェーン内の番号と設定の対応を記録する。
		appliedEffects.push_back(&effect);
	}

	// 適用対象がなければ、既存のエフェクトチェーンを取り外す。
	if (descriptors.empty()) {
		HRESULT hr = mixerVoice_->SetEffectChain(nullptr);
		assert(SUCCEEDED(hr));
		return;
	}

	// ディスクリプタの並び順で、エフェクトチェーンを設定する。
	XAUDIO2_EFFECT_CHAIN chain{};
	chain.EffectCount = static_cast<UINT32>(descriptors.size());
	chain.pEffectDescriptors = descriptors.data();

	HRESULT hr = mixerVoice_->SetEffectChain(&chain);
	assert(SUCCEEDED(hr));

	// 各エフェクトへ、その種類に対応するパラメーターを渡す。
	// effectIndexは元の設定一覧ではなく、実際のチェーン内の番号。
	for (UINT32 effectIndex = 0; effectIndex < appliedEffects.size(); ++effectIndex) {
		const auto& effect = *appliedEffects[effectIndex];

		switch (effect.type) {
		case EffectType::Reverb:
			hr = mixerVoice_->SetEffectParameters(effectIndex, &effect.reverb, sizeof(effect.reverb));
			break;
		case EffectType::Echo:
			hr = mixerVoice_->SetEffectParameters(effectIndex, &effect.echo, sizeof(effect.echo));
			break;
		case EffectType::Equalizer:
			hr = mixerVoice_->SetEffectParameters(effectIndex, &effect.equalizer, sizeof(effect.equalizer));
			break;
		case EffectType::Limiter:
			hr = mixerVoice_->SetEffectParameters(effectIndex, &effect.limiter, sizeof(effect.limiter));
			break;
		default:
			hr = E_FAIL;
			break;
		}

		assert(SUCCEEDED(hr));
	}
}
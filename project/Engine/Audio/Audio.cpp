#include "Audio.h"
#include "fstream"
#include <algorithm>
#include <array>
#include <assert.h>
#include <combaseapi.h>
#include <filesystem>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <string>
#include <utility>

// 音声再生と、Media Foundationによる音声ファイルの読み込みに使用する。
#pragma comment(lib, "xAudio2.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")

namespace {

// 現在存在するSoundDataのアドレスを保持する。
// エディターでサウンド一覧を表示するときに使用する。
// ポインターを登録するだけで、SoundDataの所有権は持たない。
std::vector<SoundData*>& SoundDataRegistry() {
	static std::vector<SoundData*> instances;
	return instances;
}

// バッファのバイト数から、音声全体のサンプルフレーム数を求める。
// nBlockAlignは、全チャンネルの1サンプルフレームに必要なバイト数。
UINT64 CalculateTotalSamples(const SoundData& soundData) {
	if (soundData.wfex.nBlockAlign == 0) {
		return 0;
	}
	return static_cast<UINT64>(soundData.buffer.size() / soundData.wfex.nBlockAlign);
}

// エフェクト一覧を、Reverb → Echo → Equalizer → Limiterの順へ揃える。
// 存在しない種類は無効状態で追加する。
// 同じ種類が複数ある場合は、最初に見つかった設定を採用する。
std::vector<Audio::MixerEffectSettings> NormalizeEffects(const std::vector<Audio::MixerEffectSettings>& effects) {
	const std::array<Audio::MixerEffectType, 4> orderedTypes = {
	    Audio::MixerEffectType::Reverb,
	    Audio::MixerEffectType::Echo,
	    Audio::MixerEffectType::Equalizer,
	    Audio::MixerEffectType::Limiter,
	};

	std::vector<Audio::MixerEffectSettings> normalized;
	normalized.reserve(orderedTypes.size());

	for (Audio::MixerEffectType type : orderedTypes) {
		auto found = std::find_if(effects.begin(), effects.end(), [type](const Audio::MixerEffectSettings& effect) { return effect.type == type; });

		if (found != effects.end()) {
			normalized.push_back(*found);
			continue;
		}

		// 未登録の種類も、設定枠として残しておく。
		Audio::MixerEffectSettings effect{};
		effect.type = type;
		effect.enabled = false;
		normalized.push_back(effect);
	}
	return normalized;
}

// 指定した種類のエフェクトについて、有効・無効だけを変更する。
void SetEffectEnabled(std::vector<Audio::MixerEffectSettings>& effects, Audio::MixerEffectType type, bool enabled) {
	auto normalized = NormalizeEffects(effects);
	auto target = std::find_if(normalized.begin(), normalized.end(), [type](const Audio::MixerEffectSettings& effect) { return effect.type == type; });

	if (target != normalized.end()) {
		target->enabled = enabled;
	}

	effects = std::move(normalized);
}

} // namespace

// 通常生成時に、サウンド一覧へ自身を登録する。
SoundData::SoundData() { RegisterInstance(); }

// 音声データと設定をコピーし、新しいインスタンスとして登録する。
SoundData::SoundData(const SoundData& other) : wfex(other.wfex), waveFormatBlob(other.waveFormatBlob), buffer(other.buffer), volume(other.volume), debugName(other.debugName), effects(other.effects) {
	RegisterInstance();
}

// バッファや文字列などの所有権を移し、新しいインスタンスとして登録する。
// 移動元も破棄されるまでは、登録されたインスタンスとして残る。
SoundData::SoundData(SoundData&& other) noexcept
    : wfex(other.wfex), waveFormatBlob(std::move(other.waveFormatBlob)), buffer(std::move(other.buffer)), volume(other.volume), debugName(std::move(other.debugName)),
      effects(std::move(other.effects)) {
	RegisterInstance();

	// 移動元のフォーマットや設定を初期状態へ戻す。
	other.wfex = {};
	other.waveFormatBlob.clear();
	other.volume = 1.0f;
	other.effects.clear();
}

// 既存のインスタンスへ、音声データと設定をコピーする。
// インスタンスのアドレスは変わらないため、再登録は行わない。
SoundData& SoundData::operator=(const SoundData& other) {
	if (this == &other) {
		return *this;
	}

	wfex = other.wfex;
	waveFormatBlob = other.waveFormatBlob;
	buffer = other.buffer;
	volume = other.volume;
	debugName = other.debugName;
	effects = other.effects;
	return *this;
}

// 既存のインスタンスへ、バッファなどの所有権を移す。
SoundData& SoundData::operator=(SoundData&& other) noexcept {
	if (this == &other) {
		return *this;
	}

	wfex = other.wfex;
	waveFormatBlob = std::move(other.waveFormatBlob);
	buffer = std::move(other.buffer);
	volume = other.volume;
	debugName = std::move(other.debugName);
	effects = std::move(other.effects);

	other.wfex = {};
	other.waveFormatBlob.clear();
	other.volume = 1.0f;
	return *this;
}

// 破棄されるインスタンスを一覧から取り除く。
SoundData::~SoundData() { UnregisterInstance(); }

// 現在登録されているSoundDataの一覧を取得する。
const std::vector<SoundData*>& SoundData::GetInstances() { return SoundDataRegistry(); }

// 拡張情報を含むフォーマットがあれば、そちらを優先して返す。
// 拡張情報がなければ、通常のWAVEFORMATEXを返す。
const WAVEFORMATEX* SoundData::GetWaveFormat() const {
	if (!waveFormatBlob.empty()) {
		return reinterpret_cast<const WAVEFORMATEX*>(waveFormatBlob.data());
	}
	return &wfex;
}

// インスタンス一覧へ自身のアドレスを追加する。
void SoundData::RegisterInstance() { SoundDataRegistry().push_back(this); }

// インスタンス一覧から自身のアドレスを削除する。
void SoundData::UnregisterInstance() {
	auto& instances = SoundDataRegistry();
	instances.erase(std::remove(instances.begin(), instances.end(), this), instances.end());
}

// Audio シングルトンインスタンス
std::unique_ptr<Audio> Audio::instance = nullptr;

// シングルトンインスタンスを返す
Audio* Audio::GetInstance() {
	if (instance == nullptr) {
		instance = std::make_unique<Audio>();
	}
	return instance.get();
}

// 再生中ボイスとオーディオAPIを終了する
void Audio::Finalize() {
	// ソースボイスを先に停止・破棄してから、出力側を解放する。
	StopAllVoices();
	mixer_.Finalize();

	if (masterVoice_) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}

	xAudio2_.Reset();

	// 音声ファイルの読み込みに使用したMedia Foundationを終了する。
	result_ = MFShutdown();
	assert(SUCCEEDED(result_));

	// このクラスで成功したCOM初期化に対応する終了処理を行う。
	if (comInitialized_) {
		CoUninitialize();
		comInitialized_ = false;
	}

	// 自身も破棄されるため、この後はメンバーへアクセスしない。
	instance = nullptr;
}

// Media Foundation / XAudio2 を初期化する
void Audio::InitializeIXAudio() {
	result_ = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	if (SUCCEEDED(result_)) {
		comInitialized_ = true;
	} else if (result_ == RPC_E_CHANGED_MODE) {
		// 既に別モデルで初期化済みでも COM は利用可能
		comInitialized_ = false;
		result_ = S_OK;
	} else {
		assert(SUCCEEDED(result_));
	}

	// ファイルのデコードに使用するMedia Foundationを開始する。
	result_ = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result_));

	// XAudio2本体と、最終的な出力先であるマスターボイスを作成する。
	result_ = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result_));

	result_ = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result_));

	// 各サウンドをまとめて処理するミキサーを初期化する。
	mixer_.Initialize(xAudio2_.Get(), masterVoice_);
}

// ワンショット再生が終わったボイスを回収する
void Audio::Update() {
	if (activeVoices_.empty()) {
		return;
	}

	for (auto& active : activeVoices_) {
		if (!active.voice) {
			continue;
		}

		// ループ音声は自動回収せず、明示的な停止処理で終了する。
		if (active.isLoop) {
			continue;
		}

		XAUDIO2_VOICE_STATE state{};
		active.voice->GetState(&state);

		// 再生済みサンプル数から、元の音声の再生終了を記録する。
		if (!active.sourceEnded && active.totalSamples > 0 && state.SamplesPlayed >= active.totalSamples) {
			active.sourceEnded = true;
		}

		// キュー内の音声バッファがなくなったボイスを破棄する。
		if (state.BuffersQueued == 0) {
			active.voice->DestroyVoice();
			active.voice = nullptr;
		}
	}

	// 破棄済みのボイス情報を管理一覧から取り除く。
	activeVoices_.erase(std::remove_if(activeVoices_.begin(), activeVoices_.end(), [](const ActiveVoice& active) { return active.voice == nullptr; }), activeVoices_.end());
}

// 音声ファイルを PCM として読み込んで SoundData を生成する
SoundData Audio::SoundLoadFile(const char* filename) {
	// フルパス → UTF-16 変換
	// 渡されたUTF-8のパスを、Windows API用のワイド文字列へ変換する。
	std::wstring filePathW;
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, filename, -1, nullptr, 0);
		filePathW.resize(size);
		MultiByteToWideChar(CP_UTF8, 0, filename, -1, &filePathW[0], size);
	}

	HRESULT hr;
	Microsoft::WRL::ComPtr<IMFSourceReader> pReader;

	// SourceReader 作成
	hr = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
	assert(SUCCEEDED(hr));

	// XAPOFX(Echo / EQ など) は FLOAT32 入力を前提とするため、
	// 読み込み時点で IEEE Float へデコードしておく
	Microsoft::WRL::ComPtr<IMFMediaType> pPCMType;
	hr = MFCreateMediaType(&pPCMType);
	assert(SUCCEEDED(hr));

	hr = pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	assert(SUCCEEDED(hr));

	hr = pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float);
	assert(SUCCEEDED(hr));

	// 最初の音声ストリームを、指定した形式で読み出すよう設定する。
	hr = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, pPCMType.Get());
	assert(SUCCEEDED(hr));

	// 実際に適用されたメディアタイプ
	Microsoft::WRL::ComPtr<IMFMediaType> pOutType;
	hr = pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);
	assert(SUCCEEDED(hr));

	// WAVEFORMATEX / WAVEFORMATEXTENSIBLE を取り出す
	WAVEFORMATEX* waveFormat = nullptr;
	UINT32 waveFormatSize = 0;
	hr = MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, &waveFormatSize);
	assert(SUCCEEDED(hr));

	// SoundData へ格納
	SoundData soundData{};
	soundData.wfex = *waveFormat; // WAVEFORMATEX コピー

	// 拡張フォーマット情報も失わないよう、全体をバイト列として保持する。
	soundData.waveFormatBlob.assign(reinterpret_cast<const BYTE*>(waveFormat), reinterpret_cast<const BYTE*>(waveFormat) + waveFormatSize);

	// Media Foundation側で確保されたフォーマット情報を解放する。
	CoTaskMemFree(waveFormat);

	// 読み込んだ PCM データを格納するバッファ
	// 現在の実装では、読み込み先としてsoundData.bufferを直接使用している。
	std::vector<BYTE> buffer;

	// 1フレームずつ読み込む
	// ReadSampleで返される音声サンプルのまとまりを、終端まで順に取得する。
	while (true) {
		DWORD streamIndex = 0, flags = 0;
		LONGLONG llTimeStamp = 0;
		Microsoft::WRL::ComPtr<IMFSample> pSample;

		hr = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);

		assert(SUCCEEDED(hr));

		if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
			break;

		if (!pSample) {
			continue;
		} else {
			// サンプル内のデータを、連続した1つのバッファとして取得する。
			Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
			hr = pSample->ConvertToContiguousBuffer(&pBuffer);
			assert(SUCCEEDED(hr));

			BYTE* pData = nullptr;
			DWORD maxLength = 0, curLength = 0;

			// Lock中にデータをコピーし、コピー後にUnlockする。
			hr = pBuffer->Lock(&pData, &maxLength, &curLength);
			assert(SUCCEEDED(hr));

			soundData.buffer.insert(soundData.buffer.end(), pData, pData + curLength);

			pBuffer->Unlock();
		}
	}

	// エディター表示用の名前には、パスを除いたファイル名を使用する。
	soundData.debugName = std::filesystem::path(filename).filename().string();

	return soundData;
}

// 指定サウンドに紐づく再生を止めてメモリを解放する
void Audio::SoundUnload(SoundData* soundData) {
	if (!soundData) {
		return;
	}

	// ボイスが参照している音声バッファを消す前に、その再生を停止する。
	StopVoicesForSound(*soundData);
	soundData->buffer.clear();
	soundData->wfex = {};
	soundData->waveFormatBlob.clear();
	soundData->effects.clear();
}

// サウンドを再生する(必要ならループ再生)
// playbackSourceで、シーン由来かエディター試聴由来かを区別する。
void Audio::SoundPlayWaveInternal(const SoundData& soundData, bool isLoop, ActiveVoice::PlaybackSource playbackSource) {
	if (!xAudio2_) {
		OutputDebugStringA("SoundPlayWave: xAudio2 is null!\n");
		return;
	}

	if (soundData.buffer.empty()) {
		OutputDebugStringA("SoundPlayWave: sound buffer is empty!\n");
		return;
	}

	// 再生ごとにソースボイスを作るため、同じ音声を重ねて再生できる。
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	const XAUDIO2_VOICE_SENDS* sendList = mixer_.GetOutputVoiceSends();
	const WAVEFORMATEX* waveFormat = soundData.GetWaveFormat();

	// ミキサーがあればそこへ送り、なければ既定の出力経路を使用する。
	if (sendList) {
		result_ = xAudio2_->CreateSourceVoice(&pSourceVoice, waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, sendList, nullptr);
	} else {
		result_ = xAudio2_->CreateSourceVoice(&pSourceVoice, waveFormat);
	}

	assert(SUCCEEDED(result_));

	if (FAILED(result_) || !pSourceVoice) {
		OutputDebugStringA("SoundPlayWave: CreateSourceVoice failed.\n");
		return;
	}

	pSourceVoice->SetVolume(soundData.volume);

	// XAudio2へ渡す音声データの範囲を設定する。
	// 再生中は、このバッファが有効な状態を維持する必要がある。
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());

	if (isLoop) {
		// 全体を無限に繰り返す。
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
		buf.Flags = 0;
	} else {
		// このバッファを音声ストリームの終端として扱う。
		buf.Flags = XAUDIO2_END_OF_STREAM;
	}

	result_ = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result_));

	// 停止、終了判定、設定変更に使用する管理情報を作成する。
	ActiveVoice activeVoice{};
	activeVoice.voice = pSourceVoice;
	activeVoice.audioData = soundData.buffer.data();
	activeVoice.isLoop = isLoop;
	activeVoice.sourceEnded = false;
	activeVoice.totalSamples = isLoop ? 0 : CalculateTotalSamples(soundData);
	activeVoice.playbackSource = playbackSource;

	// このサウンド専用のエフェクトを適用する。
	const bool effectApplied = ApplyEffectsToVoice(pSourceVoice, soundData.effects, activeVoice.effectInstances);
	assert(effectApplied);

	result_ = pSourceVoice->Start();
	assert(SUCCEEDED(result_));

	activeVoices_.push_back(std::move(activeVoice));
}

// シーン由来の音声として再生する。
// すでに同じサウンドが再生中でも、その再生は停止しない。
void Audio::SoundPlayWave(const SoundData& soundData, bool isLoop) { SoundPlayWaveInternal(soundData, isLoop, ActiveVoice::PlaybackSource::Scene); }

// 同じ音声の再生を停止してから、先頭から再生する。
void Audio::SoundPlayWaveFromStart(const SoundData& soundData, bool isLoop) {
	StopVoicesForSound(soundData);
	SoundPlayWave(soundData, isLoop);
}

// エディター試聴由来として、同じ音声を停止してから先頭再生する。
void Audio::SoundPlayPreviewFromStart(const SoundData& soundData, bool isLoop) {
	StopVoicesForSound(soundData);
	SoundPlayWaveInternal(soundData, isLoop, ActiveVoice::PlaybackSource::EditorPreview);
}

// 指定した音声を使用しているボイスを停止する。
void Audio::StopSound(const SoundData& soundData) { StopVoicesForSound(soundData); }

// 特定サウンドを再生しているボイスだけを停止する
void Audio::StopVoicesForSound(const SoundData& soundData) {
	// 音声バッファの先頭アドレスを、サウンドの識別に使用する。
	const BYTE* targetData = soundData.buffer.data();
	if (!targetData) {
		return;
	}

	for (auto& active : activeVoices_) {
		if (active.voice && active.audioData == targetData) {
			active.voice->Stop();
			active.voice->DestroyVoice();
			active.voice = nullptr;
		}
	}

	// 破棄したボイスの管理情報を削除する。
	activeVoices_.erase(std::remove_if(activeVoices_.begin(), activeVoices_.end(), [](const ActiveVoice& active) { return active.voice == nullptr; }), activeVoices_.end());
}

// サウンド音量を更新し、再生中ボイスにも反映する
void Audio::SetSoundVolume(SoundData* soundData, float volume) {
	if (!soundData) {
		return;
	}

	// 保存する音量を0～1に制限する。
	soundData->volume = std::clamp(volume, 0.0f, 1.0f);

	const BYTE* targetData = soundData->buffer.data();
	if (!targetData) {
		return;
	}

	// 同じサウンドを重ねて再生している場合は、すべてに反映する。
	for (auto& active : activeVoices_) {
		if (active.voice && active.audioData == targetData) {
			active.voice->SetVolume(soundData->volume);
		}
	}
}

// 指定したソースボイスに、有効なエフェクトを順番に適用する。
// 生成したエフェクトのCOM参照はoutInstancesで保持する。
bool Audio::ApplyEffectsToVoice(IXAudio2SourceVoice* voice, const std::vector<MixerEffectSettings>& effects, std::vector<Microsoft::WRL::ComPtr<IUnknown>>& outInstances) {
	if (!voice) {
		return false;
	}

	XAUDIO2_VOICE_DETAILS voiceDetails{};
	voice->GetVoiceDetails(&voiceDetails);

	// 各エフェクトの出力チャンネル数を、ボイスの入力チャンネル数に合わせる。
	const UINT32 outputChannels = voiceDetails.InputChannels > 0 ? voiceDetails.InputChannels : 1;

	outInstances.clear();

	std::vector<XAUDIO2_EFFECT_DESCRIPTOR> descriptors;
	std::vector<const MixerEffectSettings*> appliedEffects;
	descriptors.reserve(effects.size());
	appliedEffects.reserve(effects.size());

	for (const auto& effect : effects) {
		if (!effect.enabled) {
			continue;
		}

		// 設定された種類に対応する、音声処理オブジェクトを生成する。
		Microsoft::WRL::ComPtr<IUnknown> xapo;
		HRESULT hr = S_OK;

		switch (effect.type) {
		case MixerEffectType::Reverb:
			hr = XAudio2CreateReverb(&xapo, 0);
			break;
		case MixerEffectType::Echo:
			hr = CreateFX(__uuidof(FXEcho), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		case MixerEffectType::Equalizer:
			hr = CreateFX(__uuidof(FXEQ), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		case MixerEffectType::Limiter:
			hr = CreateFX(__uuidof(FXMasteringLimiter), reinterpret_cast<IUnknown**>(xapo.GetAddressOf()), nullptr, 0);
			break;
		default:
			hr = E_FAIL;
			break;
		}

		// 生成できなかったエフェクトは、チェーンへ追加しない。
		if (FAILED(hr) || !xapo) {
			continue;
		}

		outInstances.push_back(xapo);
		descriptors.push_back({xapo.Get(), TRUE, outputChannels});

		// 実際に追加したエフェクトと、その設定の対応を記録する。
		appliedEffects.push_back(&effect);
	}

	// 有効なエフェクトがなければ、既存のチェーンを取り外す。
	if (descriptors.empty()) {
		HRESULT hr = voice->SetEffectChain(nullptr);
		if (FAILED(hr)) {
			return false;
		}
		return true;
	}

	// ディスクリプタの順番でエフェクトチェーンを設定する。
	XAUDIO2_EFFECT_CHAIN chain{};
	chain.EffectCount = static_cast<UINT32>(descriptors.size());
	chain.pEffectDescriptors = descriptors.data();

	HRESULT hr = voice->SetEffectChain(&chain);
	if (FAILED(hr)) {
		return false;
	}

	// チェーン内の各エフェクトへ、種類に対応したパラメーターを渡す。
	for (UINT32 effectIndex = 0; effectIndex < appliedEffects.size(); ++effectIndex) {
		const auto& effect = *appliedEffects[effectIndex];

		switch (effect.type) {
		case MixerEffectType::Reverb:
			hr = voice->SetEffectParameters(effectIndex, &effect.reverb, sizeof(effect.reverb));
			break;
		case MixerEffectType::Echo:
			hr = voice->SetEffectParameters(effectIndex, &effect.echo, sizeof(effect.echo));
			break;
		case MixerEffectType::Equalizer:
			hr = voice->SetEffectParameters(effectIndex, &effect.equalizer, sizeof(effect.equalizer));
			break;
		case MixerEffectType::Limiter:
			hr = voice->SetEffectParameters(effectIndex, &effect.limiter, sizeof(effect.limiter));
			break;
		default:
			hr = E_FAIL;
			break;
		}

		if (FAILED(hr)) {
			return false;
		}
	}

	return true;
}

// 最新の音量・エフェクト設定を使い、再生中ボイスを作り直す。
// 再生位置は引き継がず、音声の先頭から再生する。
bool Audio::RecreateActiveVoice(ActiveVoice& active, const SoundData& soundData) {
	if (!active.voice) {
		return false;
	}

	// 新しいボイスの準備が完了するまで、古いボイスを保持する。
	IXAudio2SourceVoice* oldVoice = active.voice;
	IXAudio2SourceVoice* newVoice = nullptr;
	const XAUDIO2_VOICE_SENDS* sendList = mixer_.GetOutputVoiceSends();
	const WAVEFORMATEX* waveFormat = soundData.GetWaveFormat();

	if (sendList) {
		result_ = xAudio2_->CreateSourceVoice(&newVoice, waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, sendList, nullptr);
	} else {
		result_ = xAudio2_->CreateSourceVoice(&newVoice, waveFormat);
	}

	if (FAILED(result_) || !newVoice) {
		return false;
	}

	newVoice->SetVolume(soundData.volume);

	// 元のループ設定を引き継いで、音声バッファを登録する。
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());

	if (active.isLoop) {
		buf.LoopBegin = 0;
		buf.LoopLength = 0;
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
		buf.Flags = 0;
	} else {
		buf.Flags = XAUDIO2_END_OF_STREAM;
	}

	result_ = newVoice->SubmitSourceBuffer(&buf);
	if (FAILED(result_)) {
		newVoice->DestroyVoice();
		return false;
	}

	// 最新のサウンド専用エフェクトを適用する。
	if (!ApplyEffectsToVoice(newVoice, soundData.effects, active.effectInstances)) {
		newVoice->DestroyVoice();
		return false;
	}

	result_ = newVoice->Start();
	if (FAILED(result_)) {
		newVoice->DestroyVoice();
		return false;
	}

	// 新しいボイスの再生開始後に、古いボイスを停止・破棄する。
	oldVoice->Stop();
	oldVoice->DestroyVoice();

	active.voice = newVoice;
	active.audioData = soundData.buffer.data();
	return true;
}

// サウンド専用エフェクトを、種類と順番を揃えた一覧で取得する。
std::vector<Audio::MixerEffectSettings> Audio::GetSoundEffects(const SoundData* soundData) const {
	if (!soundData) {
		return {};
	}
	return NormalizeEffects(soundData->effects);
}

// サウンド専用エフェクトを置き換え、再生中のボイスにも反映する。
void Audio::SetSoundEffects(SoundData* soundData, const std::vector<MixerEffectSettings>& effects) {
	if (!soundData) {
		return;
	}

	soundData->effects = NormalizeEffects(effects);

	const BYTE* targetData = soundData->buffer.data();
	if (!targetData) {
		return;
	}

	// 同じ音声データを使用しているボイスを、それぞれ作り直す。
	for (auto& active : activeVoices_) {
		if (!active.voice || active.audioData != targetData) {
			continue;
		}

		if (!RecreateActiveVoice(active, *soundData)) {
			continue;
		}
	}
}

// 指定した種類の設定を置き換え、有効にする。
// 同じ種類のエフェクトを複数追加する処理ではない。
void Audio::AddSoundEffect(SoundData* soundData, const MixerEffectSettings& effect) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	auto target = std::find_if(effects.begin(), effects.end(), [&](const MixerEffectSettings& existing) { return existing.type == effect.type; });

	if (target != effects.end()) {
		*target = effect;
		target->enabled = true;
	}

	SetSoundEffects(soundData, effects);
}

// 一覧から設定枠を削除するのではなく、指定位置のエフェクトを無効にする。
void Audio::RemoveSoundEffect(SoundData* soundData, size_t index) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	if (index >= effects.size()) {
		return;
	}

	effects[index].enabled = false;
	SetSoundEffects(soundData, effects);
}

// パラメーターを保持したまま、サウンド専用エフェクトをすべて無効にする。
void Audio::ClearSoundEffects(SoundData* soundData) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);

	for (auto& effect : effects) {
		effect.enabled = false;
	}

	SetSoundEffects(soundData, effects);
}

// サウンド専用リバーブの有効・無効を変更する。
void Audio::SetReverb(SoundData* soundData, bool enabled) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	SetEffectEnabled(effects, MixerEffectType::Reverb, enabled);
	SetSoundEffects(soundData, effects);
}

// サウンド専用エコーの有効・無効を変更する。
void Audio::SetEcho(SoundData* soundData, bool enabled) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	SetEffectEnabled(effects, MixerEffectType::Echo, enabled);
	SetSoundEffects(soundData, effects);
}

// サウンド専用イコライザーの有効・無効を変更する。
void Audio::SetEqualizer(SoundData* soundData, bool enabled) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	SetEffectEnabled(effects, MixerEffectType::Equalizer, enabled);
	SetSoundEffects(soundData, effects);
}

// サウンド専用リミッターの有効・無効を変更する。
void Audio::SetLimiter(SoundData* soundData, bool enabled) {
	if (!soundData) {
		return;
	}

	auto effects = NormalizeEffects(soundData->effects);
	SetEffectEnabled(effects, MixerEffectType::Limiter, enabled);
	SetSoundEffects(soundData, effects);
}

// 現在存在する音声データから、エディター表示用の一覧を作成する。
std::vector<Audio::EditorSoundEntry> Audio::GetEditorSoundEntries() const {
	std::vector<EditorSoundEntry> entries;
	const auto& instances = SoundData::GetInstances();
	entries.reserve(instances.size());

	for (SoundData* soundData : instances) {
		// 音声データがないインスタンスは表示対象から外す。
		if (!soundData || soundData->buffer.empty()) {
			continue;
		}

		EditorSoundEntry entry{};
		entry.soundData = soundData;
		entry.name = soundData->debugName.empty() ? "(unnamed sound)" : soundData->debugName;

		// この音声データに対応する、再生中のボイスがあるか調べる。
		for (const auto& active : activeVoices_) {
			if (!active.voice || active.audioData != soundData->buffer.data() || active.sourceEnded) {
				continue;
			}

			entry.isPlaying = true;

			// 対応するボイスが1つでもループ中なら、ループ再生中として表示する。
			entry.isLoop = entry.isLoop || active.isLoop;
		}

		entries.push_back(std::move(entry));
	}

	return entries;
}

// ミキサー全体のエフェクトを、種類と順番を揃えた一覧で取得する。
std::vector<Audio::MixerEffectSettings> Audio::GetMixerEffects() const { return NormalizeEffects(mixer_.GetEffects()); }

// ミキサー全体のエフェクトを、種類と順番を揃えて設定する。
void Audio::SetMixerEffects(const std::vector<MixerEffectSettings>& effects) { mixer_.SetEffects(NormalizeEffects(effects)); }

// ミキサー全体について、指定した種類の設定を置き換えて有効にする。
void Audio::AddMixerEffect(const MixerEffectSettings& effect) {
	auto effects = NormalizeEffects(mixer_.GetEffects());
	auto target = std::find_if(effects.begin(), effects.end(), [&](const MixerEffectSettings& existing) { return existing.type == effect.type; });

	if (target != effects.end()) {
		*target = effect;
		target->enabled = true;
	}

	mixer_.SetEffects(effects);
}

// ミキサー全体について、指定位置のエフェクトを無効にする。
void Audio::RemoveMixerEffect(size_t index) {
	auto effects = NormalizeEffects(mixer_.GetEffects());
	if (index >= effects.size()) {
		return;
	}

	effects[index].enabled = false;
	mixer_.SetEffects(effects);
}

// ミキサー全体のエフェクトを、設定値を保持したまますべて無効にする。
void Audio::ClearMixerEffects() {
	auto effects = NormalizeEffects(mixer_.GetEffects());

	for (auto& effect : effects) {
		effect.enabled = false;
	}

	mixer_.SetEffects(effects);
}

// エフェクトの種類を、表示用の名前へ変換する。
const char* Audio::GetMixerEffectTypeName(MixerEffectType type) { return AudioMixer::GetEffectTypeName(type); }

// 指定した音声について、元音声が未終了のボイスが残っているか調べる。
// ボイスが存在しない場合や、すべて終了済みの場合はtrueを返す。
bool Audio::IsSoundFinished(const SoundData& soundData) const {
	const BYTE* targetData = soundData.buffer.data();
	if (!targetData) {
		return true;
	}

	for (const auto& active : activeVoices_) {
		if (active.voice && active.audioData == targetData && !active.sourceEnded) {
			return false;
		}
	}

	return true;
}

// 再生元の区別なく、すべてのソースボイスを停止・破棄する。
void Audio::StopAllVoices() {
	for (auto& active : activeVoices_) {
		if (!active.voice) {
			continue;
		}

		active.voice->Stop();
		active.voice->DestroyVoice();
		active.voice = nullptr;
	}

	activeVoices_.clear();
}

// シーン再生、エディター試聴など、指定した再生元のボイスだけを停止する。
void Audio::StopVoicesBySource(ActiveVoice::PlaybackSource playbackSource) {
	for (auto& active : activeVoices_) {
		if (!active.voice || active.playbackSource != playbackSource) {
			continue;
		}

		active.voice->Stop();
		active.voice->DestroyVoice();
		active.voice = nullptr;
	}

	// 停止・破棄したボイスの管理情報を取り除く。
	activeVoices_.erase(std::remove_if(activeVoices_.begin(), activeVoices_.end(), [](const ActiveVoice& active) { return active.voice == nullptr; }), activeVoices_.end());
}

// シーン由来の音声をすべて停止する。
void Audio::StopAllSceneSounds() { StopVoicesBySource(ActiveVoice::PlaybackSource::Scene); }

// エディター試聴由来の音声をすべて停止する。
void Audio::StopAllPreviewSounds() { StopVoicesBySource(ActiveVoice::PlaybackSource::EditorPreview); }
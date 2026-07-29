#pragma once
#include "Audio.h"
#include <array>
#include <cstddef>

/// <summary>
/// SEの処理の管理クラス
/// </summary>
class SEManager {
public:
	/// <summary>
	/// SEの種類。
	/// </summary>
	enum class SEType {
		NormalAttack,
		NormalAttack2,
		EndAttack,
		Magic,
		Count,
	};

	/// <summary>
	/// インスタンスを取得する。
	/// </summary>
	/// <returns></returns>
	static SEManager* GetInstance();

	/// <summary>
	/// SEを再生する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	void Play(SEType type);

	/// <summary>
	/// SEを最初から再生する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	void PlayFromStart(SEType type);

	/// <summary>
	/// マスターボリュームを設定する。
	/// </summary>
	/// <param name="volume"> ボリューム </param>
	void SetMasterVolume(float volume);
	/// <summary>
	/// SEの種類ごとのボリュームを設定する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	/// <param name="volume"> ボリューム </param>
	void SetVolume(SEType type, float volume);

	/// <summary>
	/// SEを停止する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	void Stop(SEType type);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:

	/// <summary>
	/// SEの情報
	/// </summary>
	struct SEInfo {
		const char* filePath = nullptr;
		float volume = 1.0f;
	};

	static constexpr size_t kSECount = static_cast<size_t>(SEType::Count); // SEの種類の総数
	static const std::array<SEInfo, kSECount> kSEInfos;                    // SEの情報の配列

	/// <summary>
	/// SEをロードする。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	/// <returns></returns>
	SoundData& Load(SEType type); 

	/// <summary>
	/// ボリュームを適用する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	void ApplyVolume(SEType type);

	/// <summary>
	/// SETypeをインデックスに変換する。
	/// </summary>
	/// <param name="type"> SEの種類 </param>
	/// <returns></returns>
	static size_t ToIndex(SEType type);

	std::array<SoundData, kSECount> seData_{}; // SEのデータの配列
	std::array<bool, kSECount> isLoaded_{};    // SEがロードされているかどうかのフラグの配列
	std::array<float, kSECount> volumes_{};    // SEの種類ごとのボリュームの配列
	float masterVolume_ = 1.0f;                // SEのマスターボリューム
};
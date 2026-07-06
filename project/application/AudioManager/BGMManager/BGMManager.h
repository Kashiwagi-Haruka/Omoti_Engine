#pragma once
#include "Audio.h"
#include <array>
#include <cstddef>
#include <optional>

/// <summary>
/// BGMの処理の管理クラス
/// </summary>
class BGMManager {
public:
	/// BGMの種類。
	enum class BGMType {
		Title, // タイトル画面
		Tutorial, // チュートリアル
		Game,     // ゲームプレイ中
		GameOver, // ゲームオーバー
		Result,   // リザルト画面
		Count,    // BGMの種類の総数
	};

	/// <summary>
	/// インスタンスを取得する。
	/// </summary>
	/// <returns></returns>
	static BGMManager* GetInstance();

	/// <summary>
	/// BGMを再生する。
	/// </summary>
	/// <param name="type"> BGMの種類 </param>
	/// <param name="restart"> 再生をリスタートするかどうか </param>
	void Play(BGMType type, bool restart = false);

	/// <summary>
	/// BGMを停止する。
	/// </summary>
	void Stop();
	/// <summary>
	/// BGMのマスターボリュームを設定する。
	/// </summary>
	/// <param name="volume"> ボリューム </param>
	void SetMasterVolume(float volume);
	/// <summary>
	/// BGMの種類ごとのボリュームを設定する。
	/// </summary>
	/// <param name="type"> BGMの種類 </param>
	/// <param name="volume"> ボリューム </param>
	void SetVolume(BGMType type, float volume);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

private:
	/// <summary>
	/// BGMの情報
	/// </summary>
	struct BGMInfo {
		const char* filePath = nullptr;
		float volume = 1.0f;
	};

	static constexpr size_t kBGMCount = static_cast<size_t>(BGMType::Count); // BGMの種類の総数
	static const std::array<BGMInfo, kBGMCount> kBGMInfos;                   // BGMの情報の配列

	/// <summary>
	/// BGMをロードする。
	/// </summary>
	/// <param name="type"> BGMの種類 </param>
	/// <returns></returns>
	SoundData& Load(BGMType type);
	
	/// <summary>
	/// BGMのボリュームを適用する。
	/// </summary>
	/// <param name="type"> BGMの種類 </param>
	void ApplyVolume(BGMType type);

	/// <summary>
	/// BGMTypeをインデックスに変換する。
	/// </summary>
	/// <param name="type"> BGMの種類 </param>
	/// <returns></returns>
	static size_t ToIndex(BGMType type);

	std::array<SoundData, kBGMCount> bgmData_{}; // BGMのデータの配列
	std::array<bool, kBGMCount> isLoaded_{};     // BGMがロードされているかどうかのフラグの配列
	std::array<float, kBGMCount> volumes_{};     // BGMの種類ごとのボリュームの配列
	float masterVolume_ = 1.0f;                  // BGMのマスターボリューム
	std::optional<BGMType> currentBGM_;          // 現在再生中のBGMの種類
};
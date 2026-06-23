#pragma once

#include <filesystem>
#include <string>
#include <vector>

class EditorAsset {

public:
	enum class AssetCategory { None, Audio, Camera, Sprite, Primitive, Object3d, Material };

	void AudioRegister();
	void CameraRegister();
	void SpriteRegister();
	void PrimitiveRegister();
	void Object3dRegister();
	void MaterialRegister();

	void EditorDraw();

private:
	struct AssetEntry {
		AssetCategory category = AssetCategory::None;
		std::string name;
		std::filesystem::path path;
	};

	static const char* CategoryToLabel(AssetCategory category);
	static std::filesystem::path GetSceneAssetRootPath();
	static std::filesystem::path GetCategoryPath(AssetCategory category);
	static void EnsureSceneAssetFolders();
	static void WriteAssetFile(const std::filesystem::path& path, AssetCategory category, const std::string& name);
	static std::vector<AssetEntry> CollectEntries(AssetCategory category);

	void DrawBreadcrumb();
	void DrawSceneFileContents();
	void DrawCategoryContents(AssetCategory category);
	void DrawAddPopup();
	void DrawAssetTile(const AssetEntry& entry);
	void CreatePrimitiveAsset(const std::string& primitiveName);
	void CreateObject3dAsset(const std::string& modelName);
	void CreateAudioAsset(const std::filesystem::path& audioPath);

	AssetCategory selectedCategory_ = AssetCategory::None;
	bool sceneFileOpened_ = true;
	char object3dModelName_[128] = "debugBox";
	char audioFilePath_[260] = "Resources/Audio/";
};
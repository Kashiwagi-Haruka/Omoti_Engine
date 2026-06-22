#include "EditorAsset.h"

#include "Engine/BaseScene/SceneManager.h"
#include "Engine/Texture/TextureManager.h"
#include <algorithm>
#include <fstream>
#include <imgui.h>

namespace {
constexpr const char* kDragDropPayloadType = "OMOTI_EDITOR_ASSET";
constexpr const char* kAssetExtension = ".asset";

std::string SanitizeFileName(std::string name) {
	for (char& c : name) {
		if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
			c = '_';
		}
	}
	return name;
}
} // namespace

const char* EditorAsset::CategoryToLabel(AssetCategory category) {
	switch (category) {
	case AssetCategory::Audio:
		return "Audio";
	case AssetCategory::Camera:
		return "Camera";
	case AssetCategory::Sprite:
		return "Sprite";
	case AssetCategory::Primitive:
		return "Primitive";
	case AssetCategory::Object3d:
		return "Object3d";
	case AssetCategory::Material:
		return "Material";
	case AssetCategory::None:
	default:
		return "";
	}
}

std::filesystem::path EditorAsset::GetSceneAssetRootPath() {
	const SceneManager* sceneManager = SceneManager::GetInstance();
	const std::string sceneName = (sceneManager && !sceneManager->GetCurrentSceneName().empty()) ? sceneManager->GetCurrentSceneName() : "Scene";
	return std::filesystem::path("Resources") / "Editor" / "Assets" / (sceneName + "_SceneFile");
}

std::filesystem::path EditorAsset::GetCategoryPath(AssetCategory category) { return GetSceneAssetRootPath() / CategoryToLabel(category); }

void EditorAsset::EnsureSceneAssetFolders() {
	std::error_code errorCode;
	std::filesystem::create_directories(GetSceneAssetRootPath(), errorCode);
	for (AssetCategory category : {AssetCategory::Audio, AssetCategory::Object3d, AssetCategory::Primitive}) {
		std::filesystem::create_directories(GetCategoryPath(category), errorCode);
	}
}

void EditorAsset::WriteAssetFile(const std::filesystem::path& path, AssetCategory category, const std::string& name) {
	std::ofstream file(path);
	if (!file.is_open()) {
		return;
	}
	file << "category=" << CategoryToLabel(category) << '\n';
	file << "name=" << name << '\n';
}

std::vector<EditorAsset::AssetEntry> EditorAsset::CollectEntries(AssetCategory category) {
	EnsureSceneAssetFolders();
	std::vector<AssetEntry> entries;
	const std::filesystem::path categoryPath = GetCategoryPath(category);
	std::error_code errorCode;
	for (const auto& entry : std::filesystem::directory_iterator(categoryPath, errorCode)) {
		if (errorCode || !entry.is_regular_file()) {
			continue;
		}
		std::string assetName = entry.path().stem().string();
		std::ifstream file(entry.path());
		std::string line;
		while (std::getline(file, line)) {
			constexpr const char* kNamePrefix = "name=";
			if (line.rfind(kNamePrefix, 0) == 0) {
				assetName = line.substr(std::char_traits<char>::length(kNamePrefix));
				break;
			}
		}
		entries.push_back({category, assetName, entry.path()});
	}
	std::sort(entries.begin(), entries.end(), [](const AssetEntry& a, const AssetEntry& b) { return a.name < b.name; });
	return entries;
}

void EditorAsset::CreatePrimitiveAsset(const std::string& primitiveName) {
	EnsureSceneAssetFolders();
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Primitive) / (SanitizeFileName(primitiveName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Primitive, primitiveName);
}

void EditorAsset::CreateObject3dAsset(const std::string& modelName) {
	EnsureSceneAssetFolders();
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Object3d) / (SanitizeFileName(modelName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Object3d, modelName);
}

void EditorAsset::CreateAudioAsset(const std::filesystem::path& audioPath) {
	EnsureSceneAssetFolders();
	const std::string assetName = audioPath.stem().string().empty() ? audioPath.filename().string() : audioPath.stem().string();
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Audio) / (SanitizeFileName(assetName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Audio, audioPath.string());
}

void EditorAsset::DrawBreadcrumb() {
	ImGui::TextUnformatted("Asset >");
	ImGui::SameLine(0.0f, 8.0f);
	if (ImGui::Selectable(GetSceneAssetRootPath().filename().string().c_str(), sceneFileOpened_, 0, ImGui::CalcTextSize(GetSceneAssetRootPath().filename().string().c_str()))) {
		sceneFileOpened_ = true;
		selectedCategory_ = AssetCategory::None;
	}
	if (selectedCategory_ != AssetCategory::None) {
		ImGui::SameLine(0.0f, 8.0f);
		ImGui::Text("> %s", CategoryToLabel(selectedCategory_));
	}
}

void EditorAsset::DrawAssetTile(const AssetEntry& entry) {
	constexpr const char* kFileIconPath = "Resources/Editor/File.png";
	const ImVec2 buttonSize(96.0f, 112.0f);
	const ImVec2 iconSize(48.0f, 48.0f);
	const float iconTopOffset = 16.0f;
	const float textBottomOffset = 14.0f;
	const auto fileIconHandle = TextureManager::GetInstance()->GetSrvHandleGPU(kFileIconPath);
	const ImVec2 buttonPos = ImGui::GetCursorScreenPos();
	ImGui::Button(("##asset_" + entry.path.string()).c_str(), buttonSize);
	ImDrawList* drawList = ImGui::GetWindowDrawList();
	const ImVec2 iconPos(buttonPos.x + (buttonSize.x - iconSize.x) * 0.5f, buttonPos.y + iconTopOffset);
	drawList->AddImage(ImTextureRef(reinterpret_cast<void*>(fileIconHandle.ptr)), iconPos, ImVec2(iconPos.x + iconSize.x, iconPos.y + iconSize.y));
	const ImVec2 labelSize = ImGui::CalcTextSize(entry.name.c_str());
	const ImVec2 labelPos(buttonPos.x + (buttonSize.x - labelSize.x) * 0.5f, buttonPos.y + buttonSize.y - textBottomOffset - labelSize.y);
	drawList->AddText(labelPos, IM_COL32(255, 255, 255, 255), entry.name.c_str());

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
		const std::string payload = std::string(CategoryToLabel(entry.category)) + "|" + entry.name;
		ImGui::SetDragDropPayload(kDragDropPayloadType, payload.c_str(), payload.size() + 1);
		ImGui::Text("Add %s/%s to Hierarchy", CategoryToLabel(entry.category), entry.name.c_str());
		ImGui::EndDragDropSource();
	}
}

void EditorAsset::DrawSceneFileContents() {
	struct FolderDef {
		AssetCategory category;
		const char* label;
	};
	constexpr FolderDef folders[] = {
	    {AssetCategory::Audio,     "Audio"    },
	    {AssetCategory::Object3d,  "Object3d" },
	    {AssetCategory::Primitive, "Primitive"},
	};
	for (const FolderDef& folder : folders) {
		if (ImGui::Button(folder.label, ImVec2(120.0f, 40.0f))) {
			selectedCategory_ = folder.category;
		}
		ImGui::SameLine();
	}
	ImGui::NewLine();
}

void EditorAsset::DrawCategoryContents(AssetCategory category) {
	constexpr float buttonSpacing = 12.0f;
	const float windowContentMaxX = ImGui::GetWindowPos().x + ImGui::GetWindowContentRegionMax().x;
	for (const AssetEntry& entry : CollectEntries(category)) {
		const ImVec2 buttonPos = ImGui::GetCursorScreenPos();
		DrawAssetTile(entry);
		const float nextButtonRightEdge = buttonPos.x + 96.0f + buttonSpacing + 96.0f;
		if (nextButtonRightEdge <= windowContentMaxX) {
			ImGui::SameLine(0.0f, buttonSpacing);
		}
	}
}

void EditorAsset::DrawAddPopup() {
	if (!sceneFileOpened_) {
		return;
	}
	if (ImGui::BeginPopupContextWindow("SceneFileAddPopup", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
		if (ImGui::BeginMenu("Add")) {
			if (ImGui::BeginMenu("Primitive")) {
				for (const char* primitiveName : {"Plane", "Sphere", "Cylinder", "Cone", "Box"}) {
					if (ImGui::MenuItem(primitiveName)) {
						CreatePrimitiveAsset(primitiveName);
						selectedCategory_ = AssetCategory::Primitive;
					}
				}
				ImGui::EndMenu();
			}
			ImGui::InputText("Object3d Model", object3dModelName_, sizeof(object3dModelName_));
			if (ImGui::MenuItem("Add Object3d")) {
				CreateObject3dAsset(object3dModelName_);
				selectedCategory_ = AssetCategory::Object3d;
			}
			ImGui::InputText("Audio Path", audioFilePath_, sizeof(audioFilePath_));
			if (ImGui::MenuItem("Add Audio")) {
				CreateAudioAsset(audioFilePath_);
				selectedCategory_ = AssetCategory::Audio;
			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
}

void EditorAsset::EditorDraw() {
#ifdef USE_IMGUI
	EnsureSceneAssetFolders();

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const float kTopToolbarHeight = 45.0f;
	const float kLeftPanelRatio = 0.22f;
	const float kRightPanelRatio = 0.24f;
	const float kPanelMinWidth = 260.0f;
	const float kGameAspect = 16.0f / 9.0f;
	const float leftPanelWidth = (std::max)(kPanelMinWidth, viewport->WorkSize.x * kLeftPanelRatio);
	const float rightPanelWidth = (std::max)(kPanelMinWidth, viewport->WorkSize.x * kRightPanelRatio);
	const float availableWidth = (std::max)(1.0f, viewport->WorkSize.x - leftPanelWidth - rightPanelWidth);
	const float availableHeight = (std::max)(1.0f, viewport->WorkSize.y - kTopToolbarHeight);

	float gameHeight = availableWidth / kGameAspect;
	if (gameHeight > availableHeight) {
		gameHeight = availableHeight;
	}

	const float assetWindowPosX = viewport->WorkPos.x + leftPanelWidth;
	const float assetWindowPosY = viewport->WorkPos.y + kTopToolbarHeight + gameHeight;
	const float assetWindowWidth = availableWidth;
	const float assetWindowHeight = (std::max)(1.0f, availableHeight - gameHeight);

	ImGui::SetNextWindowPos(ImVec2(assetWindowPosX, assetWindowPosY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(assetWindowWidth, assetWindowHeight), ImGuiCond_Always);
	if (ImGui::Begin("Asset", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar)) {
		DrawBreadcrumb();
		ImGui::Separator();
		DrawAddPopup();
		if (selectedCategory_ == AssetCategory::None) {
			DrawSceneFileContents();
		} else {
			DrawCategoryContents(selectedCategory_);
		}
	}
	ImGui::End();
#endif // USE_IMGUI
}
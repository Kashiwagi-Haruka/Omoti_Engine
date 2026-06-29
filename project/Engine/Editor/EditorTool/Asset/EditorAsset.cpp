#define NOMINMAX
#include "EditorAsset.h"
#include "Engine/BaseScene/SceneManager.h"
#include "Engine/Texture/TextureManager.h"
#include <algorithm>
#include <fstream>
#include <imgui.h>
#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#endif

namespace {
constexpr const char* kDragDropPayloadType = "OMOTI_EDITOR_ASSET";
constexpr const char* kAssetExtension = ".asset";
constexpr const char* kDefaultObjectModelName = "debugBox";

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

void EditorAsset::WriteAssetFile(const std::filesystem::path& path, AssetCategory category, const std::string& name, const std::string& source) {
	std::ofstream file(path);
	if (!file.is_open()) {
		return;
	}
	file << "category=" << CategoryToLabel(category) << '\n';
	file << "name=" << name << '\n';
	if (!source.empty()) {
		file << "source=" << source << '\n';
	}
}

std::string EditorAsset::MakeUniqueAssetName(AssetCategory category, const std::string& baseName) {
	const std::string fallback = baseName.empty() ? "Asset" : baseName;
	std::string uniqueName = fallback;
	int suffix = 1;
	while (std::filesystem::exists(GetCategoryPath(category) / (SanitizeFileName(uniqueName) + kAssetExtension))) {
		uniqueName = fallback + std::to_string(suffix++);
	}
	return uniqueName;
}

std::string EditorAsset::ToResourceRelativeObjPath(const std::filesystem::path& objPath) {
	if (objPath.empty()) {
		return {};
	}
	std::filesystem::path normalized = objPath.lexically_normal();
	std::filesystem::path resources = std::filesystem::absolute("Resources").lexically_normal();
	std::filesystem::path absolute = std::filesystem::absolute(normalized).lexically_normal();
	std::error_code errorCode;
	std::filesystem::path relative = std::filesystem::relative(absolute, resources, errorCode);
	if (!errorCode && !relative.empty() && *relative.begin() != "..") {
		return relative.generic_string();
	}
	return normalized.generic_string();
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
		std::string source;
		std::ifstream file(entry.path());
		std::string line;
		while (std::getline(file, line)) {
			constexpr const char* kNamePrefix = "name=";
			constexpr const char* kSourcePrefix = "source=";
			if (line.rfind(kNamePrefix, 0) == 0) {
				assetName = line.substr(std::char_traits<char>::length(kNamePrefix));
			} else if (line.rfind(kSourcePrefix, 0) == 0) {
				source = line.substr(std::char_traits<char>::length(kSourcePrefix));
			}
		}
		entries.push_back({category, assetName, source.empty() ? assetName : source, entry.path()});
	}
	std::sort(entries.begin(), entries.end(), [](const AssetEntry& a, const AssetEntry& b) { return a.name < b.name; });
	return entries;
}

void EditorAsset::CreatePrimitiveAsset(const std::string& primitiveName) {
	EnsureSceneAssetFolders();
	const std::string assetName = MakeUniqueAssetName(AssetCategory::Primitive, primitiveName);
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Primitive) / (SanitizeFileName(assetName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Primitive, assetName, primitiveName);
}

void EditorAsset::CreateObject3dAsset(const std::string& modelName) {
	EnsureSceneAssetFolders();
	const std::string modelPath = modelName.empty() ? kDefaultObjectModelName : modelName;
	const std::string baseName = std::filesystem::path(modelPath).stem().string().empty() ? modelPath : std::filesystem::path(modelPath).stem().string();
	const std::string assetName = MakeUniqueAssetName(AssetCategory::Object3d, baseName);
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Object3d) / (SanitizeFileName(assetName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Object3d, assetName, modelPath);
}

void EditorAsset::CreateAudioAsset(const std::filesystem::path& audioPath) {
	EnsureSceneAssetFolders();
	const std::string assetName = audioPath.stem().string().empty() ? audioPath.filename().string() : audioPath.stem().string();
	const std::string uniqueName = MakeUniqueAssetName(AssetCategory::Audio, assetName);
	const std::filesystem::path path = GetCategoryPath(AssetCategory::Audio) / (SanitizeFileName(uniqueName) + kAssetExtension);
	WriteAssetFile(path, AssetCategory::Audio, uniqueName, audioPath.string());
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
		const std::string payload = std::string(CategoryToLabel(entry.category)) + "|" + (entry.source.empty() ? entry.name : entry.source);
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
			ImGui::Text("Object3d Model: %s", object3dModelName_[0] == '\0' ? "debugBox (default)" : object3dModelName_);
			if (ImGui::Button("Select Object3d Obj")) {
#ifdef _WIN32
				char fileName[MAX_PATH] = {};
				OPENFILENAMEA openFileName = {};
				openFileName.lStructSize = sizeof(openFileName);
				openFileName.hwndOwner = nullptr;
				openFileName.lpstrFilter = "Wavefront OBJ (*.obj)\0*.obj\0All Files (*.*)\0*.*\0";
				openFileName.lpstrFile = fileName;
				openFileName.nMaxFile = MAX_PATH;
				const std::filesystem::path initialDir = std::filesystem::absolute("Resources");
				const std::string initialDirString = initialDir.string();
				openFileName.lpstrInitialDir = initialDirString.c_str();
				openFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetOpenFileNameA(&openFileName)) {
					const std::string selectedPath = ToResourceRelativeObjPath(fileName);
					std::copy_n(selectedPath.c_str(), std::min(selectedPath.size(), sizeof(object3dModelName_) - 1), object3dModelName_);
					object3dModelName_[std::min(selectedPath.size(), sizeof(object3dModelName_) - 1)] = '\0';
				}
#else
				ImGui::OpenPopup("Object3d Obj Select Unsupported");
#endif
			}
#ifndef _WIN32
			if (ImGui::BeginPopup("Object3d Obj Select Unsupported")) {
				ImGui::TextUnformatted("OBJ file dialog is only available on Windows builds.");
				ImGui::EndPopup();
			}
#endif
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
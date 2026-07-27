#define NOMINMAX
#include "Hierarchy.h"
#include "Camera.h"
#include "Engine/BaseScene/SceneManager.h"
#include "Engine/Editor/EditorData/Object3d/EditorObject3d.h"
#include "Engine/Editor/EditorData/Primitive/EditorPrimitive.h"
#include "Engine/Editor/EditorTool/Command/EditorCommand.h"
#include "Engine/Editor/EditorTool/Grid/EditorGrid.h"
#include "Engine/Editor/EditorTool/ToolBar/ToolBar.h"
#include "Engine/Loadfile/JSON/JsonManager.h"
#include "Function.h"
#include "Input.h"
#include "Model/ModelManager.h"
#include "Object3d/Object3d.h"
#include "Object3d/Object3dCommon.h"
#include "Primitive/Primitive.h"
#include "Sprite/SpriteCommon.h"
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_internal.h"
#include "externals/ImGuizmo/ImGuizmo.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace {
constexpr const char* kDragDropPayloadType = "OMOTI_EDITOR_ASSET";

using json = nlohmann::json;

Parameter CreateDefaultCharacterParameter() {
	Parameter parameter{};
	parameter.level = 1;
	parameter.HP = 100.0f;
	parameter.Attack = 20.0f;
	parameter.Deffence = 10.0f;
	parameter.Speed = 1.0f;
	parameter.CriticalRate = 0.05f;
	parameter.CriticalDamage = 1.5f;
	parameter.AttributeAffinity = 1.0f;
	return parameter;
}

json ToJson(const Parameter& p) {
	json param;
	param["Level"] = p.level;
	param["HP"] = p.HP;
	param["Attack"] = p.Attack;
	param["Deffence"] = p.Deffence;
	param["Speed"] = p.Speed;
	param["CriticalRate"] = p.CriticalRate;
	param["CriticalDamage"] = p.CriticalDamage;
	param["AttributeAffinity"] = p.AttributeAffinity;
	for (size_t i = 0; i < p.AttributeDamageRate.size(); ++i) {
		param["AttributeDamageRate"].push_back(p.AttributeDamageRate[i]);
		param["AttributeResistanceRate"].push_back(p.AttributeResistanceRate[i]);
	}
	return param;
}

void SaveJson(const std::string& path, const json& j) {
	std::ofstream ofs(path);
	if (ofs.is_open()) {
		ofs << j.dump(2);
	}
}

int LoadReinforcementAmount(const std::string& name) {
	std::ifstream ifs("Resources/JSON/Character/" + name + "/current_parameters.json");
	json root;
	if (!ifs.is_open() || !(ifs >> root)) {
		return 0;
	}
	return std::max(0, root.value("reinforcementAmount", 0));
}

void SaveCharacterTuningJson(
    const std::string& name, const BaseParameter& lv1Base, const Parameter& lv1Parameter, const BaseParameter& levelUpBase, const BaseParameter& currentBase, const Parameter& currentParameter,
    int reinforcementAmount) {
	const std::string directory = "Resources/JSON/Character/" + name;
	std::filesystem::create_directories(directory);
	reinforcementAmount = std::clamp(reinforcementAmount, 0, 999);

	SaveJson(
	    directory + "/lv1_parameters.json", json{
	                                            {"base",                {{"HP", lv1Base.HP}, {"Attack", lv1Base.Attack}, {"Deffence", lv1Base.Deffence}}            },
	                                            {"baseLevelUp",         {{"HP", levelUpBase.HP}, {"Attack", levelUpBase.Attack}, {"Deffence", levelUpBase.Deffence}}},
	                                            {"parameter",           ToJson(lv1Parameter)                                                                        },
	                                            {"reinforcementAmount", reinforcementAmount                                                                         }
    });
	SaveJson(
	    directory + "/current_parameters.json", json{
	                                                {"base",                {{"HP", currentBase.HP}, {"Attack", currentBase.Attack}, {"Deffence", currentBase.Deffence}}},
	                                                {"baseLevelUp",         {{"HP", levelUpBase.HP}, {"Attack", levelUpBase.Attack}, {"Deffence", levelUpBase.Deffence}}},
	                                                {"parameter",           ToJson(currentParameter)                                                                    },
	                                                {"reinforcementAmount", reinforcementAmount                                                                         }
    });
}
#ifdef USE_IMGUI
void DrawBaseParameterEditor(const char* label, BaseParameter& parameter) {
	ImGui::SeparatorText(label);
	ImGui::DragFloat("HP", &parameter.HP, 1.0f, 1.0f, 99999.0f);
	ImGui::DragFloat("Attack", &parameter.Attack, 0.1f, 0.0f, 9999.0f);
	ImGui::DragFloat("Deffence", &parameter.Deffence, 0.1f, 0.0f, 9999.0f);
}

void DrawBaseParameterViewer(const char* label, const BaseParameter& parameter) {
	ImGui::SeparatorText(label);
	ImGui::Text("HP: %.2f", parameter.HP);
	ImGui::Text("Attack: %.2f", parameter.Attack);
	ImGui::Text("Deffence: %.2f", parameter.Deffence);
}

void DrawParameterEditor(const char* label, Parameter& parameter, bool levelOnlyEditable = false) {
	ImGui::PushID(label);
	ImGui::SeparatorText(label);
	ImGui::DragInt("Level", &parameter.level, 1.0f, 1, 100);
	if (levelOnlyEditable) {
		ImGui::Text("HP: %.2f", parameter.HP);
		ImGui::Text("Attack: %.2f", parameter.Attack);
		ImGui::Text("Deffence: %.2f", parameter.Deffence);
		ImGui::Text("Speed: %.2f", parameter.Speed);
		ImGui::Text("CriticalRate: %.3f", parameter.CriticalRate);
		ImGui::Text("CriticalDamage: %.2f", parameter.CriticalDamage);
		ImGui::Text("AttributeAffinity: %.2f", parameter.AttributeAffinity);
		ImGui::PopID();
		return;
	}
	ImGui::DragFloat("HP", &parameter.HP, 1.0f, 1.0f, 99999.0f);
	ImGui::DragFloat("Attack", &parameter.Attack, 0.1f, 0.0f, 9999.0f);
	ImGui::DragFloat("Deffence", &parameter.Deffence, 0.1f, 0.0f, 9999.0f);
	ImGui::DragFloat("Speed", &parameter.Speed, 0.01f, 0.0f, 100.0f);
	ImGui::DragFloat("CriticalRate", &parameter.CriticalRate, 0.001f, 0.0f, 1.0f);
	ImGui::DragFloat("CriticalDamage", &parameter.CriticalDamage, 0.01f, 1.0f, 10.0f);
	ImGui::DragFloat("AttributeAffinity", &parameter.AttributeAffinity, 0.1f, 0.0f, 999.0f);
	ImGui::PopID();
}
#endif

BaseParameter CalculateCurrentBaseParameter(const BaseParameter& lv1Base, const BaseParameter& levelUpBase, int level) {
	const float levelOffset = static_cast<float>(std::max(level, 1) - 1);
	return BaseParameter{lv1Base.HP + levelUpBase.HP * levelOffset, lv1Base.Attack + levelUpBase.Attack * levelOffset, lv1Base.Deffence + levelUpBase.Deffence * levelOffset};
}

Parameter CalculateCurrentParameter(const Parameter& lv1Parameter, const BaseParameter& levelUpBase, int level) {
	Parameter currentParameter = lv1Parameter;
	currentParameter.level = std::max(level, 1);
	const float levelOffset = static_cast<float>(currentParameter.level - 1);
	currentParameter.HP = lv1Parameter.HP + levelUpBase.HP * levelOffset;
	currentParameter.Attack = lv1Parameter.Attack + levelUpBase.Attack * levelOffset;
	currentParameter.Deffence = lv1Parameter.Deffence + levelUpBase.Deffence * levelOffset;
	return currentParameter;
}
constexpr const char* kDefaultObjectModelName = "debugBox";
constexpr float kDegreesToRadians = 3.14159265358979323846f / 180.0f;
std::filesystem::path ResolveObjectEditorJsonPath(const std::string& filePath) { return std::filesystem::path("Resources") / "JSON" / std::filesystem::path(filePath).filename(); }

bool HasObjectEditorJsonFile(const std::string& filePath) { return std::filesystem::exists(ResolveObjectEditorJsonPath(filePath)); }

bool IsNearlyEqual(float lhs, float rhs) { return std::fabs(lhs - rhs) <= 0.0001f; }

Primitive::PrimitiveName PrimitiveTypeFromName(const std::string& primitiveName) {
	if (primitiveName == "Plane") {
		return Primitive::Plane;
	}
	if (primitiveName == "Sphere") {
		return Primitive::Sphere;
	}
	if (primitiveName == "Cylinder") {
		return Primitive::Cylinder;
	}
	if (primitiveName == "Cone") {
		return Primitive::Cone;
	}
	return Primitive::Box;
}

std::string PrimitiveTypeToName(Primitive::PrimitiveName primitiveName) {
	switch (primitiveName) {
	case Primitive::Plane:
		return "Plane";
	case Primitive::Sphere:
		return "Sphere";
	case Primitive::Cylinder:
		return "Cylinder";
	case Primitive::Cone:
		return "Cone";
	case Primitive::Box:
	default:
		return "Box";
	}
}
void LoadEditorObjectModel(const std::string& modelName) {
	const std::filesystem::path modelPath(modelName);
	const std::string directoryPath = (modelPath.has_extension() || modelPath.has_parent_path()) ? "Resources" : "Resources/3d";
	ModelManager::GetInstance()->LoadModel(directoryPath, modelName);
}
Object3d* CreateEditorOwnedObject(std::vector<std::unique_ptr<Object3d>>& ownedObjects, const std::string& modelName, const std::string& editorId) {
	LoadEditorObjectModel(modelName);
	std::unique_ptr<Object3d> object = std::make_unique<Object3d>();
	object->Initialize();
	object->SetModel(modelName);
	object->SetCamera(Object3dCommon::GetInstance()->GetDefaultCamera());
	object->SetEditorId(editorId);
	Object3d* rawObject = object.get();
	ownedObjects.push_back(std::move(object));
	return rawObject;
}

Primitive* CreateEditorOwnedPrimitive(std::vector<std::unique_ptr<Primitive>>& ownedPrimitives, Primitive::PrimitiveName type, const std::string& editorId) {
	std::unique_ptr<Primitive> primitive = std::make_unique<Primitive>();
	primitive->Initialize(type);
	primitive->SetCamera(Object3dCommon::GetInstance()->GetDefaultCamera());
	primitive->SetEditorId(editorId);
	Primitive* rawPrimitive = primitive.get();
	ownedPrimitives.push_back(std::move(primitive));
	return rawPrimitive;
}

bool IsTransformNearlyEqual(const Transform& lhs, const Transform& rhs) {
	return IsNearlyEqual(lhs.scale.x, rhs.scale.x) && IsNearlyEqual(lhs.scale.y, rhs.scale.y) && IsNearlyEqual(lhs.scale.z, rhs.scale.z) && IsNearlyEqual(lhs.rotate.x, rhs.rotate.x) &&
	       IsNearlyEqual(lhs.rotate.y, rhs.rotate.y) && IsNearlyEqual(lhs.rotate.z, rhs.rotate.z) && IsNearlyEqual(lhs.translate.x, rhs.translate.x) &&
	       IsNearlyEqual(lhs.translate.y, rhs.translate.y) && IsNearlyEqual(lhs.translate.z, rhs.translate.z);
}
void EnsureObjectEditorDataSize(
    size_t size, std::vector<std::string>& objectNames, std::vector<std::string>& objectModelNames, std::vector<Transform>& editorTransforms, std::vector<InspectorMaterial>& editorMaterials) {
	while (objectNames.size() < size) {
		objectNames.push_back("Object " + std::to_string(objectNames.size()));
	}
	objectModelNames.resize(size);
	editorTransforms.resize(size);
	editorMaterials.resize(size);
}

void EnsurePrimitiveEditorDataSize(size_t size, std::vector<std::string>& primitiveNames, std::vector<Transform>& primitiveEditorTransforms, std::vector<InspectorMaterial>& primitiveEditorMaterials) {
	while (primitiveNames.size() < size) {
		primitiveNames.push_back("Primitive " + std::to_string(primitiveNames.size()));
	}
	primitiveEditorTransforms.resize(size);
	primitiveEditorMaterials.resize(size);
}
std::string GetCurrentSceneName() {
	const SceneManager* sceneManager = SceneManager::GetInstance();
	return sceneManager ? sceneManager->GetCurrentSceneName() : std::string();
}
} // namespace

Hierarchy* Hierarchy::GetInstance() {
	static Hierarchy instance;
	return &instance;
}
void Hierarchy::Finalize() {
	editorAudio_.Finalize();
	objects_.clear();
	objectNames_.clear();
	editorTransforms_.clear();
	editorMaterials_.clear();
	objectModelNames_.clear();
	objectSceneNames_.clear();

	primitives_.clear();
	primitiveNames_.clear();
	primitiveSceneNames_.clear();
	primitiveEditorTransforms_.clear();
	primitiveEditorMaterials_.clear();

	selectionBoxPrimitive_.reset();
	editorGridPlane_.reset();
	editorOwnedObjects_.clear();
	editorOwnedPrimitives_.clear();

	selectedObjectIndex_ = 0;
	selectedIsPrimitive_ = false;
	selectedIsCharacterParameter_ = false;
	selectionBoxDirty_ = true;
	gridSettings_.dirty = true;
	loadedSceneName_.clear();
	editorCamera_.Reset();
	ResetForSceneChange();
}
void Hierarchy::OnSceneChangeRequested(const std::string& nextSceneName) {
	PushGuizmoUndoIfNeeded();
	objects_.clear();
	objectNames_.clear();
	editorTransforms_.clear();
	editorMaterials_.clear();
	objectModelNames_.clear();
	objectSceneNames_.clear();

	primitives_.clear();
	primitiveNames_.clear();
	primitiveSceneNames_.clear();
	primitiveEditorTransforms_.clear();
	primitiveEditorMaterials_.clear();

	selectionBoxPrimitive_.reset();
	selectedObjectIndex_ = 0;
	selectedIsPrimitive_ = false;
	selectedIsCharacterParameter_ = false;
	selectionBoxDirty_ = true;
	loadedSceneName_ = nextSceneName;
	ResetForSceneChange();
#ifdef USE_IMGUI
	ImGui::CloseCurrentPopup();
	ImGui::ClearActiveID();
#endif
}
bool Hierarchy::IsEditorPreviewActive() const {
#ifdef USE_IMGUI
	return !isPlaying_;
#else
	return false;
#endif
}

void Hierarchy::UpdateEditorPreview() {
#ifdef USE_IMGUI
	editorCamera_.UpdateEditorPreview(IsEditorPreviewActive(), objects_, primitives_);
#endif
}
std::string Hierarchy::GetSceneScopedEditorFilePath(const std::string& defaultFilePath) const {
	const SceneManager* sceneManager = SceneManager::GetInstance();
	if (!sceneManager) {
		return defaultFilePath;
	}
	const std::string& sceneName = sceneManager->GetCurrentSceneName();
	if (sceneName.empty()) {
		return defaultFilePath;
	}
	return sceneName + "_" + std::filesystem::path(defaultFilePath).filename().string();
}

void Hierarchy::ResetForSceneChange() {
	isPaused_ = false;
	const std::string currentSceneName = GetCurrentSceneName();
	editorAudio_.ResetForSceneChange();
	hasUnsavedChanges_ = false;
	saveStatusMessage_.clear();
	hasLoadedForCurrentScene_ = false;
	hasLoadedSnapshot_ = false;
	loadedSnapshotFilePath_.clear();
	editorLight_.Reset();
	undoStack_.clear();
	redoStack_.clear();
	guizmoWasUsing_ = false;
	editorCamera_.DeactivatePreview();
	for (const auto& object : editorOwnedObjects_) {
		UnregisterObject3d(object.get());
	}
	for (const auto& primitive : editorOwnedPrimitives_) {
		UnregisterPrimitive(primitive.get());
	}
	editorOwnedObjects_.clear();
	editorOwnedPrimitives_.clear();
	for (size_t i = 0; i < objects_.size(); ++i) {
		if (i >= objectSceneNames_.size() || objectSceneNames_[i] != currentSceneName) {
			objects_[i] = nullptr;
		}
	}
	for (size_t i = 0; i < primitives_.size(); ++i) {
		if (i >= primitiveSceneNames_.size() || primitiveSceneNames_[i] != currentSceneName) {
			primitives_[i] = nullptr;
		}
	}
}


Hierarchy::EditorSnapshot Hierarchy::CreateCurrentSnapshot() const {
	EditorSnapshot snapshot{};
	snapshot.objectTransforms = editorTransforms_;
	snapshot.objectMaterials = editorMaterials_;
	snapshot.objectNames = objectNames_;
	snapshot.objectModelNames = objectModelNames_;
	snapshot.primitiveTransforms = primitiveEditorTransforms_;
	snapshot.primitiveMaterials = primitiveEditorMaterials_;
	snapshot.primitiveNames = primitiveNames_;
	return snapshot;
}

bool Hierarchy::ResetToLoadedSnapshot() {
	if (!hasLoadedSnapshot_) {
		return false;
	}
	if (!loadedSnapshotFilePath_.empty() && HasObjectEditorJsonFile(loadedSnapshotFilePath_)) {
		if (!LoadObjectEditorsFromJson(loadedSnapshotFilePath_)) {
			return false;
		}
	} else {
		ApplyEditorSnapshot(loadedSnapshot_);
	}
	undoStack_.clear();
	redoStack_.clear();
	hasUnsavedChanges_ = false;
	return true;
}


void Hierarchy::UndoEditorChange() {
	if (undoStack_.empty()) {
		return;
	}
	EditorSnapshot current = CreateCurrentSnapshot();
	redoStack_.push_back(std::move(current));
	ApplyEditorSnapshot(undoStack_.back());
	undoStack_.pop_back();
	hasUnsavedChanges_ = true;
}

void Hierarchy::RedoEditorChange() {
	if (redoStack_.empty()) {
		return;
	}
	EditorSnapshot current = CreateCurrentSnapshot();
	undoStack_.push_back(std::move(current));
	ApplyEditorSnapshot(redoStack_.back());
	redoStack_.pop_back();
	hasUnsavedChanges_ = true;
}
void Hierarchy::PushGuizmoUndoIfNeeded() {
	if (!guizmoWasUsing_) {
		return;
	}

	const bool targetExists = guizmoTargetIsPrimitive_ ? guizmoTargetIndex_ < primitiveEditorTransforms_.size() : guizmoTargetIndex_ < editorTransforms_.size();
	if (targetExists) {
		const Transform& currentTransform = guizmoTargetIsPrimitive_ ? primitiveEditorTransforms_[guizmoTargetIndex_] : editorTransforms_[guizmoTargetIndex_];
		if (!IsTransformNearlyEqual(currentTransform, guizmoStartTransform_)) {
			undoStack_.push_back(std::move(guizmoBeforeEdit_));
			redoStack_.clear();
			hasUnsavedChanges_ = true;
		}
	}

	guizmoWasUsing_ = false;
}
void Hierarchy::ApplyEditorSnapshot(const EditorSnapshot& snapshot) {
	editorTransforms_ = snapshot.objectTransforms;
	editorMaterials_ = snapshot.objectMaterials;
	objectNames_ = snapshot.objectNames;
	objectModelNames_ = snapshot.objectModelNames;
	primitiveEditorTransforms_ = snapshot.primitiveTransforms;
	primitiveEditorMaterials_ = snapshot.primitiveMaterials;
	primitiveNames_ = snapshot.primitiveNames;

	for (size_t i = 0; i < objects_.size(); ++i) {
		if (!objects_[i] || i >= editorTransforms_.size() || i >= editorMaterials_.size()) {
			continue;
		}
		if (i < objectModelNames_.size() && objects_[i]->GetModelFilePath() != objectModelNames_[i]) {
			LoadEditorObjectModel(objectModelNames_[i]);
			objects_[i]->SetModel(objectModelNames_[i]);
		}
		EditorObject3d::ApplyEditorValues(objects_[i], editorTransforms_[i], editorMaterials_[i]);
	}

	for (size_t i = 0; i < primitives_.size(); ++i) {
		if (!primitives_[i] || primitives_[i] == selectionBoxPrimitive_.get() || i >= primitiveEditorTransforms_.size() || i >= primitiveEditorMaterials_.size()) {
			continue;
		}
		EditorPrimitive::ApplyEditorValues(primitives_[i], primitiveEditorTransforms_[i], primitiveEditorMaterials_[i]);
	}

	selectionBoxDirty_ = true;
}
void Hierarchy::AddPrimitiveAssetToHierarchy(const std::string& primitiveName) {
	Primitive* rawPrimitive = CreateEditorOwnedPrimitive(editorOwnedPrimitives_, PrimitiveTypeFromName(primitiveName), "");
	RegisterPrimitive(rawPrimitive);
	const size_t index = primitiveEditorTransforms_.empty() ? 0 : primitiveEditorTransforms_.size() - 1;
	if (index < primitiveNames_.size()) {
		primitiveNames_[index] = primitiveName;
	}
	selectedObjectIndex_ = index;
	selectedIsPrimitive_ = true;
	selectedIsCharacterParameter_ = false;
	selectionBoxDirty_ = true;
	hasUnsavedChanges_ = true;
}

void Hierarchy::AddObject3dAssetToHierarchy(const std::string& modelName) {
	Object3d* rawObject = CreateEditorOwnedObject(editorOwnedObjects_, modelName.empty() ? kDefaultObjectModelName : modelName, "");
	RegisterObject3d(rawObject);
	const size_t index = editorTransforms_.empty() ? 0 : editorTransforms_.size() - 1;
	if (index < objectNames_.size()) {
		objectNames_[index] = modelName;
	}
	if (index < objectModelNames_.size()) {
		objectModelNames_[index] = modelName.empty() ? kDefaultObjectModelName : modelName;
	}
	selectedObjectIndex_ = index;
	selectedIsPrimitive_ = false;
	selectedIsCharacterParameter_ = false;
	selectionBoxDirty_ = true;
	hasUnsavedChanges_ = true;
}

void Hierarchy::AddAudioAssetToHierarchy(const std::filesystem::path& audioPath) {
	editorAudio_.AddSoundForEditor(audioPath.string());
	saveStatusMessage_ = "Audio added: " + audioPath.string();
	hasUnsavedChanges_ = true;
}
void Hierarchy::DeleteObjectAtIndex(size_t index) {
	if (index >= objects_.size() || !objects_[index]) {
		return;
	}

	Object3d* object = objects_[index];
	UnregisterObject3d(object);
	editorOwnedObjects_.erase(
	    std::remove_if(editorOwnedObjects_.begin(), editorOwnedObjects_.end(), [object](const std::unique_ptr<Object3d>& ownedObject) { return ownedObject.get() == object; }),
	    editorOwnedObjects_.end());

	if (!selectedIsPrimitive_ && selectedObjectIndex_ == index) {
		selectedObjectIndex_ = 0;
	}
	selectionBoxDirty_ = true;
	redoStack_.clear();
	hasUnsavedChanges_ = true;
	saveStatusMessage_ = "Deleted object";
}

void Hierarchy::DeletePrimitiveAtIndex(size_t index) {
	if (index >= primitives_.size() || !primitives_[index] || primitives_[index] == selectionBoxPrimitive_.get()) {
		return;
	}

	Primitive* primitive = primitives_[index];
	UnregisterPrimitive(primitive);
	editorOwnedPrimitives_.erase(
	    std::remove_if(editorOwnedPrimitives_.begin(), editorOwnedPrimitives_.end(), [primitive](const std::unique_ptr<Primitive>& ownedPrimitive) { return ownedPrimitive.get() == primitive; }),
	    editorOwnedPrimitives_.end());

	if (selectedIsPrimitive_ && selectedObjectIndex_ == index) {
		selectedObjectIndex_ = 0;
	}
	selectionBoxDirty_ = true;
	redoStack_.clear();
	hasUnsavedChanges_ = true;
	saveStatusMessage_ = "Deleted primitive";
}
void Hierarchy::RegisterObject3d(Object3d* object) {
	if (!object) {
		return;
	}
	if (std::find(objects_.begin(), objects_.end(), object) != objects_.end()) {
		return;
	}
	auto emptyIt = std::find(objects_.begin(), objects_.end(), nullptr);
	if (emptyIt != objects_.end()) {
		const size_t index = static_cast<size_t>(std::distance(objects_.begin(), emptyIt));
		EnsureObjectEditorDataSize(index + 1, objectNames_, objectModelNames_, editorTransforms_, editorMaterials_);
		objectSceneNames_.resize(index + 1);
		objects_[index] = object;
		objectModelNames_[index] = object->GetModelFilePath();
		objectSceneNames_[index] = GetCurrentSceneName();
		EditorObject3d::ApplyEditorValues(object, editorTransforms_[index], editorMaterials_[index]);
		return;
	}
	const size_t index = objects_.size();
	objects_.push_back(object);
	objectNames_.push_back("Object " + std::to_string(index));
	objectModelNames_.push_back(object->GetModelFilePath());
	objectSceneNames_.push_back(GetCurrentSceneName());
	editorTransforms_.push_back(object->GetTransform());
	editorMaterials_.push_back(EditorObject3d::CaptureMaterial(object));
}

void Hierarchy::UnregisterObject3d(Object3d* object) {
	if (!object) {
		return;
	}
	for (size_t i = 0; i < objects_.size(); ++i) {
		if (objects_[i] == object) {
			objects_[i] = nullptr;
			if (i < objectSceneNames_.size()) {
				objectSceneNames_[i].clear();
			}
			if (!selectedIsPrimitive_ && selectedObjectIndex_ == i) {
				selectedObjectIndex_ = 0;
			}
			break;
		}
	}
}

void Hierarchy::RegisterPrimitive(Primitive* primitive) {
	if (!primitive) {
		return;
	}
	if (std::find(primitives_.begin(), primitives_.end(), primitive) != primitives_.end()) {
		return;
	}
	auto emptyIt = std::find(primitives_.begin(), primitives_.end(), nullptr);
	if (emptyIt != primitives_.end()) {
		const size_t index = static_cast<size_t>(std::distance(primitives_.begin(), emptyIt));
		EnsurePrimitiveEditorDataSize(index + 1, primitiveNames_, primitiveEditorTransforms_, primitiveEditorMaterials_);
		primitiveSceneNames_.resize(index + 1);
		primitives_[index] = primitive;
		primitiveSceneNames_[index] = GetCurrentSceneName();
		EditorPrimitive::ApplyEditorValues(primitive, primitiveEditorTransforms_[index], primitiveEditorMaterials_[index]);
		return;
	}

	const size_t index = primitives_.size();
	primitives_.push_back(primitive);
	primitiveNames_.push_back("Primitive " + std::to_string(index));
	primitiveSceneNames_.push_back(GetCurrentSceneName());
	primitiveEditorTransforms_.push_back(primitive->GetTransform());
	primitiveEditorMaterials_.push_back(EditorPrimitive::CaptureMaterial(primitive));
}

void Hierarchy::UnregisterPrimitive(Primitive* primitive) {
	if (!primitive) {
		return;
	}
	for (size_t i = 0; i < primitives_.size(); ++i) {
		if (primitives_[i] == primitive) {
			primitives_[i] = nullptr;
			if (i < primitiveSceneNames_.size()) {
				primitiveSceneNames_[i].clear();
			}
			if (selectedIsPrimitive_ && selectedObjectIndex_ == i) {
				selectedObjectIndex_ = 0;
			}
			break;
		}
	}
}
void Hierarchy::RegisterCamera(Camera* camera) {
#ifdef USE_IMGUI
	editorCamera_.RegisterCamera(camera);
#else
	(void)camera;
#endif
}

void Hierarchy::UnregisterCamera(Camera* camera) {
#ifdef USE_IMGUI
	editorCamera_.UnregisterCamera(camera);
#else
	(void)camera;
#endif
}
bool Hierarchy::HasRegisteredObjects() const { return !objects_.empty() || !primitives_.empty(); }

bool Hierarchy::LoadObjectEditorsFromJsonIfExists(const std::string& filePath) {
	const SceneManager* sceneManager = SceneManager::GetInstance();
	const std::string sceneName = sceneManager ? sceneManager->GetCurrentSceneName() : std::string();
	if (sceneName != loadedSceneName_) {
		ResetForSceneChange();
		loadedSceneName_ = sceneName;
	}

	const std::string scopedFilePath = GetSceneScopedEditorFilePath(filePath);
	if (!HasObjectEditorJsonFile(scopedFilePath)) {
		return false;
	}
	if (hasLoadedForCurrentScene_) {
		return true;
	}
	hasLoadedForCurrentScene_ = LoadObjectEditorsFromJson(scopedFilePath);
	return hasLoadedForCurrentScene_;
}

bool Hierarchy::SaveObjectEditorsToJson(const std::string& filePath) const {
	nlohmann::json root;
	root["objects"] = nlohmann::json::array();
	root["primitives"] = nlohmann::json::array();
	editorLight_.SaveToJson(root["lights"]);
	for (size_t i = 0; i < objects_.size(); ++i) {
		const Object3d* object = objects_[i];
		if (!object) {
			continue;
		}
		const Transform& transform = editorTransforms_[i];
		const InspectorMaterial& material = editorMaterials_[i];
		nlohmann::json objectJson;
		objectJson["index"] = i;
		objectJson["name"] = objectNames_[i];
		objectJson["editorId"] = object->GetEditorId();
		objectJson["modelName"] = i < objectModelNames_.size() ? objectModelNames_[i] : object->GetModelFilePath();
		objectJson["transform"] = {
		    {"scale",     {transform.scale.x, transform.scale.y, transform.scale.z}            },
		    {"rotate",    {transform.rotate.x, transform.rotate.y, transform.rotate.z}         },
		    {"translate", {transform.translate.x, transform.translate.y, transform.translate.z}},
		};
		objectJson["material"] = {
		    {"color",                  {material.color.x, material.color.y, material.color.z, material.color.w}},
		    {"enableLighting",         material.enableLighting                                                 },
		    {"shininess",              material.shininess		                                              },
		    {"environmentCoefficient", material.environmentCoefficient                                         },
		    {"grayscaleEnabled",       material.grayscaleEnabled                                               },
		    {"sepiaEnabled",           material.sepiaEnabled		                                           },
		    {"distortionStrength",     material.distortionStrength                                             },
		    {"distortionFalloff",      material.distortionFalloff                                              },
		    {"uvScale",                {material.uvScale.x, material.uvScale.y, material.uvScale.z}            },
		    {"uvRotate",               {material.uvRotate.x, material.uvRotate.y, material.uvRotate.z}         },
		    {"uvTranslate",            {material.uvTranslate.x, material.uvTranslate.y, material.uvTranslate.z}},
		    {"uvAnchor",               {material.uvAnchor.x, material.uvAnchor.y}                              },
		    {"texturePath",            material.texturePath		                                            },
		};
		root["objects"].push_back(objectJson);
	}

	for (size_t i = 0; i < primitives_.size(); ++i) {
		const Primitive* primitive = primitives_[i];
		if (!primitive || primitive == selectionBoxPrimitive_.get()) {
			continue;
		}
		const Transform& transform = primitiveEditorTransforms_[i];
		const InspectorMaterial& material = primitiveEditorMaterials_[i];
		nlohmann::json primitiveJson;
		primitiveJson["index"] = i;
		primitiveJson["name"] = primitiveNames_[i];
		primitiveJson["editorId"] = primitive->GetEditorId();
		primitiveJson["primitiveType"] = PrimitiveTypeToName(primitive->GetPrimitiveName());
		primitiveJson["transform"] = {
		    {"scale",     {transform.scale.x, transform.scale.y, transform.scale.z}            },
		    {"rotate",    {transform.rotate.x, transform.rotate.y, transform.rotate.z}         },
		    {"translate", {transform.translate.x, transform.translate.y, transform.translate.z}},
		};
		primitiveJson["material"] = {
		    {"color",                  {material.color.x, material.color.y, material.color.z, material.color.w}},
		    {"enableLighting",         material.enableLighting                                                 },
		    {"shininess",              material.shininess		                                              },
		    {"environmentCoefficient", material.environmentCoefficient                                         },
		    {"grayscaleEnabled",       material.grayscaleEnabled                                               },
		    {"sepiaEnabled",           material.sepiaEnabled		                                           },
		    {"distortionStrength",     material.distortionStrength                                             },
		    {"distortionFalloff",      material.distortionFalloff                                              },
		    {"uvScale",                {material.uvScale.x, material.uvScale.y, material.uvScale.z}            },
		    {"uvRotate",               {material.uvRotate.x, material.uvRotate.y, material.uvRotate.z}         },
		    {"uvTranslate",            {material.uvTranslate.x, material.uvTranslate.y, material.uvTranslate.z}},
		    {"uvAnchor",               {material.uvAnchor.x, material.uvAnchor.y}                              },
		    {"texturePath",            material.texturePath		                                            },
		};
		root["primitives"].push_back(primitiveJson);
	}

	editorAudio_.SaveToJson(root["audio"]);
	JsonManager* jsonManager = JsonManager::GetInstance();
	jsonManager->SetData(root);
	return jsonManager->SaveJson(filePath);
}

bool Hierarchy::LoadObjectEditorsFromJson(const std::string& filePath) {
	const std::filesystem::path jsonPath = ResolveObjectEditorJsonPath(filePath);
	std::ifstream file(jsonPath);
	if (!file.is_open()) {
		return false;
	}

	nlohmann::json root;
	try {
		file >> root;
	} catch (const nlohmann::json::parse_error&) {
		return false;
	}
	if (!root.is_object()) {
		return false;
	}

	std::unordered_map<std::string, size_t> objectIdToIndex;
	for (size_t i = 0; i < objects_.size(); ++i) {
		if (!objects_[i]) {
			continue;
		}
		const std::string& editorId = objects_[i]->GetEditorId();
		if (!editorId.empty()) {
			objectIdToIndex.emplace(editorId, i);
		}
	}

	if (root.contains("objects") && root["objects"].is_array()) {
		for (const auto& objectJson : root["objects"]) {
			size_t index = std::numeric_limits<size_t>::max();
			if (objectJson.contains("editorId") && objectJson["editorId"].is_string()) {
				const std::string editorId = objectJson["editorId"].get<std::string>();
				const auto idIt = objectIdToIndex.find(editorId);
				if (idIt != objectIdToIndex.end()) {
					index = idIt->second;
				}
			}
			if (index == std::numeric_limits<size_t>::max()) {
				if (!objectJson.contains("index") || !objectJson["index"].is_number_unsigned()) {
					continue;
				}
				index = objectJson["index"].get<size_t>();
			}
			if (index >= objects_.size() || !objects_[index]) {
				std::string modelName;
				if (objectJson.contains("modelName") && objectJson["modelName"].is_string()) {
					modelName = objectJson["modelName"].get<std::string>();
				} else if (objectJson.contains("name") && objectJson["name"].is_string()) {
					modelName = objectJson["name"].get<std::string>();
				}
				if (modelName.empty()) {
					continue;
				}
				const std::string editorId = objectJson.value("editorId", std::string());
				Object3d* rawObject = CreateEditorOwnedObject(editorOwnedObjects_, modelName, editorId);
				RegisterObject3d(rawObject);
				const auto objectIt = std::find(objects_.begin(), objects_.end(), rawObject);
				index = objectIt == objects_.end() ? std::numeric_limits<size_t>::max() : static_cast<size_t>(std::distance(objects_.begin(), objectIt));
				if (index == std::numeric_limits<size_t>::max()) {
					continue;
				}
			}
			EnsureObjectEditorDataSize(index + 1, objectNames_, objectModelNames_, editorTransforms_, editorMaterials_);
			if (objectJson.contains("name") && objectJson["name"].is_string()) {
				objectNames_[index] = objectJson["name"].get<std::string>();
			}
			if (objectJson.contains("modelName") && objectJson["modelName"].is_string()) {
				const std::string modelName = objectJson["modelName"].get<std::string>();
				if (!modelName.empty()) {
					if (index >= objectModelNames_.size()) {
						objectModelNames_.resize(index + 1);
					}
					objectModelNames_[index] = modelName;
					LoadEditorObjectModel(modelName);
					objects_[index]->SetModel(modelName);
				}
			}
			if (objectJson.contains("transform") && objectJson["transform"].is_object()) {
				const auto& transformJson = objectJson["transform"];
				if (transformJson.contains("scale") && transformJson["scale"].is_array() && transformJson["scale"].size() == 3 && transformJson.contains("rotate") &&
				    transformJson["rotate"].is_array() && transformJson["rotate"].size() == 3 && transformJson.contains("translate") && transformJson["translate"].is_array() &&
				    transformJson["translate"].size() == 3) {
					Transform transform = objects_[index]->GetTransform();
					transform.scale = {transformJson["scale"][0].get<float>(), transformJson["scale"][1].get<float>(), transformJson["scale"][2].get<float>()};
					transform.rotate = {transformJson["rotate"][0].get<float>(), transformJson["rotate"][1].get<float>(), transformJson["rotate"][2].get<float>()};
					transform.translate = {transformJson["translate"][0].get<float>(), transformJson["translate"][1].get<float>(), transformJson["translate"][2].get<float>()};
					editorTransforms_[index] = transform;
					objects_[index]->SetTransform(transform);
				}
			}
			InspectorMaterial material = editorMaterials_[index];
			if (objectJson.contains("material") && objectJson["material"].is_object()) {
				const auto& materialJson = objectJson["material"];
				if (materialJson.contains("color") && materialJson["color"].is_array() && materialJson["color"].size() == 4) {
					material.color = {materialJson["color"][0].get<float>(), materialJson["color"][1].get<float>(), materialJson["color"][2].get<float>(), materialJson["color"][3].get<float>()};
				}
				if (materialJson.contains("enableLighting") && materialJson["enableLighting"].is_boolean()) {
					material.enableLighting = materialJson["enableLighting"].get<bool>();
				}
				if (materialJson.contains("shininess") && materialJson["shininess"].is_number()) {
					material.shininess = materialJson["shininess"].get<float>();
				}
				if (materialJson.contains("environmentCoefficient") && materialJson["environmentCoefficient"].is_number()) {
					material.environmentCoefficient = materialJson["environmentCoefficient"].get<float>();
				}
				if (materialJson.contains("grayscaleEnabled") && materialJson["grayscaleEnabled"].is_boolean()) {
					material.grayscaleEnabled = materialJson["grayscaleEnabled"].get<bool>();
				}
				if (materialJson.contains("sepiaEnabled") && materialJson["sepiaEnabled"].is_boolean()) {
					material.sepiaEnabled = materialJson["sepiaEnabled"].get<bool>();
				}
				if (materialJson.contains("distortionStrength") && materialJson["distortionStrength"].is_number()) {
					material.distortionStrength = materialJson["distortionStrength"].get<float>();
				}
				if (materialJson.contains("distortionFalloff") && materialJson["distortionFalloff"].is_number()) {
					material.distortionFalloff = materialJson["distortionFalloff"].get<float>();
				}
				if (materialJson.contains("uvScale") && materialJson["uvScale"].is_array() && materialJson["uvScale"].size() == 3) {
					material.uvScale = {materialJson["uvScale"][0].get<float>(), materialJson["uvScale"][1].get<float>(), materialJson["uvScale"][2].get<float>()};
				}
				if (materialJson.contains("uvRotate") && materialJson["uvRotate"].is_array() && materialJson["uvRotate"].size() == 3) {
					material.uvRotate = {materialJson["uvRotate"][0].get<float>(), materialJson["uvRotate"][1].get<float>(), materialJson["uvRotate"][2].get<float>()};
				}
				if (materialJson.contains("uvTranslate") && materialJson["uvTranslate"].is_array() && materialJson["uvTranslate"].size() == 3) {
					material.uvTranslate = {materialJson["uvTranslate"][0].get<float>(), materialJson["uvTranslate"][1].get<float>(), materialJson["uvTranslate"][2].get<float>()};
				}
				if (materialJson.contains("uvAnchor") && materialJson["uvAnchor"].is_array() && materialJson["uvAnchor"].size() == 2) {
					material.uvAnchor = {materialJson["uvAnchor"][0].get<float>(), materialJson["uvAnchor"][1].get<float>()};
				}
				if (materialJson.contains("texturePath") && materialJson["texturePath"].is_string()) {
					material.texturePath = materialJson["texturePath"].get<std::string>();
				}
			}
			editorMaterials_[index] = material;
			EditorObject3d::ApplyEditorValues(objects_[index], editorTransforms_[index], material);
		}
	}

	std::unordered_map<std::string, size_t> primitiveIdToIndex;
	for (size_t i = 0; i < primitives_.size(); ++i) {
		if (!primitives_[i] || primitives_[i] == selectionBoxPrimitive_.get()) {
			continue;
		}
		const std::string& editorId = primitives_[i]->GetEditorId();
		if (!editorId.empty()) {
			primitiveIdToIndex.emplace(editorId, i);
		}
	}

	if (root.contains("primitives") && root["primitives"].is_array()) {
		for (const auto& primitiveJson : root["primitives"]) {
			size_t index = std::numeric_limits<size_t>::max();
			if (primitiveJson.contains("editorId") && primitiveJson["editorId"].is_string()) {
				const std::string editorId = primitiveJson["editorId"].get<std::string>();
				const auto idIt = primitiveIdToIndex.find(editorId);
				if (idIt != primitiveIdToIndex.end()) {
					index = idIt->second;
				}
			}
			if (index == std::numeric_limits<size_t>::max()) {
				if (!primitiveJson.contains("index") || !primitiveJson["index"].is_number_unsigned()) {
					continue;
				}
				index = primitiveJson["index"].get<size_t>();
			}
			std::string primitiveTypeName;
			if (primitiveJson.contains("primitiveType") && primitiveJson["primitiveType"].is_string()) {
				primitiveTypeName = primitiveJson["primitiveType"].get<std::string>();
			} else if (primitiveJson.contains("name") && primitiveJson["name"].is_string()) {
				primitiveTypeName = primitiveJson["name"].get<std::string>();
			}
			if (index >= primitives_.size() || !primitives_[index] || primitives_[index] == selectionBoxPrimitive_.get()) {
				const std::string editorId = primitiveJson.value("editorId", std::string());
				Primitive* rawPrimitive = CreateEditorOwnedPrimitive(editorOwnedPrimitives_, PrimitiveTypeFromName(primitiveTypeName), editorId);
				RegisterPrimitive(rawPrimitive);
				const auto primitiveIt = std::find(primitives_.begin(), primitives_.end(), rawPrimitive);
				index = primitiveIt == primitives_.end() ? std::numeric_limits<size_t>::max() : static_cast<size_t>(std::distance(primitives_.begin(), primitiveIt));
			}
			if (index == std::numeric_limits<size_t>::max() || index >= primitives_.size() || !primitives_[index] || primitives_[index] == selectionBoxPrimitive_.get()) {
				continue;
			}
			EnsurePrimitiveEditorDataSize(index + 1, primitiveNames_, primitiveEditorTransforms_, primitiveEditorMaterials_);
			if (primitiveJson.contains("name") && primitiveJson["name"].is_string()) {
				primitiveNames_[index] = primitiveJson["name"].get<std::string>();
			}
			if (primitiveJson.contains("transform") && primitiveJson["transform"].is_object()) {
				const auto& transformJson = primitiveJson["transform"];
				if (transformJson.contains("scale") && transformJson["scale"].is_array() && transformJson["scale"].size() == 3 && transformJson.contains("rotate") &&
				    transformJson["rotate"].is_array() && transformJson["rotate"].size() == 3 && transformJson.contains("translate") && transformJson["translate"].is_array() &&
				    transformJson["translate"].size() == 3) {
					Transform transform = primitives_[index]->GetTransform();
					transform.scale = {transformJson["scale"][0].get<float>(), transformJson["scale"][1].get<float>(), transformJson["scale"][2].get<float>()};
					transform.rotate = {transformJson["rotate"][0].get<float>(), transformJson["rotate"][1].get<float>(), transformJson["rotate"][2].get<float>()};
					transform.translate = {transformJson["translate"][0].get<float>(), transformJson["translate"][1].get<float>(), transformJson["translate"][2].get<float>()};
					primitiveEditorTransforms_[index] = transform;
					primitives_[index]->SetTransform(transform);
				}
			}
			InspectorMaterial material = primitiveEditorMaterials_[index];
			if (primitiveJson.contains("material") && primitiveJson["material"].is_object()) {
				const auto& materialJson = primitiveJson["material"];
				if (materialJson.contains("color") && materialJson["color"].is_array() && materialJson["color"].size() == 4) {
					material.color = {materialJson["color"][0].get<float>(), materialJson["color"][1].get<float>(), materialJson["color"][2].get<float>(), materialJson["color"][3].get<float>()};
				}
				if (materialJson.contains("enableLighting") && materialJson["enableLighting"].is_boolean()) {
					material.enableLighting = materialJson["enableLighting"].get<bool>();
				}
				if (materialJson.contains("shininess") && materialJson["shininess"].is_number()) {
					material.shininess = materialJson["shininess"].get<float>();
				}
				if (materialJson.contains("environmentCoefficient") && materialJson["environmentCoefficient"].is_number()) {
					material.environmentCoefficient = materialJson["environmentCoefficient"].get<float>();
				}
				if (materialJson.contains("grayscaleEnabled") && materialJson["grayscaleEnabled"].is_boolean()) {
					material.grayscaleEnabled = materialJson["grayscaleEnabled"].get<bool>();
				}
				if (materialJson.contains("sepiaEnabled") && materialJson["sepiaEnabled"].is_boolean()) {
					material.sepiaEnabled = materialJson["sepiaEnabled"].get<bool>();
				}
				if (materialJson.contains("distortionStrength") && materialJson["distortionStrength"].is_number()) {
					material.distortionStrength = materialJson["distortionStrength"].get<float>();
				}
				if (materialJson.contains("distortionFalloff") && materialJson["distortionFalloff"].is_number()) {
					material.distortionFalloff = materialJson["distortionFalloff"].get<float>();
				}
				if (materialJson.contains("uvScale") && materialJson["uvScale"].is_array() && materialJson["uvScale"].size() == 3) {
					material.uvScale = {materialJson["uvScale"][0].get<float>(), materialJson["uvScale"][1].get<float>(), materialJson["uvScale"][2].get<float>()};
				}
				if (materialJson.contains("uvRotate") && materialJson["uvRotate"].is_array() && materialJson["uvRotate"].size() == 3) {
					material.uvRotate = {materialJson["uvRotate"][0].get<float>(), materialJson["uvRotate"][1].get<float>(), materialJson["uvRotate"][2].get<float>()};
				}
				if (materialJson.contains("uvTranslate") && materialJson["uvTranslate"].is_array() && materialJson["uvTranslate"].size() == 3) {
					material.uvTranslate = {materialJson["uvTranslate"][0].get<float>(), materialJson["uvTranslate"][1].get<float>(), materialJson["uvTranslate"][2].get<float>()};
				}
				if (materialJson.contains("uvAnchor") && materialJson["uvAnchor"].is_array() && materialJson["uvAnchor"].size() == 2) {
					material.uvAnchor = {materialJson["uvAnchor"][0].get<float>(), materialJson["uvAnchor"][1].get<float>()};
				}
				if (materialJson.contains("texturePath") && materialJson["texturePath"].is_string()) {
					material.texturePath = materialJson["texturePath"].get<std::string>();
				}
			}
			primitiveEditorMaterials_[index] = material;
			EditorPrimitive::ApplyEditorValues(primitives_[index], primitiveEditorTransforms_[index], material);
		}
	}

	if (root.contains("lights") && root["lights"].is_object()) {
		editorLight_.LoadFromJson(root["lights"]);
	}
#ifdef USE_IMGUI
	editorAudio_.LoadFromJson(root.value("audio", nlohmann::json::object()));
#endif
	loadedSnapshot_ = CreateCurrentSnapshot();
	hasLoadedSnapshot_ = true;
	loadedSnapshotFilePath_ = filePath;
	return true;
}
void Hierarchy::HandleHierarchyAssetDrop() {
#ifdef USE_IMGUI
	const ImVec2 min = ImGui::GetWindowPos();
	const ImVec2 max(min.x + ImGui::GetWindowSize().x, min.y + ImGui::GetWindowSize().y);
	const ImGuiID dropTargetId = ImGui::GetID("##HierarchyWindowAssetDropTarget");
	if (!ImGui::BeginDragDropTargetCustom(ImRect(min, max), dropTargetId)) {
		return;
	}
	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(kDragDropPayloadType)) {
		const std::string payloadText(static_cast<const char*>(payload->Data));
		const size_t separator = payloadText.find('|');
		if (separator != std::string::npos) {
			const std::string category = payloadText.substr(0, separator);
			const std::string name = payloadText.substr(separator + 1);
			if (category == "Primitive") {
				AddPrimitiveAssetToHierarchy(name);
			} else if (category == "Object3d") {
				AddObject3dAssetToHierarchy(name);
			} else if (category == "Audio") {
				AddAudioAssetToHierarchy(name);
			}
		}
	}
	ImGui::EndDragDropTarget();
#endif
}
void Hierarchy::DrawSceneSelector() {
#ifdef USE_IMGUI
	SceneManager* sceneManager = SceneManager::GetInstance();
	if (!sceneManager) {
		return;
	}
	const std::vector<std::string> sceneNames = sceneManager->GetSceneNames();
	if (sceneNames.empty()) {
		ImGui::TextUnformatted("No scene list available");
		return;
	}

	const std::string& currentScene = sceneManager->GetCurrentSceneName();
	if (ImGui::BeginCombo("Scene", currentScene.empty() ? "(none)" : currentScene.c_str())) {
		for (const std::string& sceneName : sceneNames) {
			const bool isSelected = (sceneName == currentScene);
			if (ImGui::Selectable(sceneName.c_str(), isSelected) && !isSelected) {
				sceneManager->ChangeScene(sceneName);
				hasUnsavedChanges_ = false;
				saveStatusMessage_ = "Scene changed: " + sceneName;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
#else
	SceneManager* sceneManager = SceneManager::GetInstance();
	if (!sceneManager) {
		return;
	}
#endif
}


void Hierarchy::SetPlayMode(bool isPlaying) {
	const bool wasPlaying = isPlaying_;
	isPlaying_ = isPlaying;
	if (!isPlaying_) {
		isPaused_ = false;
	}
	editorAudio_.OnPlayModeChanged(isPlaying_, wasPlaying);
	if (isPlaying_) {
		isPaused_ = false;
		editorCamera_.DeactivatePreview();
	}
}
void Hierarchy::StartPlayMode() {

	for (CharacterParameterEditorData& data : characterParameterEditorDatas_) {
		if (data.name.empty() || !data.hasUnsavedChanges) {
			continue;
		}
		SaveCharacterTuningJson(data.name, data.lv1Base, data.lv1Parameter, data.levelUpBase, data.currentBase, data.currentParameter, data.reinforcementAmount);
		data.hasUnsavedChanges = false;
	}

	SceneManager::GetInstance()->RequestReinitializeCurrentScene();
	SetPlayMode(true);
}
void Hierarchy::DrawEditorGridLines() {
#ifdef USE_IMGUI
	DrawCameraBillboards();
#endif
	Object3dCommon* object3dCommon = Object3dCommon::GetInstance();
	if (object3dCommon) {
		const bool hasEditorOwnedObject = std::any_of(editorOwnedObjects_.begin(), editorOwnedObjects_.end(), [](const auto& object) { return object != nullptr; });
		const bool hasEditorOwnedPrimitive = std::any_of(editorOwnedPrimitives_.begin(), editorOwnedPrimitives_.end(), [](const auto& primitive) { return primitive != nullptr; });
		if (hasEditorOwnedObject || hasEditorOwnedPrimitive) {
			object3dCommon->DrawCommon();
		}
	}
	Camera* defaultCamera = Object3dCommon::GetInstance()->GetDefaultCamera();
	for (const auto& object : editorOwnedObjects_) {
		if (object) {
			object->SetCamera(defaultCamera);
			object->Update();
			object->Draw();
		}
	}
	for (const auto& primitive : editorOwnedPrimitives_) {
		if (primitive) {
			primitive->SetCamera(defaultCamera);
			primitive->Update();
			primitive->Draw();
		}
	}
#ifdef USE_IMGUI
	EditorGrid::DrawEditorGridLines(gridSettings_, editorGridPlane_);
	if (!showSelectionBox_ || !IsObjectSelected()) {
		return;
	}
	if (selectionBoxDirty_ || !selectionBoxPrimitive_) {
		SyncSelectionBoxToTarget();
		selectionBoxDirty_ = false;
	}
	if (!selectionBoxPrimitive_) {
		return;
	}
	selectionBoxPrimitive_->SetCamera(Object3dCommon::GetInstance()->GetDefaultCamera());
	Object3dCommon::GetInstance()->DrawCommon(Object3dCommon::DrawCommonType::WireframeNoDepth);
	selectionBoxPrimitive_->Update();
	selectionBoxPrimitive_->Draw();
#endif
}
void Hierarchy::DrawCameraBillboards() { editorCamera_.DrawCameraBillboards(isPlaying_); }
void Hierarchy::DrawCameraEditor() { editorCamera_.DrawCameraEditor(); }
void Hierarchy::DrawCharacterParameterHierarchy() {
#ifdef USE_IMGUI
	if (characterParameterEditorDatas_[0].name.empty()) {
		const std::array<std::string, 4> names = {"Arte", "Sizuku", "Yuzuki", "Mei"};
		for (size_t i = 0; i < characterParameterEditorDatas_.size(); ++i) {
			characterParameterEditorDatas_[i].name = names[i];
			characterParameterEditorDatas_[i].lv1Base = {100.0f, 20.0f, 10.0f};
			characterParameterEditorDatas_[i].lv1Parameter = CreateDefaultCharacterParameter();
			characterParameterEditorDatas_[i].levelUpBase = {0.0f, 0.0f, 0.0f};
			characterParameterEditorDatas_[i].reinforcementAmount = LoadReinforcementAmount(names[i]);
			characterParameterEditorDatas_[i].currentBase =
			    CalculateCurrentBaseParameter(characterParameterEditorDatas_[i].lv1Base, characterParameterEditorDatas_[i].levelUpBase, characterParameterEditorDatas_[i].currentParameter.level);
			characterParameterEditorDatas_[i].currentParameter =
			    CalculateCurrentParameter(characterParameterEditorDatas_[i].lv1Parameter, characterParameterEditorDatas_[i].levelUpBase, characterParameterEditorDatas_[i].currentParameter.level);
		}
	}

	ImGui::SeparatorText("Character Parameters");
	for (size_t i = 0; i < characterParameterEditorDatas_.size(); ++i) {
		const auto& data = characterParameterEditorDatas_[i];
		const bool selected = selectedIsCharacterParameter_ && selectedCharacterParameterIndex_ == i;
		if (ImGui::Selectable((data.name + "##character_parameter_select_" + std::to_string(i)).c_str(), selected)) {
			selectedCharacterParameterIndex_ = i;
			selectedIsCharacterParameter_ = true;
			selectedIsPrimitive_ = false;
		}
	}
#endif
}

void Hierarchy::DrawCharacterParameterInspector() {
#ifdef USE_IMGUI
	if (!IsCharacterParameterSelected()) {
		return;
	}
	CharacterParameterEditorData& data = characterParameterEditorDatas_[selectedCharacterParameterIndex_];
	const BaseParameter previousLv1Base = data.lv1Base;
	const BaseParameter previousLevelUpBase = data.levelUpBase;
	const json previousLv1Parameter = ToJson(data.lv1Parameter);
	const int previousCurrentLevel = data.currentParameter.level;
	const int previousReinforcementAmount = data.reinforcementAmount;
	ImGui::Text("Character Parameter: %s", data.name.c_str());
	ImGui::Text("LV1: Resources/JSON/Character/%s/lv1_parameters.json", data.name.c_str());
	ImGui::Text("Current: Resources/JSON/Character/%s/current_parameters.json", data.name.c_str());

	ImGui::PushID("lv1_base");
	DrawBaseParameterEditor("LV1 Base Parameters", data.lv1Base);
	ImGui::PopID();
	ImGui::PushID("level_up_base");
	DrawBaseParameterEditor("Base Parameter Increase Per Level", data.levelUpBase);
	ImGui::PopID();
	const int currentLevel = data.currentParameter.level;
	data.currentBase = CalculateCurrentBaseParameter(data.lv1Base, data.levelUpBase, currentLevel);
	data.currentParameter = CalculateCurrentParameter(data.lv1Parameter, data.levelUpBase, currentLevel);
	ImGui::PushID("current_base");
	DrawBaseParameterViewer("Current Base Parameters", data.currentBase);
	ImGui::PopID();
	DrawParameterEditor("LV1 Parameter", data.lv1Parameter);
	DrawParameterEditor("Current Level Parameter", data.currentParameter, true);
	ImGui::SeparatorText("Reinforcement");
	ImGui::DragInt("Reinforcement Amount", &data.reinforcementAmount, 1.0f, 0, 6);
	data.currentBase = CalculateCurrentBaseParameter(data.lv1Base, data.levelUpBase, data.currentParameter.level);
	data.currentParameter = CalculateCurrentParameter(data.lv1Parameter, data.levelUpBase, data.currentParameter.level);
	if (!IsNearlyEqual(previousLv1Base.HP, data.lv1Base.HP) || !IsNearlyEqual(previousLv1Base.Attack, data.lv1Base.Attack) || !IsNearlyEqual(previousLv1Base.Deffence, data.lv1Base.Deffence) ||
	    !IsNearlyEqual(previousLevelUpBase.HP, data.levelUpBase.HP) || !IsNearlyEqual(previousLevelUpBase.Attack, data.levelUpBase.Attack) ||
	    !IsNearlyEqual(previousLevelUpBase.Deffence, data.levelUpBase.Deffence) || previousLv1Parameter != ToJson(data.lv1Parameter) || previousCurrentLevel != data.currentParameter.level ||
	    previousReinforcementAmount != data.reinforcementAmount) {
		data.hasUnsavedChanges = true;
	}

	if (ImGui::Button("Save Character Json")) {
		SaveCharacterTuningJson(data.name, data.lv1Base, data.lv1Parameter, data.levelUpBase, data.currentBase, data.currentParameter, data.reinforcementAmount);
		data.hasUnsavedChanges = false;
		saveStatusMessage_ = "Saved character parameters: " + data.name;
	}
#endif
}

bool Hierarchy::IsCharacterParameterSelected() const {
	return selectedIsCharacterParameter_ && selectedCharacterParameterIndex_ < characterParameterEditorDatas_.size() && !characterParameterEditorDatas_[selectedCharacterParameterIndex_].name.empty();
}
void Hierarchy::DrawLightEditor() {
#ifdef USE_IMGUI
	if (editorLight_.DrawEditor(isPlaying_)) {
		hasUnsavedChanges_ = true;
	}
#endif
}

bool Hierarchy::IsObjectSelected() const {
	if (selectedIsCharacterParameter_) {
		return false;
	}
	if (selectedIsPrimitive_) {
		return selectedObjectIndex_ < primitives_.size() && primitives_[selectedObjectIndex_] != nullptr;
	}
	return selectedObjectIndex_ < objects_.size() && objects_[selectedObjectIndex_] != nullptr;
}

Transform Hierarchy::GetSelectedTransform() const {
	if (selectedIsPrimitive_) {
		if (selectedObjectIndex_ < primitiveEditorTransforms_.size()) {
			return primitiveEditorTransforms_[selectedObjectIndex_];
		}
	} else {
		if (selectedObjectIndex_ < editorTransforms_.size()) {
			return editorTransforms_[selectedObjectIndex_];
		}
	}
	return Transform{};
}

void Hierarchy::SyncSelectionBoxToTarget() {
	if (!showSelectionBox_ || !IsObjectSelected()) {
		return;
	}
	if (!selectionBoxPrimitive_) {
		selectionBoxPrimitive_ = std::make_unique<Primitive>();
		selectionBoxPrimitive_->SetEditorRegistrationEnabled(false);
		selectionBoxPrimitive_->Initialize(Primitive::Box);
		selectionBoxPrimitive_->SetEnableLighting(false);
		selectionBoxPrimitive_->SetColor({1.0f, 0.9f, 0.1f, 1.0f});
	}
	selectionBoxPrimitive_->SetCamera(Object3dCommon::GetInstance()->GetDefaultCamera());
	const Transform selectedTransform = GetSelectedTransform();
	selectionBoxPrimitive_->SetTransform(selectedTransform);
}

void Hierarchy::DrawSelectionBoxEditor() {
#ifdef USE_IMGUI
	if (ImGui::Checkbox("Draw Selection Box", &showSelectionBox_)) {
		selectionBoxDirty_ = true;
	}
#endif
}
void Hierarchy::DrawSelectedObjectGuizmo(const ImGuiViewport* viewport, float contentStartY, float leftPanelWidth, float rightPanelWidth, float availableHeight) {
#ifdef USE_IMGUI
	if (isPlaying_ || !IsObjectSelected()) {
		PushGuizmoUndoIfNeeded();
		return;
	}

	(void)contentStartY;
	(void)availableHeight;

	Camera* camera = Object3dCommon::GetInstance()->GetDefaultCamera();
	if (!camera) {
		PushGuizmoUndoIfNeeded();
		return;
	}

	ImGuizmo::BeginFrame();
	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList(const_cast<ImGuiViewport*>(viewport)));

	constexpr float kRenderedToolbarHeight = 44.0f;
	constexpr float kGameAspect = 16.0f / 9.0f;
	const float availableWidth = std::max(1.0f, viewport->WorkSize.x - leftPanelWidth - rightPanelWidth);
	const float renderedAvailableHeight = std::max(1.0f, viewport->WorkSize.y - kRenderedToolbarHeight);

	float sceneWidth = availableWidth;
	float sceneHeight = sceneWidth / kGameAspect;
	if (sceneHeight > renderedAvailableHeight) {
		sceneHeight = renderedAvailableHeight;
		sceneWidth = sceneHeight * kGameAspect;
	}

	const float scenePosX = viewport->WorkPos.x + leftPanelWidth + (availableWidth - sceneWidth) * 0.5f;
	const float scenePosY = viewport->WorkPos.y + kRenderedToolbarHeight;
	ImGuizmo::SetRect(scenePosX, scenePosY, std::max(1.0f, sceneWidth), std::max(1.0f, sceneHeight));

	Transform& transform = selectedIsPrimitive_ ? primitiveEditorTransforms_[selectedObjectIndex_] : editorTransforms_[selectedObjectIndex_];
	Matrix4x4 worldMatrix = Function::MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	const EditorSnapshot beforeGuizmoEdit = CreateCurrentSnapshot();
	const Transform startTransform = transform;

	if (ImGui::IsKeyPressed(ImGuiKey_W, false)) {
		currentGuizmoOperation_ = 0;
	} else if (ImGui::IsKeyPressed(ImGuiKey_E, false)) {
		currentGuizmoOperation_ = 1;
	} else if (ImGui::IsKeyPressed(ImGuiKey_R, false)) {
		currentGuizmoOperation_ = 2;
	}

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	if (currentGuizmoOperation_ == 1) {
		operation = ImGuizmo::ROTATE;
	} else if (currentGuizmoOperation_ == 2) {
		operation = ImGuizmo::SCALE;
	}

	const ImVec2 operationWindowPos{scenePosX + 12.0f, scenePosY + 12.0f};
	ImGui::SetNextWindowPos(operationWindowPos, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0.65f);
	if (ImGui::Begin("##GuizmoOperation", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav)) {
		if (ImGui::RadioButton("Move (T)", currentGuizmoOperation_ == 0)) {
			currentGuizmoOperation_ = 0;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate (R)", currentGuizmoOperation_ == 1)) {
			currentGuizmoOperation_ = 1;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale (S)", currentGuizmoOperation_ == 2)) {
			currentGuizmoOperation_ = 2;
		}
	}
	ImGui::End();

	const ImGuizmo::MODE gizmoMode = operation == ImGuizmo::TRANSLATE ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
	if (ImGuizmo::Manipulate(&camera->GetViewMatrix().m[0][0], &camera->GetProjectionMatrix().m[0][0], operation, gizmoMode, &worldMatrix.m[0][0])) {
		float translation[3]{};
		float rotationDegrees[3]{};
		float scale[3]{};
		ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotationDegrees, scale);

		transform.translate = {translation[0], translation[1], translation[2]};
		transform.rotate = {rotationDegrees[0] * kDegreesToRadians, rotationDegrees[1] * kDegreesToRadians, rotationDegrees[2] * kDegreesToRadians};
		transform.scale = {scale[0], scale[1], scale[2]};

		if (selectedIsPrimitive_) {
			if (Primitive* primitive = primitives_[selectedObjectIndex_]) {
				primitive->SetTransform(transform);
			}
		} else {
			if (Object3d* object = objects_[selectedObjectIndex_]) {
				object->SetTransform(transform);
			}
		}
		selectionBoxDirty_ = true;
		hasUnsavedChanges_ = true;
	}

	const bool isUsingGuizmo = ImGuizmo::IsUsing();
	if (isUsingGuizmo && !guizmoWasUsing_) {
		guizmoBeforeEdit_ = beforeGuizmoEdit;
		guizmoStartTransform_ = startTransform;
		guizmoTargetIndex_ = selectedObjectIndex_;
		guizmoTargetIsPrimitive_ = selectedIsPrimitive_;
		guizmoWasUsing_ = true;
	} else if (!isUsingGuizmo) {
		PushGuizmoUndoIfNeeded();
	}
#else
	(void)viewport;
	(void)contentStartY;
	(void)leftPanelWidth;
	(void)rightPanelWidth;
	(void)availableHeight;
#endif
}
void Hierarchy::DrawObjectEditors() {
	LoadObjectEditorsFromJsonIfExists("objectEditors.json");
#ifdef USE_IMGUI
	if (!isPlaying_ && !ImGui::IsAnyItemActive()) {
		if (EditorCommand::GetUndo()) {
			UndoEditorChange();
			saveStatusMessage_ = "Undo";
		}
		if (EditorCommand::GetRedo()) {
			RedoEditorChange();
			saveStatusMessage_ = "Redo";
		}
	}
	if (!isPlaying_) {
		for (size_t i = 0; i < objects_.size(); ++i) {
			Object3d* object = objects_[i];
			if (!object) {
				continue;
			}
			EditorObject3d::ApplyEditorValues(object, editorTransforms_[i], editorMaterials_[i]);
		}

		for (size_t i = 0; i < primitives_.size(); ++i) {
			Primitive* primitive = primitives_[i];
			if (!primitive) {
				continue;
			}
			EditorPrimitive::ApplyEditorValues(primitive, primitiveEditorTransforms_[i], primitiveEditorMaterials_[i]);
		}
	}
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const float kTopToolbarHeight = 80.0f;
	const float kLeftPanelRatio = 0.22f;
	const float kRightPanelRatio = 0.24f;
	const float kPanelMinWidth = 260.0f;
	const float availableHeight = std::max(1.0f, viewport->WorkSize.y - kTopToolbarHeight);
	const float leftPanelWidth = std::max(kPanelMinWidth, viewport->WorkSize.x * kLeftPanelRatio);
	const float rightPanelWidth = std::max(kPanelMinWidth, viewport->WorkSize.x * kRightPanelRatio);


	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, kTopToolbarHeight), ImGuiCond_Always);
	if (ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar)) {
		ToolBar::Result toolbarResult = ToolBar::Draw(isPlaying_, isPaused_, hasUnsavedChanges_, !undoStack_.empty(), !redoStack_.empty(), showGridWindow_, gridSettings_);
		if (toolbarResult.saveRequested) {
			if (!isPlaying_) {
				const std::string saveFilePath = GetSceneScopedEditorFilePath("objectEditors.json");
				const bool saved = SaveObjectEditorsToJson(saveFilePath);
				if (saved) {
					hasUnsavedChanges_ = false;
				}
				saveStatusMessage_ = saved ? ("Saved: " + saveFilePath) : ("Save failed: " + saveFilePath);
			}
		}
		if (toolbarResult.spriteVisibilityChanged) {
			saveStatusMessage_ = SpriteCommon::GetInstance()->IsSpriteVisible() ? "View: show all sprites" : "View: hide all sprites";
		}
		if (toolbarResult.allResetRequested) {
			const bool reset = ResetToLoadedSnapshot();
			saveStatusMessage_ = reset ? "AllReset: restored loaded values" : "AllReset failed: no loaded data";
		}
		if (toolbarResult.undoRequested) {
			UndoEditorChange();
			saveStatusMessage_ = "Undo";
		}
		if (toolbarResult.redoRequested) {
			RedoEditorChange();
			saveStatusMessage_ = "Redo";
		}
		if (toolbarResult.pauseRequested && isPlaying_) {
			isPaused_ = true;
			saveStatusMessage_ = "Paused";
		}
		if (toolbarResult.resumeRequested && isPlaying_) {
			isPaused_ = false;
			saveStatusMessage_ = "Playing";
		}
		if (toolbarResult.playRequested) {
			if (hasUnsavedChanges_) {
				ImGui::OpenPopup("Unsaved Changes");
			} else {
				StartPlayMode();
				saveStatusMessage_ = "Playing";
			}
		}

		if (ImGui::BeginPopupModal("Unsaved Changes", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::TextUnformatted("There are unsaved changes. Do you want to continue playing?");
			ImGui::Separator();

			if (ImGui::Button("YES", ImVec2(100.0f, 0.0f))) {
				StartPlayMode();
				saveStatusMessage_ = "Playing (unsaved changes kept)";
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("NO", ImVec2(100.0f, 0.0f))) {
				saveStatusMessage_ = "Play canceled";
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Save", ImVec2(100.0f, 0.0f))) {
				const std::string saveFilePath = GetSceneScopedEditorFilePath("objectEditors.json");
				const bool saved = SaveObjectEditorsToJson(saveFilePath);
				if (saved) {
					hasUnsavedChanges_ = false;
					StartPlayMode();
					saveStatusMessage_ = "Saved and Playing: " + saveFilePath;
					ImGui::CloseCurrentPopup();
				} else {
					saveStatusMessage_ = "Save failed: " + saveFilePath;
				}
			}

			ImGui::EndPopup();
		}
		if (toolbarResult.stopRequested) {
			SetPlayMode(false);
			SceneManager::GetInstance()->RequestReinitializeCurrentScene();
			saveStatusMessage_ = "Stopped: restored editor state";
		}
	}
	ImGui::End();

	const float contentStartY = viewport->WorkPos.y + kTopToolbarHeight;
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, contentStartY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(leftPanelWidth, availableHeight), ImGuiCond_Always);
	if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar)) {
		ImGui::Text("Auto Object Editor");
		ImGui::Separator();
		HandleHierarchyAssetDrop();
		ImGui::TextDisabled("Assetからここへドラッグ&ドロップでHierarchyへ追加");
		ImGui::Separator();
		ImGui::SeparatorText("Scene Switch");
		DrawSceneSelector();
		if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			DrawLightEditor();
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			DrawCameraEditor();
			ImGui::TreePop();
		}
		ImGui::Separator();
		if (ImGui::TreeNodeEx("Audio", ImGuiTreeNodeFlags_DefaultOpen)) {
			editorAudio_.DrawEditor(isPlaying_, hasUnsavedChanges_);
			ImGui::TreePop();
		}
		ImGui::SeparatorText("Selection");
		DrawSelectionBoxEditor();
		ImGui::Separator();

		if (!saveStatusMessage_.empty()) {
			ImGui::Text("%s", saveStatusMessage_.c_str());
		}

		if (ImGui::TreeNodeEx("Object3d", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (size_t i = 0; i < objects_.size(); ++i) {
				Object3d* object = objects_[i];
				if (!object) {
					continue;
				}
				std::string displayName = objectNames_[i].empty() ? ("Object " + std::to_string(i)) : objectNames_[i];
				const bool selected = (!selectedIsCharacterParameter_ && !selectedIsPrimitive_ && selectedObjectIndex_ == i);
				if (ImGui::Selectable((displayName + "##object_select_" + std::to_string(i)).c_str(), selected)) {
					selectedObjectIndex_ = i;
					selectedIsPrimitive_ = false;
					selectedIsCharacterParameter_ = false;
					selectionBoxDirty_ = true;
				}
				if (ImGui::BeginPopupContextItem(("##object_context_" + std::to_string(i)).c_str())) {
					if (ImGui::MenuItem("delete")) {
						DeleteObjectAtIndex(i);
					}
					ImGui::EndPopup();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Primitive", ImGuiTreeNodeFlags_DefaultOpen)) {
			for (size_t i = 0; i < primitives_.size(); ++i) {
				Primitive* primitive = primitives_[i];
				if (!primitive) {
					continue;
				}
				std::string displayName = primitiveNames_[i].empty() ? ("Primitive " + std::to_string(i)) : primitiveNames_[i];
				const bool selected = (!selectedIsCharacterParameter_ && selectedIsPrimitive_ && selectedObjectIndex_ == i);
				if (ImGui::Selectable((displayName + "##primitive_select_" + std::to_string(i)).c_str(), selected)) {
					selectedObjectIndex_ = i;
					selectedIsPrimitive_ = true;
					selectedIsCharacterParameter_ = false;
					selectionBoxDirty_ = true;
				}
				if (ImGui::BeginPopupContextItem(("##primitive_context_" + std::to_string(i)).c_str())) {
					if (ImGui::MenuItem("delete")) {
						DeletePrimitiveAtIndex(i);
					}
					ImGui::EndPopup();
				}
			}
			ImGui::TreePop();
		}
		DrawCharacterParameterHierarchy();
	}
	ImGui::End();
	const float inspectorPosX = viewport->WorkPos.x + viewport->WorkSize.x - rightPanelWidth;
	ImGui::SetNextWindowPos(ImVec2(inspectorPosX, contentStartY), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(rightPanelWidth, availableHeight), ImGuiCond_Always);
	if (ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_HorizontalScrollbar)) {
		if (IsCharacterParameterSelected()) {
			DrawCharacterParameterInspector();
		} else if (!IsObjectSelected()) {
			ImGui::TextUnformatted("No object selected.");
		} else {
			EditorSnapshot beforeEdit{};
			beforeEdit.objectTransforms = editorTransforms_;
			beforeEdit.objectMaterials = editorMaterials_;
			beforeEdit.objectNames = objectNames_;
			beforeEdit.objectModelNames = objectModelNames_;
			beforeEdit.primitiveTransforms = primitiveEditorTransforms_;
			beforeEdit.primitiveMaterials = primitiveEditorMaterials_;
			beforeEdit.primitiveNames = primitiveNames_;
			bool transformChanged = false;
			bool materialChanged = false;
			bool nameChanged = false;
			bool modelChanged = false;
			if (selectedIsPrimitive_) {
				Primitive* primitive = primitives_[selectedObjectIndex_];
				if (primitive) {
					Transform& transform = primitiveEditorTransforms_[selectedObjectIndex_];
					InspectorMaterial& material = primitiveEditorMaterials_[selectedObjectIndex_];
					std::string& name = primitiveNames_[selectedObjectIndex_];
					Inspector::DrawPrimitiveInspector(selectedObjectIndex_, name, transform, material, isPlaying_, transformChanged, materialChanged, nameChanged);
					if (transformChanged) {
						selectionBoxDirty_ = true;
						primitive->SetTransform(transform);
					}
					if (materialChanged) {
						EditorPrimitive::ApplyEditorValues(primitive, transform, material);
					}
				}
			} else {
				Object3d* object = objects_[selectedObjectIndex_];
				if (object) {
					Transform& transform = editorTransforms_[selectedObjectIndex_];
					InspectorMaterial& material = editorMaterials_[selectedObjectIndex_];
					std::string& name = objectNames_[selectedObjectIndex_];
					if (selectedObjectIndex_ >= objectModelNames_.size()) {
						objectModelNames_.resize(selectedObjectIndex_ + 1);
					}
					if (objectModelNames_[selectedObjectIndex_].empty()) {
						objectModelNames_[selectedObjectIndex_] = object->GetModelFilePath();
					}
					std::string& modelName = objectModelNames_[selectedObjectIndex_];
					Inspector::DrawObjectInspector(selectedObjectIndex_, name, modelName, transform, material, isPlaying_, transformChanged, materialChanged, nameChanged, modelChanged);
					if (transformChanged) {
						selectionBoxDirty_ = true;
						object->SetTransform(transform);
					}
					if (materialChanged) {
						EditorObject3d::ApplyEditorValues(object, transform, material);
					}
					if (modelChanged && !modelName.empty()) {
						LoadEditorObjectModel(modelName);
						object->SetModel(modelName);
						selectionBoxDirty_ = true;
					}
				}
			}
			if (transformChanged || materialChanged || nameChanged || modelChanged) {
				undoStack_.push_back(std::move(beforeEdit));
				redoStack_.clear();
				hasUnsavedChanges_ = true;
			}
		}
	}
	ImGui::End();

	DrawSelectedObjectGuizmo(viewport, contentStartY, leftPanelWidth, rightPanelWidth, availableHeight);
#endif
}
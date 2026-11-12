#define NOMINMAX
#include "CharacterScene.h"

#include <algorithm>
#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif

namespace {
using Json = nlohmann::json;

Json ReadJson(const std::filesystem::path& path) {
	std::ifstream stream(path);

	if (!stream) {
		throw std::runtime_error("Cannot open: " + path.string());
	}

	Json root;
	stream >> root;
	return root;
}

void WriteJson(const std::filesystem::path& path, const Json& root) {
	std::ofstream stream(path);

	if (!stream) {
		throw std::runtime_error("Cannot write: " + path.string());
	}

	stream << root.dump(2);
	stream.flush();

	if (!stream) {
		throw std::runtime_error("Write failed: " + path.string());
	}
}

void ValidateRoot(const Json& root) {
	for (const char* section : {"base", "baseLevelUp", "parameter"}) {
		if (!root.at(section).is_object()) {
			throw std::runtime_error(std::string("Invalid section: ") + section);
		}
	}

	for (const char* section : {"base", "baseLevelUp", "parameter"}) {
		for (const char* key : {"HP", "Attack", "Deffence"}) {
			root.at(section).at(key).get<float>();
		}
	}

	root.at("parameter").at("Level").get<int>();
	root.value("reinforcementAmount", 0);
}

#ifdef USE_IMGUI
bool EditFloat(Json& object, const char* key, float speed, float minimum, float maximum, float fallback = 0.0f) {
	float value = object.value(key, fallback);

	if (!ImGui::DragFloat(key, &value, speed, minimum, maximum)) {
		return false;
	}

	object[key] = std::clamp(value, minimum, maximum);
	return true;
}

bool EditBase(const char* label, Json& object) {
	ImGui::PushID(label);
	ImGui::SeparatorText(label);

	bool changed = false;
	changed |= EditFloat(object, "HP", 1.0f, 0.0f, 99999.0f);
	changed |= EditFloat(object, "Attack", 0.1f, 0.0f, 99999.0f);
	changed |= EditFloat(object, "Deffence", 0.1f, 0.0f, 99999.0f);

	ImGui::PopID();
	return changed;
}

bool EditRates(const char* label, Json& parameter) {
	if (!parameter.contains(label)) {
		return false;
	}

	Json& rates = parameter.at(label);

	if (!rates.is_array()) {
		throw std::runtime_error(std::string("Invalid array: ") + label);
	}

	ImGui::PushID(label);
	ImGui::SeparatorText(label);

	bool changed = false;

	for (std::size_t i = 0; i < rates.size(); ++i) {
		ImGui::PushID(static_cast<int>(i));

		float value = rates.at(i).get<float>();
		const std::string name = "Attribute " + std::to_string(i);

		if (ImGui::DragFloat(name.c_str(), &value, 0.01f)) {
			rates[i] = value;
			changed = true;
		}

		ImGui::PopID();
	}

	ImGui::PopID();
	return changed;
}

bool EditParameter(Json& parameter) {
	ImGui::PushID("LV1 Parameter");

	bool changed = EditBase("LV1 Parameter", parameter);

	changed |= EditFloat(parameter, "Speed", 0.01f, 0.0f, 100.0f);
	changed |= EditFloat(parameter, "CriticalRate", 0.001f, 0.0f, 1.0f);
	changed |= EditFloat(parameter, "CriticalDamage", 0.01f, 1.0f, 10.0f);
	changed |= EditFloat(parameter, "HealingEffect", 0.01f, 0.0f, 999.0f);
	changed |= EditFloat(parameter, "AttributeAffinity", 0.01f, 0.0f, 999.0f, 1.0f);

	changed |= EditRates("AttributeDamageRate", parameter);
	changed |= EditRates("AttributeResistanceRate", parameter);

	ImGui::PopID();
	return changed;
}

void ShowBase(const char* label, const Json& object) {
	ImGui::SeparatorText(label);

	for (const char* key : {"HP", "Attack", "Deffence"}) {
		ImGui::Text("%s: %.2f", key, object.at(key).get<float>());
	}
}
#endif
} // namespace

void CharacterScene::Initialize() {
	// エディターの再生開始でも Initialize が呼ばれる。
	// 同じインスタンス内の未保存データは維持する。
	if (initialized_) {
		return;
	}

	const std::array<const char*, 4> names = {"Arte", "Sizuku", "Yuzuki", "Mei"};

	for (std::size_t i = 0; i < characters_.size(); ++i) {
		characters_[i].name = names[i];
		Load(characters_[i]);
	}

	selectedIndex_ = 0;
	initialized_ = true;
}

void CharacterScene::Update() {
	// データ編集専用シーンなので、ゲーム更新は行わない。
}

void CharacterScene::Draw() {
	// ImGui は DrawEditorUI から構築する。
}

void CharacterScene::Finalize() {
	// 自動保存は行わない。
	// シーンを離れるときは Save を押してから切り替える。
}

void CharacterScene::Load(CharacterData& data) {
	try {
		const std::filesystem::path directory = std::filesystem::path("Resources/JSON/Character") / data.name;

		// 両方の読み込みが成功してから編集データへ反映する。
		Json lv1 = ReadJson(directory / "lv1_parameters.json");
		Json current = ReadJson(directory / "current_parameters.json");

		ValidateRoot(lv1);
		ValidateRoot(current);

		data.lv1 = std::move(lv1);
		data.current = std::move(current);
		data.loaded = true;
		data.dirty = false;

		status_ = "Loaded: " + data.name;
	} catch (const std::exception& error) {
		status_ = error.what();
	}
}

void CharacterScene::Recalculate(CharacterData& data) {
	const int level = std::clamp(data.current.at("parameter").at("Level").get<int>(), 1, 100);

	const float offset = static_cast<float>(level - 1);
	const Json& increase = data.lv1.at("baseLevelUp");

	// 現在値は LV1 の能力を基に計算する。
	Json parameter = data.lv1.at("parameter");
	parameter["Level"] = level;

	for (const char* key : {"HP", "Attack", "Deffence"}) {
		const float growth = increase.at(key).get<float>() * offset;

		data.current["base"][key] = data.lv1.at("base").at(key).get<float>() + growth;

		parameter[key] = data.lv1.at("parameter").at(key).get<float>() + growth;
	}

	data.current["parameter"] = std::move(parameter);
	data.current["baseLevelUp"] = increase;
}

void CharacterScene::Save(CharacterData& data) {
	if (!data.loaded) {
		return;
	}

	try {
		const std::filesystem::path directory = std::filesystem::path("Resources/JSON/Character") / data.name;

		// 既存形式に合わせ、強化回数は両方のファイルへ保存する。
		data.lv1["reinforcementAmount"] = data.current.value("reinforcementAmount", 0);

		WriteJson(directory / "lv1_parameters.json", data.lv1);
		WriteJson(directory / "current_parameters.json", data.current);

		data.dirty = false;
		status_ = "Saved: " + data.name;
	} catch (const std::exception& error) {
		status_ = error.what();
	}
}

void CharacterScene::DrawEditorUI() {
#ifdef USE_IMGUI
	if (!ImGui::Begin("Character Parameters")) {
		ImGui::End();
		return;
	}

	ImGui::TextUnformatted("Save changes before switching scenes.");

	for (std::size_t i = 0; i < characters_.size(); ++i) {
		if (i != 0) {
			ImGui::SameLine();
		}

		const CharacterData& character = characters_[i];
		const std::string label = character.name + (character.dirty ? " *" : "");

		if (ImGui::Selectable(label.c_str(), selectedIndex_ == i, 0, ImVec2(90.0f, 0.0f))) {
			selectedIndex_ = i;
		}
	}

	CharacterData& data = characters_[selectedIndex_];

	if (data.loaded) {
		try {
			bool changed = false;

			changed |= EditBase("LV1 Base", data.lv1.at("base"));
			changed |= EditBase("Increase Per Level", data.lv1.at("baseLevelUp"));
			changed |= EditParameter(data.lv1.at("parameter"));

			ImGui::SeparatorText("Current");

			int level = data.current.at("parameter").at("Level").get<int>();

			if (ImGui::DragInt("Level", &level, 1.0f, 1, 100)) {
				data.current["parameter"]["Level"] = std::clamp(level, 1, 100);
				changed = true;
			}

			int reinforcement = data.current.value("reinforcementAmount", 0);

			if (ImGui::DragInt("Reinforcement Amount", &reinforcement, 1.0f, 0, 6)) {
				data.current["reinforcementAmount"] = std::clamp(reinforcement, 0, 6);
				changed = true;
			}

			// 編集があったときだけ再計算する。
			// 読み込んだ現在値を、表示だけで上書きしない。
			if (changed) {
				data.dirty = true;
				Recalculate(data);
			}

			ShowBase("Current Base", data.current.at("base"));
			ShowBase("Current Parameter", data.current.at("parameter"));

			if (ImGui::Button("Save Character Json")) {
				Save(data);
			}
		} catch (const std::exception& error) {
			status_ = error.what();
		}
	} else {
		ImGui::TextUnformatted("Character data could not be loaded.");

		if (ImGui::Button("Retry Load")) {
			Load(data);
		}
	}

	if (!status_.empty()) {
		ImGui::Separator();
		ImGui::TextWrapped("%s", status_.c_str());
	}

	ImGui::End();
#endif
}
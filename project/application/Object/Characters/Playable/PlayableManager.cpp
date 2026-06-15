#include "PlayableManager.h"
#include "Individual/Arte/Arte.h"
#include "Individual/Mei/Mei.h"
#include "Individual/Sizuku/Sizuku.h"
#include "Individual/Yuzuki/Yuzuki.h"

#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

namespace {
BaseParameter LoadBaseParameter(const nlohmann::json& baseJson) {
	BaseParameter base{};
	base.HP = baseJson.value("HP", base.HP);
	base.Attack = baseJson.value("Attack", base.Attack);
	base.Deffence = baseJson.value("Deffence", base.Deffence);
	return base;
}

void LoadParameterRates(const nlohmann::json& ratesJson, std::array<float, static_cast<std::size_t>(Attribute::MAXATTRIBUTE)>& rates) {
	if (!ratesJson.is_array()) {
		return;
	}

	const std::size_t count = std::min(rates.size(), ratesJson.size());
	for (std::size_t i = 0; i < count; ++i) {
		rates[i] = ratesJson[i].get<float>();
	}
}

Parameter LoadParameter(const nlohmann::json& parameterJson) {
	Parameter parameter{};
	parameter.level = parameterJson.value("Level", parameter.level);
	parameter.HP = parameterJson.value("HP", parameter.HP);
	parameter.Attack = parameterJson.value("Attack", parameter.Attack);
	parameter.Deffence = parameterJson.value("Deffence", parameter.Deffence);
	parameter.Speed = parameterJson.value("Speed", parameter.Speed);
	parameter.CriticalRate = parameterJson.value("CriticalRate", parameter.CriticalRate);
	parameter.CriticalDamage = parameterJson.value("CriticalDamage", parameter.CriticalDamage);
	LoadParameterRates(parameterJson.value("AttributeDamageRate", nlohmann::json::array()), parameter.AttributeDamageRate);
	LoadParameterRates(parameterJson.value("AttributeResistanceRate", nlohmann::json::array()), parameter.AttributeResistanceRate);
	return parameter;
}

bool LoadCurrentParameters(const std::string& characterName, BaseParameter& outBaseParameter, Parameter& outParameter) {
	const std::string filePath = "Resources/JSON/Character/" + characterName + "/current_parameters.json";
	std::ifstream ifs(filePath);
	if (!ifs.is_open()) {
		return false;
	}

	nlohmann::json root;
	ifs >> root;
	outBaseParameter = LoadBaseParameter(root.value("base", nlohmann::json::object()));
	outParameter = LoadParameter(root.value("parameter", nlohmann::json::object()));
	return true;
}
} // namespace

void PlayableManager::Initialize(const std::string& defaultPlayableName) {
	if (!ChangePlayable(defaultPlayableName)) {
		ChangePlayable("Sizuku");
	}
}

std::unique_ptr<PlayableBase> PlayableManager::CreatePlayable(const std::string& name) const {
	if (name == "Sizuku") {
		return std::make_unique<Sizuku>();
	}
	if (name == "Mei") {
		return std::make_unique<Mei>();
	}
	if (name == "Yuzuki") {
		return std::make_unique<Yuzuki>();
	}
	if (name == "Arte") {
		return std::make_unique<Arte>();
	}
	return nullptr;
}

bool PlayableManager::ChangePlayable(const std::string& name) {
	auto nextPlayable = CreatePlayable(name);
	if (!nextPlayable) {
		return false;
	}

	nextPlayable->Initialize();

	BaseParameter baseParameter{};
	Parameter parameter{};
	if (LoadCurrentParameters(name, baseParameter, parameter)) {
		nextPlayable->SetBaseParameter(baseParameter);
		nextPlayable->SetParameter(parameter);
	}

	currentPlayable_ = std::move(nextPlayable);
	return true;
}
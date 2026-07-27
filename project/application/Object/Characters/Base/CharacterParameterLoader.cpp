#include "CharacterParameterLoader.h"

#include <algorithm>
#include <array>
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
	parameter.AttributeAffinity = parameterJson.value("AttributeAffinity", parameter.AttributeAffinity);
	LoadParameterRates(parameterJson.value("AttributeDamageRate", nlohmann::json::array()), parameter.AttributeDamageRate);
	LoadParameterRates(parameterJson.value("AttributeResistanceRate", nlohmann::json::array()), parameter.AttributeResistanceRate);
	return parameter;
}

bool LoadJsonFile(const std::string& filePath, nlohmann::json& outRoot) {
	std::ifstream ifs(filePath);
	if (!ifs.is_open()) {
		return false;
	}

	ifs >> outRoot;
	return true;
}
} // namespace

namespace CharacterParameterLoader {

bool LoadCurrentParameters(const std::string& characterName, BaseParameter& outBaseParameter, Parameter& outParameter, int* outReinforcementAmount) {
	const std::string characterDirectory = "Resources/JSON/Character/" + characterName + "/";
	const std::array<std::string, 2> filePaths{
	    characterDirectory + "current_parameters.json",
	    characterDirectory + "current_parameter.json",
	};

	nlohmann::json root;
	for (const std::string& filePath : filePaths) {
		if (LoadJsonFile(filePath, root)) {
			outBaseParameter = LoadBaseParameter(root.value("base", nlohmann::json::object()));
			outParameter = LoadParameter(root.value("parameter", nlohmann::json::object()));
			if (outReinforcementAmount) {
				*outReinforcementAmount = std::max(0, root.value("reinforcementAmount", 0));
			}
			return true;
		}
	}

	return false;
}


} // namespace CharacterParameterLoader
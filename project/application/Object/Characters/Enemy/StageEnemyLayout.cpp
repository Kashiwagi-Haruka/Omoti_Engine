#include "StageEnemyLayout.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace {
using Json = nlohmann::json;

EnemyType ReadEnemyType(const std::string& name) {
	if (name == "Switch") {
		return EnemyType::Switch;
	}
	if (name == "BrainStealer") {
		return EnemyType::BrainStealer;
	}
	throw std::runtime_error("Unknown enemy type: " + name);
}

const char* EnemyTypeName(EnemyType type) {
	switch (type) {
	case EnemyType::Switch:
		return "Switch";
	case EnemyType::BrainStealer:
		return "BrainStealer";
	}
	throw std::runtime_error("Unknown enemy type.");
}

void ValidatePosition(const Vector3& position) {
	if (!std::isfinite(position.x) || !std::isfinite(position.y) || !std::isfinite(position.z)) {
		throw std::runtime_error("Position must contain finite numbers.");
	}
}

Vector3 ReadPosition(const Json& value) {
	if (!value.is_array() || value.size() != 3 || !value.at(0).is_number() || !value.at(1).is_number() || !value.at(2).is_number()) {
		throw std::runtime_error("Position must be [x, y, z].");
	}

	const Vector3 position{
	    value.at(0).get<float>(),
	    value.at(1).get<float>(),
	    value.at(2).get<float>(),
	};
	ValidatePosition(position);
	return position;
}
} // namespace

StageEnemyLayout LoadStageEnemyLayout(const char* path) {
	try {
		std::ifstream stream(path);
		if (!stream) {
			throw std::runtime_error("Cannot open file.");
		}

		const Json root = Json::parse(stream);
		const auto& version = root.at("schemaVersion");
		if (!version.is_number_integer() || version != 1) {
			throw std::runtime_error("Unsupported schemaVersion.");
		}

		const auto& phases = root.at("phases");
		StageEnemyLayout layout{};
		if (!phases.is_array() || phases.size() != layout.size()) {
			throw std::runtime_error("Expected five phase arrays.");
		}

		for (std::size_t phase = 0; phase < layout.size(); ++phase) {
			const auto& entries = phases.at(phase);
			if (!entries.is_array()) {
				throw std::runtime_error("Phase " + std::to_string(phase + 1) + " must be an array.");
			}

			auto& spawns = layout[phase];
			spawns.reserve(entries.size());

			for (std::size_t index = 0; index < entries.size(); ++index) {
				try {
					const auto& entry = entries.at(index);
					spawns.push_back({
					    ReadEnemyType(entry.at("type").get<std::string>()),
					    ReadPosition(entry.at("position")),
					});
				} catch (const std::exception& error) {
					throw std::runtime_error("Phase " + std::to_string(phase + 1) + ", enemy " + std::to_string(index + 1) + ": " + error.what());
				}
			}
		}

		return layout;
	} catch (const std::exception& error) {
		throw std::runtime_error(std::string(path) + ": " + error.what());
	}
}

void SaveStageEnemyLayout(const char* path, const StageEnemyLayout& layout) {
	Json phases = Json::array();

	for (const auto& phase : layout) {
		Json entries = Json::array();
		for (const auto& spawn : phase) {
			const Vector3& position = spawn.position;
			ValidatePosition(position);
			entries.push_back({
			    {"type",     EnemyTypeName(spawn.type)           },
			    {"position", {position.x, position.y, position.z}},
			});
		}
		phases.push_back(std::move(entries));
	}

	const Json root{
	    {"schemaVersion", 1                },
        {"phases",        std::move(phases)}
    };
	const std::filesystem::path destination(path);
	auto temporary = destination;
	temporary += ".tmp";

	if (destination.has_parent_path()) {
		std::filesystem::create_directories(destination.parent_path());
	}

	try {
		std::ofstream stream(temporary, std::ios::trunc);
		if (!stream) {
			throw std::runtime_error("Cannot open: " + temporary.string());
		}

		stream << root.dump(2) << '\n';
		stream.close();
		if (!stream) {
			throw std::runtime_error("Write failed: " + temporary.string());
		}

		std::filesystem::rename(temporary, destination);
	} catch (...) {
		std::error_code removeError;
		std::filesystem::remove(temporary, removeError);
		throw;
	}
}
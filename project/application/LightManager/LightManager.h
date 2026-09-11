#pragma once

#include "Light/CommonLight/DirectionalCommonLight.h"
#include "Light/CommonLight/PointCommonLight.h"
#include "Light/CommonLight/SpotCommonLight.h"

#include <array>
#include <cstdint>

class Player;

class LightManager {
public:
	void Initialize();
	void Update(Player& player);
	void DebugImGui();

private:
	void ApplyLights();

	DirectionalCommonLight directionalLight_{};
	std::array<PointCommonLight, kMaxPointLights> pointLights_{};
	uint32_t activePointLightCount_ = 0;
	std::array<SpotCommonLight, kMaxSpotLights> spotLights_{};
	uint32_t activeSpotLightCount_ = 0;
};
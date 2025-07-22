#pragma once

#include "config-utils/shared/config-utils.hpp"

#include "UnityEngine/Vector2.hpp"


DECLARE_CONFIG(Config) {

    // Defined in main.cpp
    static std::vector<UnityEngine::Vector2> RESOLUTIONS;

    CONFIG_VALUE(resolution, int, "resolution", 0);
    CONFIG_VALUE(customWidth, int, "customWidth", 1280);
    CONFIG_VALUE(customHeight, int, "customHeight", 720);
    CONFIG_VALUE(fieldOfView, float, "fieldOfView", 90);
    CONFIG_VALUE(reverseClick, bool, "reverseClick", false);
};

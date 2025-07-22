#pragma once

#include "config-utils/shared/config-utils.hpp"

DECLARE_CONFIG(Config) {
    CONFIG_VALUE(resolutionX, int, "resolutionX", 1280);
    CONFIG_VALUE(resolutionY, int, "resolutionY", 720);
    CONFIG_VALUE(fov, float, "fov", 100);
    CONFIG_VALUE(reverseClick, bool, "reverseClick", false);
};

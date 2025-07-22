#pragma once

#include "HMUI/ViewController.hpp"

namespace FPFC::Settings
{
    void DidActivate(HMUI::ViewController* self, bool firstActivation, bool screenSystemEnabling, bool addedToHierarchy);
}
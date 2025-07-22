#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "HMUI/UIKeyboard.hpp"

extern bool _isOpen;

DECLARE_CLASS_CODEGEN(FPFC, KeyboardKeyer, UnityEngine::MonoBehaviour) {

    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnGUI);
    DECLARE_INSTANCE_METHOD(void, OnDisable);

    DECLARE_INSTANCE_FIELD(HMUI::UIKeyboard*, _keyboard);
    DECLARE_INSTANCE_FIELD(bool, _caps);
};

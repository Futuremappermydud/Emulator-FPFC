#pragma once

#include "custom-types/shared/macros.hpp"

#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Quaternion.hpp"
#include "UnityEngine/AnimationCurve.hpp"

#include "GlobalNamespace/VRController.hpp"

DECLARE_CLASS_CODEGEN(FPFC, SimpleCameraController, UnityEngine::MonoBehaviour) {

    DECLARE_CTOR(ctor);
    DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, Update);

    DECLARE_INSTANCE_FIELD_PRIVATE(UnityEngine::AnimationCurve*, animationCurve);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::VRController*, leftController);
    DECLARE_INSTANCE_FIELD_PRIVATE(GlobalNamespace::VRController*, rightController);

    static UnityEngine::Vector3 GetInputTranslationDirection(UnityEngine::Quaternion rotation);
    static UnityEngine::Vector2 GetInputLookRotation();
};
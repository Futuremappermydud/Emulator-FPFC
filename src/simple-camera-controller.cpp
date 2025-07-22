#include "simple-camera-controller.hpp"
#include "operators.hpp"
#include "logger.hpp"

#include "UnityEngine/Input.hpp"
#include "UnityEngine/KeyCode.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Resources.hpp"

#include "GlobalNamespace/MenuPlayerController.hpp"
#include "GlobalNamespace/PauseMenuManager.hpp"

#include "VRUIControls/VRLaserPointer.hpp"
#include "VRUIControls/VRPointer.hpp"

using namespace UnityEngine;

DEFINE_TYPE(FPFC, SimpleCameraController);

static constexpr Vector3 DEFAULT_POSITION(0, 1.7f, 0);
static constexpr float MOUSE_SENSITIVITY_MULTIPLIER = 0.25f;
static constexpr float MOVEMENT_SENSITIVITY_MULTIPLIER = 3.0f;

void FPFC::SimpleCameraController::ctor()
{
    animationCurve = AnimationCurve::New_ctor();
    animationCurve->AddKey(0.75f, 0.75f);
    animationCurve->AddKey(0.75f, 0.75f);
}

void FPFC::SimpleCameraController::Awake()
{
    transform->position = DEFAULT_POSITION;

    // MenuPlayerController still exists in the game scene
    // So locate pause menu manager first and if it is not found then we are in the menu scene
    auto pauseMenuManager = Resources::FindObjectsOfTypeAll<GlobalNamespace::PauseMenuManager*>()->FirstOrDefault();
    if(pauseMenuManager)
    {
        leftController = pauseMenuManager->transform->Find("MenuControllers/ControllerLeft")->GetComponent<GlobalNamespace::VRController*>();
        rightController = pauseMenuManager->transform->Find("MenuControllers/ControllerRight")->GetComponent<GlobalNamespace::VRController*>();
    }
    else
    {
        auto menuPlayerController = Resources::FindObjectsOfTypeAll<GlobalNamespace::MenuPlayerController*>()->FirstOrDefault();
        if(!menuPlayerController)
        {
            Logger.error("Could not find controllers!");
            return;
        }
        leftController = menuPlayerController->leftController;
        rightController = menuPlayerController->rightController;
    }
    
    leftController->enabled = false;
    rightController->enabled = false;
    leftController->mouseMode = true;
    rightController->mouseMode = true;
}

void FPFC::SimpleCameraController::Update()
{
    if(Input::GetMouseButton(1))
    {
        Vector2 mouseMovement = GetInputLookRotation() * MOUSE_SENSITIVITY_MULTIPLIER;
        float sensitivityFactor = animationCurve->Evaluate(mouseMovement.magnitude);
        transform->localEulerAngles += Vector3(mouseMovement.x, mouseMovement.y, 0) * sensitivityFactor;
    }
    
    transform->localPosition += GetInputTranslationDirection(transform->rotation) * Time::get_deltaTime() * MOVEMENT_SENSITIVITY_MULTIPLIER;

    leftController->transform->SetPositionAndRotation(transform->position, transform->rotation);
    rightController->transform->SetPositionAndRotation(transform->position, transform->rotation);
}

Vector3 FPFC::SimpleCameraController::GetInputTranslationDirection(UnityEngine::Quaternion rotation)
{
    Vector3 direction(0, 0, 0);
    if(Input::GetKey(KeyCode::W))
    {
        direction += rotation * Vector3::getStaticF_forwardVector();
    }
    if(Input::GetKey(KeyCode::S))
    {
        direction += rotation * Vector3::getStaticF_backVector();
    }
    if(Input::GetKey(KeyCode::A))
    {
        direction += rotation * Vector3::getStaticF_leftVector();
    }
    if(Input::GetKey(KeyCode::D))
    {
        direction += rotation * Vector3::getStaticF_rightVector();
    }
    if(Input::GetKey(KeyCode::E))
    {
        direction += Vector3::getStaticF_upVector();
    }
    if(Input::GetKey(KeyCode::Q))
    {
        direction += Vector3::getStaticF_downVector();
    }

    return direction;
}

Vector2 FPFC::SimpleCameraController::GetInputLookRotation()
{
    // Flip Y axis
    return Vector2(-Input::GetAxis("MouseY"), Input::GetAxis("MouseX"));
}
#include "logger.hpp"
#include "_config.hpp"
#include "Config.hpp"
#include "simple-camera-controller.hpp"
#include "keyboard-keyer.hpp"

#include "scotland2/shared/modloader.h"

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Screen.hpp"
#include "UnityEngine/Input.hpp"
#include "UnityEngine/Camera.hpp"
#include "UnityEngine/Resolution.hpp"
#include "UnityEngine/EventSystems/PointerInputModule.hpp"

#include "BGLib/DotnetExtension/CommandLine/CommandLineParser.hpp"

#include "VRUIControls/VRInputModule.hpp"
#include "VRUIControls/MouseState.hpp"
#include "VRUIControls/ButtonState.hpp"

#include "GlobalNamespace/FirstPersonFlyingController.hpp"
#include "GlobalNamespace/LightManager.hpp"
#include "GlobalNamespace/IVRPlatformHelper.hpp"
#include "GlobalNamespace/GameplaySetupViewController.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"

#include "HMUI/UIKeyboard.hpp"

using namespace UnityEngine;

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

MAKE_HOOK_MATCH(AudioTimeSyncController_Awake, &GlobalNamespace::AudioTimeSyncController::Start, void, GlobalNamespace::AudioTimeSyncController* self)
{
  AudioTimeSyncController_Awake(self);
  if(!Camera::get_main()->gameObject->GetComponent<FPFC::SimpleCameraController*>())
  {
    Camera::get_main()->gameObject->AddComponent<FPFC::SimpleCameraController*>();
  }
}

MAKE_HOOK(SetResolution_Internal, nullptr, void, int width, int height, FullScreenMode fullScreenMode, RefreshRate* refreshRate)
{
  width = getConfig().resolutionX.GetValue();
  height = getConfig().resolutionY.GetValue();
  SetResolution_Internal(width, height, fullScreenMode, refreshRate);
}

MAKE_HOOK_MATCH(CommandLineParser_GetCommandLineArgs, &BGLib::DotnetExtension::CommandLine::CommandLineParser::GetCommandLineArgs, ArrayW<StringW>)
{
  auto args = CommandLineParser_GetCommandLineArgs();
  auto list = System::Collections::Generic::List_1<StringW>::New_ctor(reinterpret_cast<System::Collections::Generic::IEnumerable_1<StringW>*>(args.convert()));
  list->Add("fpfc");
  return list->ToArray();
}

MAKE_HOOK_MATCH(MouseState_GetButtonState, &VRUIControls::MouseState::GetButtonState, VRUIControls::ButtonState*, VRUIControls::MouseState* self, EventSystems::PointerEventData::InputButton button)
{
  auto state = MouseState_GetButtonState(self, button);
  // Makes right clicking not interact with menus
  if(Input::GetMouseButtonDown(1) && !getConfig().reverseClick.GetValue())
  {
    // 1 = not pressed ??
    state->_pressedValue = 1;
  }

  return state;
}

MAKE_HOOK_MATCH(VRController_get_triggerValue, &GlobalNamespace::VRController::get_triggerValue, float, GlobalNamespace::VRController* self)
{
    if (!self->_mouseMode)
    {
        return self->_vrPlatformHelper->GetTriggerValue(self->node);
    }
    if (!Input::GetMouseButton(getConfig().reverseClick.GetValue() ? 1 : 0))
    {
        return 0.0f;
    }
    return 1.0f;
}

MAKE_HOOK_MATCH(FirstPersonFlyingController_OnEnable, &GlobalNamespace::FirstPersonFlyingController::OnEnable, void, GlobalNamespace::FirstPersonFlyingController* self)
{
  if(!self->_camera->gameObject->GetComponent<FPFC::SimpleCameraController*>())
  {
    self->_camera->gameObject->AddComponent<FPFC::SimpleCameraController*>();
  }
}

MAKE_HOOK_MATCH(FirstPersonFlyingController_OnDisable, &GlobalNamespace::FirstPersonFlyingController::OnDisable, void, GlobalNamespace::FirstPersonFlyingController* self)
{}

MAKE_HOOK_MATCH(FirstPersonFlyingController_Update, &GlobalNamespace::FirstPersonFlyingController::Update, void, GlobalNamespace::FirstPersonFlyingController* self)
{}

MAKE_HOOK_MATCH(UIKeyboard_Awake, &HMUI::UIKeyboard::Awake, void, HMUI::UIKeyboard* self)
{
    if(!self->gameObject->GetComponent<FPFC::KeyboardKeyer*>())
    {
      self->gameObject->AddComponent<FPFC::KeyboardKeyer*>();
    }
}

MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();

  getConfig().Init(modInfo);

  Paper::Logger::RegisterFileContextId(Logger.tag);

  Logger.info("Completed setup!");
}

MOD_EXTERN_FUNC void late_load() noexcept {
  il2cpp_functions::Init();

  custom_types::Register::AutoRegister();

  Logger.info("Installing hooks...");

  INSTALL_HOOK(Logger, CommandLineParser_GetCommandLineArgs);
  INSTALL_HOOK(Logger, FirstPersonFlyingController_Update);
  INSTALL_HOOK(Logger, AudioTimeSyncController_Awake);
  INSTALL_HOOK(Logger, MouseState_GetButtonState);
  INSTALL_HOOK(Logger, VRController_get_triggerValue);
  INSTALL_HOOK(Logger, FirstPersonFlyingController_OnEnable);
  INSTALL_HOOK(Logger, FirstPersonFlyingController_OnDisable);
  INSTALL_HOOK(Logger, UIKeyboard_Awake);

  // Offset of UnityEngine_Screen_SetResolution_Internal
  constexpr uintptr_t methodOffset = 0x00673b70;
  INSTALL_HOOK_DIRECT(Logger, SetResolution_Internal, (void*)(baseAddr("libunity.so") + methodOffset));

  Logger.info("Installed all hooks!");
}

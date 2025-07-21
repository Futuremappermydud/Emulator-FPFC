#include "logger.hpp"
#include "_config.hpp"
#include "Config.hpp"

#include "scotland2/shared/modloader.h"

#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"

#include "System/Collections/Generic/List_1.hpp"
#include "System/Collections/Generic/IEnumerable_1.hpp"

#include "UnityEngine/Screen.hpp"
#include "UnityEngine/RefreshRate.hpp"
#include "UnityEngine/Input.hpp"
#include "UnityEngine/KeyCode.hpp"
#include "UnityEngine/Vector3.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Time.hpp"
#include "UnityEngine/Resolution.hpp"
#include "UnityEngine/EventSystems/PointerInputModule.hpp"

#include "BGLib/DotnetExtension/CommandLine/CommandLineParser.hpp"

#include "VRUIControls/VRInputModule.hpp"
#include "VRUIControls/MouseState.hpp"
#include "VRUIControls/ButtonState.hpp"

#include "GlobalNamespace/MouseLook.hpp"
#include "GlobalNamespace/HealthWarningViewController.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/FirstPersonFlyingController.hpp"
#include "GlobalNamespace/SelectRegionViewController.hpp"
#include "GlobalNamespace/MainFlowCoordinator.hpp"
#include "GlobalNamespace/LightManager.hpp"
#include "GlobalNamespace/AudioTimeSyncController.hpp"

using namespace UnityEngine;

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};

inline void SetResolution()
{
  UnityEngine::Screen::SetResolution(getConfig().resolutionX.GetValue(), getConfig().resolutionY.GetValue(), true);
}

MAKE_HOOK_MATCH(HealthWarningViewController_DidActivate, &GlobalNamespace::HealthWarningViewController::DidActivate, void, GlobalNamespace::HealthWarningViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
  HealthWarningViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
  SetResolution();
}

MAKE_HOOK_MATCH(MainMenuViewController_DidActivate, &GlobalNamespace::MainMenuViewController::DidActivate, void, GlobalNamespace::MainMenuViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
  MainMenuViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);
  SetResolution();
}

MAKE_HOOK_MATCH(SelectRegionViewController_add_didPressContinueButtonEvent, &GlobalNamespace::SelectRegionViewController::add_didPressContinueButtonEvent, void, GlobalNamespace::SelectRegionViewController* self, System::Action_1<::GlobalNamespace::SelectRegionViewController_Region>* value)
{
  SelectRegionViewController_add_didPressContinueButtonEvent(self, value);
  SetResolution();
}

MAKE_HOOK_MATCH(AudioTimeSyncController_Awake, &GlobalNamespace::AudioTimeSyncController::Start, void, GlobalNamespace::AudioTimeSyncController* self)
{
  AudioTimeSyncController_Awake(self);
  SetResolution();
}

MAKE_HOOK_MATCH(LightManager_OnCameraPreRender, &GlobalNamespace::LightManager::OnCameraPreRender, void, GlobalNamespace::LightManager* self, UnityEngine::Camera* camera)
{
  LightManager_OnCameraPreRender(self, camera);
  SetResolution();
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
  if(Input::GetMouseButtonDown(1))
  {
    state->_pressedValue = 1;
  }

  return state;
}

static bool outOfWindow = true;
MAKE_HOOK_MATCH(MouseLook_LookRotation, &GlobalNamespace::MouseLook::LookRotation, void, GlobalNamespace::MouseLook* self, Transform* character, Transform* camera)
{
  // Fixes camera jumping when entering cursor back into window
  if(Input::get_mousePosition().x == 0 || Input::get_mousePosition().x == Screen::get_currentResolution().height || Input::get_mousePosition().y == 0 || Input::get_mousePosition().y == Screen::get_currentResolution().height)
  {
    outOfWindow = true;
    return;
  }
  if(outOfWindow)
  {
    outOfWindow = false;
    return;
  }
  
  // Drag camera with right click
  if(!Input::GetMouseButton(1))
  {
    return;
  }

  MouseLook_LookRotation(self, character, camera);
}

MAKE_HOOK_MATCH(FirstPersonFlyingController_Update, &GlobalNamespace::FirstPersonFlyingController::Update, void, GlobalNamespace::FirstPersonFlyingController* self)
{
  FirstPersonFlyingController_Update(self);

  Vector3 position = self->_transform->position;
  Vector3 vector3 = Vector3::getStaticF_zeroVector();
  if(Input::GetKey(KeyCode::Z))
  {
    vector3 = self->_cameraTransform->up;
  }
  if(Input::GetKey(KeyCode::X))
  {
    vector3 = Vector3::op_UnaryNegation(self->_cameraTransform->up);
  }

  position = Vector3::op_Addition(
    position,
    Vector3::op_Multiply(vector3, self->_moveSensitivity * Time::get_deltaTime())
  );
  self->_transform->position = position;
}

MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();

  getConfig().Init(modInfo);

  Paper::Logger::RegisterFileContextId(Logger.tag);

  Logger.info("Completed setup!");
}

MOD_EXTERN_FUNC void late_load() noexcept {
  il2cpp_functions::Init();

  Logger.info("Installing hooks...");

  INSTALL_HOOK(Logger, CommandLineParser_GetCommandLineArgs);
  INSTALL_HOOK(Logger, MainMenuViewController_DidActivate);
  INSTALL_HOOK(Logger, HealthWarningViewController_DidActivate);
  INSTALL_HOOK(Logger, FirstPersonFlyingController_Update);
  INSTALL_HOOK(Logger, MouseLook_LookRotation);
  INSTALL_HOOK(Logger, SelectRegionViewController_add_didPressContinueButtonEvent);
  INSTALL_HOOK(Logger, AudioTimeSyncController_Awake);
  INSTALL_HOOK(Logger, MouseState_GetButtonState);
  
  Logger.info("Installed all hooks!");
}
#include "keyboard-keyer.hpp"

#include "UnityEngine/Event.hpp"
#include "UnityEngine/EventType.hpp"
#include "UnityEngine/KeyCode.hpp"

#include "System/Char.hpp"
#include "System/Action_1.hpp"
#include "System/Action.hpp"

using namespace UnityEngine;

bool _isOpen = false;

DEFINE_TYPE(FPFC, KeyboardKeyer);

void FPFC::KeyboardKeyer::ctor()
{
}

void FPFC::KeyboardKeyer::Awake()
{
    _keyboard = GetComponent<HMUI::UIKeyboard*>();
    _caps = false;
}

void FPFC::KeyboardKeyer::OnGUI()
{
    _isOpen = true;
    auto e = UnityEngine::Event::get_current();

    if (!e->isKey)
    {
        return;
    }

    bool caps = _caps;
    if (e->shift)
    {
        caps = !caps;
    }

    if (caps != _keyboard->_shouldCapitalize)
    {
        _keyboard->_shouldCapitalize = caps;
        _keyboard->SetKeyboardCapitalization(caps);
    }

    if (e->type != EventType::KeyDown)
    {
        return;
    }

    auto keyCode = e->keyCode;
    if (keyCode != KeyCode::None)
    {
        switch (keyCode)
        {
            case UnityEngine::KeyCode::Backspace:
                _keyboard->deleteButtonWasPressedEvent->Invoke();
                break;
            case KeyCode::Return:
                _keyboard->okButtonWasPressedEvent->Invoke();
                break;
            case KeyCode::CapsLock:
                _caps = !_caps;
                break;
            default:
                if (!System::Char::IsControl(e->character))
                {
                    _keyboard->keyWasPressedEvent->Invoke(e->character);
                }
                break;
        }
    }
}

void FPFC::KeyboardKeyer::OnDisable()
{
    _isOpen = false;
}

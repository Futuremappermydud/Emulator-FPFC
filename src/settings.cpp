#include "settings.hpp"
#include "Config.hpp"

#include "UnityEngine/Screen.hpp"
#include "UnityEngine/Camera.hpp"

#include "bsml/shared/BSML-Lite.hpp"

#include <vector>


void FPFC::Settings::DidActivate(HMUI::ViewController* self, bool firstActivation, bool screenSystemEnabling, bool addedToHierarchy)
{
    if(!firstActivation)
    {
        return;
    }

    auto increment = BSML::Lite::CreateIncrementSetting(self, "Resolution", 0, 1, getConfig().resolution.GetValue(), [](float fidx)
    {
        int idx = fidx;
        getConfig().resolution.SetValue(idx);
        // Values dont matter here since it is hooked and overriden
        UnityEngine::Screen::SetResolution(0, 0, true);
    });

    increment->formatter = [](float fidx) -> StringW
    {
        int idx = fidx;
        if(idx == 4)
        {
            return "Custom";
        }

        return fmt::format("{}x{}", Config_t::RESOLUTIONS[idx].x, Config_t::RESOLUTIONS[idx].y);
    };

    increment->text->text = increment->formatter(increment->currentValue);

    BSML::Lite::CreateSliderSetting(self, "Field Of View", 1, getConfig().fieldOfView.GetValue(), 70, 120, [](float value)
    {
        getConfig().fieldOfView.SetValue(value);
        UnityEngine::Camera::get_main()->fieldOfView = value;
    });
}
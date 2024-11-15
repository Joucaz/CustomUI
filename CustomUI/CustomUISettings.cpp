#include "pch.h"
#include "CustomUI.h"
using namespace std;

void CustomUI::RenderSettings() {
    ImGui::TextUnformatted("CustomUI");

    CVarWrapper boostBarCvar = cvarManager->getCvar("CustomUI_positionBoostBar");
    if (!boostBarCvar) { return; }
    //string boostBarCvarValue = boostBarCvar.getStringValue();
    
    const char* items[] = { "left", "right", "top", "bottom" };
    static int item_current = 0;


    /*for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
        if (boostBarCvarValue == items[i]) {
            item_current = i;
            break;
        }
    }*/

    //ImGui::Text("Current Boost Bar Position: %s", boostBarCvarValue.c_str());

    if (ImGui::Combo("Boost Bar Position", &item_current, items, IM_ARRAYSIZE(items))) {
        // Mise à jour du CVar avec la chaîne de caractères sélectionnée
        boostBarCvar.setValue(item_current);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the position of the progress bar for the boost");
    }

    
    /*if (ImGui::ArrowButton("##themes_left", ImGuiDir_Left) && rs_theme > 0)
        --rs_theme;
    ImGui::SameLine(0.f, 0.f);
    if (ImGui::ArrowButton("##themes_right", ImGuiDir_Right) && themes.size() && rs_theme < (themes.size() - 1))
        ++rs_theme;

    bool enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable plugin", &enabled)) {
        enableCvar.setValue(enabled);
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Toggle Cool Plugin");
    }

    CVarWrapper distanceCvar = cvarManager->getCvar("cool_distance");
    if (!distanceCvar) { return; }
    float distance = distanceCvar.getFloatValue();
    if (ImGui::SliderFloat("Distance", &distance, 0.0, 500.0)) {
        distanceCvar.setValue(distance);
    }
    if (ImGui::IsItemHovered()) {
        std::string hoverText = "distance is " + std::to_string(distance);
        ImGui::SetTooltip(hoverText.c_str());
    }*/
}
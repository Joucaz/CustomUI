#include "pch.h"
#include "CustomUI.h"
using namespace std;

void CustomUI::RenderSettings() {
    ImGui::TextUnformatted("CustomUI");

    CVarWrapper boostBarCvar = cvarManager->getCvar("CustomUI_positionBoostBar");
    if (!boostBarCvar) { return; }
    //string boostBarCvarValue = boostBarCvar.getStringValue();

    CVarWrapper presetChoosenCvar = cvarManager->getCvar("CustomUI_choosenPresets");
    if (!presetChoosenCvar) { return; }
    
    const char* itemsPositionBoost[] = { "left", "right", "top", "bottom" };
    static int currentPositionBoost = 0;

    if (ImGui::Combo("Boost Bar Position", &currentPositionBoost, itemsPositionBoost, IM_ARRAYSIZE(itemsPositionBoost))) {
        // Mise à jour du CVar avec la chaîne de caractères sélectionnée
        boostBarCvar.setValue(currentPositionBoost);
        writeCvar();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the position of the progress bar for the boost");
    }

    static vector<const char*> itemsPreset;
    static int currentPreset = 0;

    if (itemsPreset.empty()) {
        // Lire le CVar contenant le nom du preset sélectionné
        string savedPresetName = presetChoosenCvar.getStringValue();

        // Remplir le tableau des noms de presets
        itemsPreset.clear();
        int index = 0;
        for (const auto& pair : allPresets) {
            itemsPreset.push_back(pair.first.c_str());
            if (pair.first == savedPresetName) {
                currentPreset = index;  // Trouver l'indice correspondant au preset enregistré
            }
            index++;
        }
    }

    if (!itemsPreset.empty() && ImGui::Combo("Choose Preset", &currentPreset, itemsPreset.data(), itemsPreset.size())) {
        // Appliquer le preset sélectionné
        auto selectedPreset = allPresets.begin();
        std::advance(selectedPreset, currentPreset);  // Aller au preset correspondant

        //changePreset(selectedPreset->second, selectedPreset->first);
        presetChoosenCvar.setValue(selectedPreset->first);
        writeCvar();
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the preset to apply");
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
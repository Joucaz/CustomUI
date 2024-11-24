#include "pch.h"
#include "CustomUI.h"
using namespace std;

void CustomUI::RenderSettings() {
    ImGui::TextUnformatted("CustomUI");

    CVarWrapper boostFormCvar = cvarManager->getCvar("CustomUI_boostForm");
    if (!boostFormCvar) { return; }
    //string boostBarCvarValue = boostBarCvar.getStringValue();

    CVarWrapper presetChoosenCvar = cvarManager->getCvar("CustomUI_choosenPresets");
    if (!presetChoosenCvar) { return; }

    CVarWrapper itemsPositionSelected = cvarManager->getCvar("CustomUI_itemsNamePosition");
    if (!itemsPositionSelected) { return; }

    string keyPreset = getCvarString("CustomUI_choosenPresets");
    //
    //const char* itemsPositionBoost[] = { "left", "right", "top", "bottom" };
    //static int currentPositionBoost = 0;

    //if (ImGui::Combo("Boost Bar Position", &currentPositionBoost, itemsPositionBoost, IM_ARRAYSIZE(itemsPositionBoost))) {
    //    // Mise à jour du CVar avec la chaîne de caractères sélectionnée
    //    boostBarCvar.setValue(currentPositionBoost);
    //    writeCvar();
    //}
    //if (ImGui::IsItemHovered()) {
    //    ImGui::SetTooltip("Choose the position of the progress bar for the boost");
    //}

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

        setCvarString(presetChoosenCvar, selectedPreset->first);
        setCvarString(boostFormCvar, selectedPreset->second.boostForm);
    }

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Choose the preset to apply");
    }

    // Configuration des options en fonction du format du preset
    std::vector<const char*> itemsPositionCombo;
    std::vector<const char*> itemsPosition;

    if (allPresets[keyPreset].format == "default") {
        itemsPositionCombo = { "Boost All Items", "Score All Items" };
        itemsPosition = { "settingsBoostAllItems", "settingsScoreAllItems" };
    }
    else {
        itemsPositionCombo = {
            "Boost All Items",
            "Background Image Boost",
            "Texture Image Boost",
            "Text Boost",
            "Score All Items",
            "Background Image Score",
            "Text Score My Team",
            "Text Score Opposite Team",
            "Text Gametime"
        };
        itemsPosition = {
            "settingsBoostAllItems",
            "settingsBoostDisplay",
            "settingsBoostTexture",
            "settingsBoostText",
            "settingsScoreAllItems",
            "settingsScoreDisplay",
            "settingsScoreMyTeam",
            "settingsScoreOppositeTeam",
            "settingsGameTime"
        };
    }

    static int currentPosition = 0;

    // Conversion du vector en tableau pour ImGui
    std::vector<std::string> stringItems(itemsPositionCombo.begin(), itemsPositionCombo.end());
    std::vector<const char*> cStringItems;
    for (const auto& str : stringItems) {
        cStringItems.push_back(str.c_str());
    }

    if (ImGui::Combo("Choose Items to move and resize", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
        setCvarString(itemsPositionSelected, itemsPosition[currentPosition]);
        writeCvar();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Select which item you want to move or resize");
    }

    if (ImGui::Button("Edit position"))
    {
        showPositionEditor = true;   // Active l'éditeur de position
        showSizeEditor = false;
    }
    if (ImGui::Button("Edit size"))
    {
        showPositionEditor = false;   // Active l'éditeur de position
        showSizeEditor = true;
    }

    if (showPositionEditor) {
        showRenderEditPosition();
    }
    if (showSizeEditor) {
        showRenderEditSize();
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

void CustomUI::showRenderEditPosition() {
    string keyPreset = getCvarString("CustomUI_choosenPresets");
    string settingsItems = getCvarString("CustomUI_itemsNamePosition");
    static int slider_x = 0;
    static int slider_y = 0;
    if (ImGui::SliderInt("Position X", &slider_x, -screenSize.X, screenSize.X, "%d")) {
        changePositionX = slider_x;
    }
    if (ImGui::SliderInt("Position Y", &slider_y, -screenSize.X, screenSize.X, "%d")) {
        changePositionY = slider_y;
        /*updateJsonField(keyPreset, "settingsBoostDisplay", 0, slider_i);*/
    }
    if (ImGui::Button("Save Position"))
    {
        updateJsonField(keyPreset, settingsItems, 0, changePositionX);
        updateJsonField(keyPreset, settingsItems, 1, changePositionY);
        changePositionX = 0;
        changePositionY = 0;
        showPositionEditor = false;
    }
    if (ImGui::Button("Cancel")) {
        showPositionEditor = false;
        changePositionX = 0;
        changePositionY = 0;
    }
}

void CustomUI::showRenderEditSize() {

    static float slider_x = 1;
    static float slider_y = 1;

    float aspectRatio = static_cast<float>(screenSize.X) / static_cast<float>(screenSize.Y);
    bool isAspectRatio16_9 = fabs(aspectRatio - (16.0f / 9.0f)) < 0.01f;

    if (ImGui::SliderFloat("Size X", &slider_x, 0, 5, "%.2f")) {
        LOG("Size X modifiée !");
        changeSizeX = slider_x;
        if (isAspectRatio16_9) {
            changeSizeY = slider_x;
        }
        // updateJsonField(keyPreset, "settingsBoostDisplay", 0, slider_x);
    }

    if (!isAspectRatio16_9) {
        if (ImGui::SliderFloat("Size Y", &slider_y, -screenSize.Y, screenSize.Y, "%d")) {
            LOG("Size Y modifiée !");
            changeSizeY = slider_y;
            // updateJsonField(keyPreset, "settingsBoostDisplay", 1, slider_y);
        }
    }

    if (ImGui::Button("Save Position")) {
        showSizeEditor = false;
    }
    if (ImGui::Button("Cancel")) {
        showSizeEditor = false;
    }
}

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
        LOG(itemsPosition[currentPosition]);
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

    SettingsItems& settings = getSettings(allPresets[keyPreset], settingsItems);
    if (!changingBeginPosition) {
        changePositionX = settings.int1;
        changePositionY = settings.int2;
        changingBeginPosition = true;
    }
    static int slider_x = settings.int1;
    static int slider_y = settings.int2;
    if (ImGui::SliderInt("Position X", &slider_x, -screenSize.X, screenSize.X, "%d")) {
        changePositionX = slider_x;
    }
    if (ImGui::SliderInt("Position Y", &slider_y, -screenSize.X, screenSize.X, "%d")) {
        changePositionY = slider_y;
    }
    if (ImGui::Button("Save Position"))
    {
        updateJsonFieldInt(keyPreset, settingsItems, settings.int1, changePositionX);
        updateJsonFieldInt(keyPreset, settingsItems, settings.int2, changePositionY);
        changePositionX = 0;
        changePositionY = 0;
        showPositionEditor = false;
        showSizeEditor = false;
        changingBeginPosition = false;
    }
    if (ImGui::Button("Cancel")) {
        changePositionX = 0;
        changePositionY = 0;
        showPositionEditor = false;
        showSizeEditor = false;
        changingBeginPosition = false;
    }
}

void CustomUI::showRenderEditSize() {
    string keyPreset = getCvarString("CustomUI_choosenPresets");
    string settingsItems = getCvarString("CustomUI_itemsNamePosition");

    SettingsItems& settings = getSettings(allPresets[keyPreset], settingsItems);
    if (!changingBeginSize) {
        changeSizeX = settings.float1;
        changeSizeY = settings.float2;
        changingBeginSize = true;
    }
    static float slider_x = settings.float1;
    static float slider_y = settings.float2;

    float aspectRatio = static_cast<float>(screenSize.X) / static_cast<float>(screenSize.Y);
    bool isAspectRatio16_9 = fabs(aspectRatio - (16.0f / 9.0f)) < 0.01f;

    if (ImGui::SliderFloat("Size X", &slider_x, 0, 5, "%.2f")) {
        changeSizeX = slider_x;
        if (isAspectRatio16_9) {
            changeSizeY = slider_x;
        }
    }

    if (!isAspectRatio16_9) {
        if (ImGui::SliderFloat("Size Y", &slider_y, -screenSize.Y, screenSize.Y, "%d")) {
            changeSizeY = slider_y;
        }
    }

    if (ImGui::Button("Save Position")) {
        LOG("X : " + to_string(changeSizeX) + " Y : " + to_string(changeSizeY));
        updateJsonFieldFloat(keyPreset, settingsItems, settings.float1, changeSizeX);
        updateJsonFieldFloat(keyPreset, settingsItems, settings.float2, changeSizeY);
        changeSizeX = 0;
        changeSizeY = 0;
        showPositionEditor = false;
        showSizeEditor = false;
        changingBeginSize = false;
    }
    if (ImGui::Button("Cancel")) {

        changeSizeX = 0;
        changeSizeY = 0;
        showPositionEditor = false;
        showSizeEditor = false;
        changingBeginSize = false;
    }
}

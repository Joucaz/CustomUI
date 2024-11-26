#include "pch.h"
#include "CustomUI.h"
using namespace std;



void CustomUI::RenderMenu() {

	static bool no_titlebar = false;
	static bool no_scrollbar = false;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = false;
	static bool no_nav = false;
	static bool no_background = false;
	static bool no_bring_to_front = false;

	ImGuiWindowFlags window_flags = 0;
	if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//if (!ImGui::Begin(menuTitle_.c_str(), nullptr)) {
	if (!ImGui::Begin("TestMenu", &isSettingsOpen, window_flags)) {
		ImGui::End();
		return;
	}
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
	static int currentPosition = 0;


	if (!itemsPreset.empty() && ImGui::Combo("Choose Preset", &currentPreset, itemsPreset.data(), itemsPreset.size())) {
		// Appliquer le preset sélectionné
		auto selectedPreset = allPresets.begin();
		std::advance(selectedPreset, currentPreset);  // Aller au preset correspondant
		currentPosition = 0;

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
		itemsPositionCombo = { "", "Boost All Items", "Score All Items" };
		itemsPosition = { "", "settingsBoostAllItems", "settingsScoreAllItems" };
	}
	else {
		itemsPositionCombo = {
			"",
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
			"",
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

	// Conversion du vector en tableau pour ImGui
	std::vector<std::string> stringItems(itemsPositionCombo.begin(), itemsPositionCombo.end());
	std::vector<const char*> cStringItems;
	for (const auto& str : stringItems) {
		cStringItems.push_back(str.c_str());
	}

	if (ImGui::Combo("Choose Items to move and resize", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
		setCvarString(itemsPositionSelected, itemsPosition[currentPosition]);
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginPosition = false;
		LOG(itemsPosition[currentPosition]);
		writeCvar();
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Select which item you want to move or resize");
	}

	if (ImGui::Button("Edit position"))
	{
		if (currentPosition != 0) {
			showPositionEditor = true;
			showSizeEditor = false;
		}
		
	}
	if (ImGui::Button("Edit size"))
	{
		if (currentPosition != 0) {
			showPositionEditor = false;
			showSizeEditor = true;
		}
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

	ImGui::End();





}

void CustomUI::RenderSettings() {
    
    if (ImGui::Button("Open Menu"))
    {
        isSettingsOpen = true;
    }

    ImGui::Separator();

    ImGui::Text("Set A Bind To Open The Customizable Window");
    static char KeyBindInputBuf[200] = "F3";
    ImGui::InputText("##KeyBindInput", KeyBindInputBuf, IM_ARRAYSIZE(KeyBindInputBuf));

    ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("BindSet", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("To open the window, you can now press : %s", std::string(KeyBindInputBuf));
        ImGui::NewLine();

        if (ImGui::Button("OK", ImVec2(100.f, 25.f))) { 
			ImGui::CloseCurrentPopup(); 
		}
        ImGui::EndPopup();
    }


    if (ImGui::Button("Set Bind"))
    {
		//isSettingsOpen = true;
		gameWrapper->Execute([&](GameWrapper* gw) {
			cvarManager->setBind(KeyBindInputBuf, "CustomUI_openSettings");
			cvarManager->log("New key bind set : " + std::string(KeyBindInputBuf) + " -> CustomUI_openSettings");
			});

        ImGui::OpenPopup("BindSet");
    }




    ImGui::Separator();
    ImGui::Text("Plugin made by Vync");
    // Commence l'affichage de la fenêtre ImGui
    /*ImGuiStyle& style = ImGui::GetStyle();
    UserStyle = style;

    style.FrameRounding = 1.5f;
    style.FramePadding = ImVec2{ 2.0f, 5.0f };
    style.FrameBorderSize = 0.5f;

    style.Colors[ImGuiCol_Button] = ImVec4{ 0.0f, 193.0f / 255.0f, 173.0f / 255.0f, 1.0f };
    style.Colors[ImGuiCol_CheckMark] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 1.0f };

    style.Colors[ImGuiCol_Border] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.2f };
    style.Colors[ImGuiCol_BorderShadow] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.0f };

    style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 1.0f };
    style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 0.75f };
    style.Colors[ImGuiCol_Tab] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.2f };
    style.Colors[ImGuiCol_Header] = ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_FrameBg] = ImVec4{ 31.0f / 255.0f, 31.0f / 255.0f, 31.0f / 255.0f, 1.0f };

    style.TabRounding = 2.0f;
    style.FramePadding.x = 10.0f;*/
       //LOG("open : " + to_string(this->isWindowOpen_));
       //LOG("open : " + GetPluginName());
    
}

void CustomUI::showRenderEditPosition() {
    string keyPreset = getCvarString("CustomUI_choosenPresets");
    string settingsItems = getCvarString("CustomUI_itemsNamePosition");

    SettingsItems& settings = getSettings(allPresets[keyPreset], settingsItems);
    static int slider_x = 0;
    static int slider_y =0 ;
    if (!changingBeginPosition) {
        LOG("test beginPos");
        changePositionX = settings.int1;
        changePositionY = settings.int2;
        changingBeginPosition = true;
        slider_x = settings.int1;
        slider_y = settings.int2;
    }
    
    if (ImGui::SliderInt("Position X", &slider_x, -screenSize.X, screenSize.X, "%d")) {
        changePositionX = slider_x;
    }
    if (ImGui::SliderInt("Position Y", &slider_y, -screenSize.X, screenSize.X, "%d")) {
        changePositionY = slider_y;
    }
    if (ImGui::Button("Save Position"))
    {
        updateJsonFieldInt(keyPreset, settingsItems, "int1", changePositionX);
        updateJsonFieldInt(keyPreset, settingsItems, "int2", changePositionY);
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

    if (ImGui::Button("Save Size")) {
        LOG("X : " + to_string(changeSizeX) + " Y : " + to_string(changeSizeY));
        updateJsonFieldFloat(keyPreset, settingsItems, "float1", changeSizeX);
        updateJsonFieldFloat(keyPreset, settingsItems, "float2", changeSizeY);
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

#include "pch.h"
#include "CustomUI.h"
#include <cmath>

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
	if (!ImGui::Begin("CustomUI Menu", &isSettingsOpen, window_flags)) {
		ImGui::End();
		return;
	}

	CVarWrapper boostFormCvar = cvarManager->getCvar("CustomUI_boostForm");
	if (!boostFormCvar) { return; }
	//string boostBarCvarValue = boostBarCvar.getStringValue();

	CVarWrapper presetChoosenCvar = cvarManager->getCvar("CustomUI_choosenPresets");
	if (!presetChoosenCvar) { return; }

	CVarWrapper itemsPositionSelected = cvarManager->getCvar("CustomUI_itemsNamePosition");
	if (!itemsPositionSelected) { return; }

	string keyPreset = getCvarString("CustomUI_choosenPresets");

	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem("Preset"))
		{


			static vector<const char*> itemsPreset;
			static int currentChoosenPreset = 0;

			if (itemsPreset.empty()) {
				// Lire le CVar contenant le nom du preset sélectionné
				string savedPresetName = presetChoosenCvar.getStringValue();

				// Remplir le tableau des noms de presets
				itemsPreset.clear();
				int index = 0;
				for (const auto& pair : allPresets) {
					itemsPreset.push_back(pair.first.c_str());
					if (pair.first == savedPresetName) {
						currentChoosenPreset = index;  // Trouver l'indice correspondant au preset enregistré
					}
					index++;
				}
			}
			static int currentPosition = 0;

			ImGui::Text("ChoosePreset");
			ImGui::SetNextItemWidth(200.0f);
			if (!itemsPreset.empty() && ImGui::Combo("##ChoosePreset", &currentChoosenPreset, itemsPreset.data(), itemsPreset.size())) {
				// Appliquer le preset sélectionné
				auto selectedPreset = allPresets.begin();
				std::advance(selectedPreset, currentChoosenPreset);  // Aller au preset correspondant
				currentPosition = 0;

				setCvarString(presetChoosenCvar, selectedPreset->first);
				setCvarString(boostFormCvar, selectedPreset->second.boostForm);
				currentPreset = loadCurrentPreset(selectedPreset->first);
				loadThemeFont();
				appendFont();
			}

			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Choose the preset to apply");
			}

			// Configuration des options en fonction du format du preset
			std::vector<const char*> itemsPositionCombo;
			std::vector<const char*> itemsPosition;

			if (!isArtistMode) {
				itemsPositionCombo = { "", "Boost All Items", "Score All Items" };
				itemsPosition = { "", "settingsBoostAllItems", "settingsScoreAllItems" };
			}
			else {
				itemsPositionCombo = {
					"",
					"Background Image Boost",
					"Texture Image Boost",
					"Text Boost",
					"Background Image Score",
					"Text Score My Team",
					"Text Score Opposite Team",
					"Text Gametime"
				};
				itemsPosition = {
					"",
					"settingsBoostDisplay",
					"settingsBoostTexture",
					"settingsBoostText",
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

			ImGui::Text("Choose Items to move and resize");
			ImGui::SetNextItemWidth(200.0f);
			if (ImGui::Combo("##ItemsToModify", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
				setCvarString(itemsPositionSelected, itemsPosition[currentPosition]);
				showPositionEditor = false;
				showSizeEditor = false;
				changingBeginPosition = false;
				writeCvar();
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Select which item you want to move or resize");
			}

			static bool showError = false; // Variable pour suivre l'état d'erreur
			static float errorTimer = 0.0f;

			if (ImGui::Button("Edit position"))
			{
				if (currentPosition != 0) {
					showPositionEditor = true;
					showSizeEditor = false;
					showError = false;
				}
				else {
					showError = true;
					errorTimer = 2.0f;
				}

			}
			ImGui::SameLine();
			if (ImGui::Button("Edit size"))
			{
				if (currentPosition != 0) {
					showPositionEditor = false;
					showSizeEditor = true;
					showError = false;
				}
				else {
					showError = true;
					errorTimer = 2.0f;
				}
			}
			if (showError) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Select an item before editing on the droplist");
				errorTimer -= ImGui::GetIO().DeltaTime;

				if (errorTimer <= 0.0f) {
					showError = false;
				}
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Couleur rouge
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.0f, 0.0f, 1.0f)); // Rouge légèrement plus sombre au survol
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f)); // Rouge encore plus sombre en clic

			if (ImGui::Button("Reset the preset")) {
				std::vector<const char*> itemsPositionReset;
				if (!isArtistMode) {
					itemsPositionReset = { "settingsBoostAllItems", "settingsScoreAllItems" };
				}
				else {
					itemsPositionReset = { "settingsBoostDisplay",
					"settingsBoostTexture",
					"settingsBoostText",
					"settingsScoreDisplay",
					"settingsScoreMyTeam",
					"settingsScoreOppositeTeam",
					"settingsGameTime" };
				}
				
				for (size_t i = 0; i < itemsPositionReset.size(); ++i) {
					updateJsonFieldFloat(keyPreset, itemsPositionReset[i], "positionX", 0.5f);
					updateJsonFieldFloat(keyPreset, itemsPositionReset[i], "positionY", 0.5f);
					updateJsonFieldFloat(keyPreset, itemsPositionReset[i], "sizeX", 1.0f);
					updateJsonFieldFloat(keyPreset, itemsPositionReset[i], "sizeY", 1.0f);
					changePositionX = 0.5f;
					changePositionX = 0.5f;
					changeSizeX = 1.0f;
					changeSizeY = 1.0f;
				}
			}

			ImGui::PopStyleColor(3);



			if (showPositionEditor) {
				showRenderEditPosition();
			}
			if (showSizeEditor) {
				showRenderEditSize();
			}


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Customization"))
		{
			ImGui::Checkbox("Artist Mode", &isArtistMode);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f)); // Rouge vif
			ImGui::TextWrapped("Warning: When Artist Mode is active, all items are editable, and presets can be broken. Proceed with caution!");
			ImGui::PopStyleColor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Others"))
		{
			ImGui::Text("Made by @JoucazJC");

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
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

float CustomUI::intToFloatPosition(int position, int screenSize) {
	// Calcul de la position flottante
	float result = static_cast<float>(position + screenSize) / (2.0f * screenSize);

	// Arrondi à 2 décimales
	result = std::round(result * 1000.0f) / 1000.0f;

	return result;
}
int CustomUI::floatToIntPosition(float position, int screenSize) {
	return static_cast<int>(std::round(2.0f * position * screenSize - screenSize));
}

void CustomUI::showRenderEditPosition() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");


	static float sliderX = 0.5f;
	static float sliderY = 0.5f;

	SettingsItems& settings = getSettings(currentPreset, settingsItems);

	if (!changingBeginPosition) {
		LOG("test beginPos");
		sliderX = settings.positionX;
		sliderY = settings.positionY;
		changePositionX = floatToIntPosition(settings.positionX, screenSize.X);
		changePositionY = floatToIntPosition(settings.positionY, screenSize.Y);
		changingBeginPosition = true;
	}


	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderFloat("Position X", &sliderX, 0.0f, 1.0f, "%.3f"))
	{
		changePositionX = floatToIntPosition(sliderX, screenSize.X);

	}
	ImGui::SameLine();
	if (ImGui::Button("-##PositionX"))
	{
		changePositionX -= 1;
		sliderX = intToFloatPosition(changePositionX, screenSize.X);
	}
	ImGui::SameLine();
	if (ImGui::Button("+##PositionX"))
	{
		changePositionX += 1;
		sliderX = intToFloatPosition(changePositionX, screenSize.X);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Position X"))
	{
		changePositionX = 0;
		sliderX = 0.5f;
	}

	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderFloat("Position Y", &sliderY, 0.0f, 1.0f))
	{
		changePositionY = floatToIntPosition(sliderY, screenSize.Y);
	}
	ImGui::SameLine();
	if (ImGui::Button("-##PositionY"))
	{
		changePositionY -= 1;
		sliderY = intToFloatPosition(changePositionY, screenSize.Y);
	}
	ImGui::SameLine();
	if (ImGui::Button("+##PositionY"))
	{
		changePositionY += 1;
		sliderY = intToFloatPosition(changePositionY, screenSize.Y);
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Position Y"))
	{
		sliderY = 0.5f;
		changePositionY = 0;
	}

	if (ImGui::Button("Save Position"))
	{
		updateJsonFieldFloat(keyPreset, settingsItems, "positionX", sliderX);
		updateJsonFieldFloat(keyPreset, settingsItems, "positionY", sliderY);
		changePositionX = 0;
		changePositionY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginPosition = false;
	}
	ImGui::SameLine();
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

	static float slider_x;
	static float slider_y;

	SettingsItems& settings = getSettings(currentPreset, settingsItems);
	if (!changingBeginSize) {
		changeSizeX = settings.sizeX;
		changeSizeY = settings.sizeY;
		slider_x = settings.sizeX;
		slider_y = settings.sizeY;
		changingBeginSize = true;
	}

	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderFloat("Size X", &slider_x, 0, 5, "%.2f")) {
		changeSizeX = slider_x;
		changeSizeY = slider_x;
	}
	ImGui::SameLine();
	if (ImGui::Button("-##Size"))
	{
		changeSizeX -= .01f;
		changeSizeY -= .01f;
		slider_x -= .01f;
	}
	ImGui::SameLine();
	if (ImGui::Button("+##Size"))
	{
		changeSizeX += .01f;
		changeSizeY += .01f;
		slider_x += .01f;
	}
	
	ImGui::SameLine();
	if (ImGui::Button("Reset Scale"))
	{
		changeSizeX = 1.0f;
		changeSizeY = 1.0f;
		slider_x = 1.0f;
		slider_y = 1.0f;
	}

	if (ImGui::Button("Save Size")) {

		updateJsonFieldFloat(keyPreset, settingsItems, "sizeX", changeSizeX);
		updateJsonFieldFloat(keyPreset, settingsItems, "sizeY", changeSizeY);
		changeSizeX = 1;
		changeSizeY = 1;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeSizeX = 1;
		changeSizeY = 1;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
}

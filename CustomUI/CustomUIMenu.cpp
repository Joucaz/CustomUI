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

	ImGui::Text("ChoosePreset");
	ImGui::SetNextItemWidth(200.0f);
	if (!itemsPreset.empty() && ImGui::Combo("##ChoosePreset", &currentPreset, itemsPreset.data(), itemsPreset.size())) {
		// Appliquer le preset sélectionné
		auto selectedPreset = allPresets.begin();
		std::advance(selectedPreset, currentPreset);  // Aller au preset correspondant
		currentPosition = 0;

		setCvarString(presetChoosenCvar, selectedPreset->first);
		setCvarString(boostFormCvar, selectedPreset->second.boostForm);
		loadThemeFont();
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

	ImGui::Text("Choose Items to move and resize");
	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::Combo("##ItemsToModify", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
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
		for (size_t i = 1; i < itemsPosition.size(); ++i) {
			updateJsonFieldInt(keyPreset, itemsPosition[i], "int1", 0);
			updateJsonFieldInt(keyPreset, itemsPosition[i], "int2", 0);
			updateJsonFieldFloat(keyPreset, itemsPosition[i], "float1", 1.0f);
			updateJsonFieldFloat(keyPreset, itemsPosition[i], "float2", 1.0f);
		}
	}

	ImGui::PopStyleColor(3);



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


void CustomUI::showRenderEditPosition() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	SettingsItems& settings = getSettings(allPresets[keyPreset], settingsItems);
	static int slider_x = 0;
	static int slider_y = 0;
	if (!changingBeginPosition) {
		LOG("test beginPos");
		changePositionX = settings.int1;
		changePositionY = settings.int2;
		changingBeginPosition = true;
		slider_x = settings.int1;
		slider_y = settings.int2;
	}

	//float step = ImGui::IsKeyDown(ImGuiKey_LeftArrow) || ImGui::IsKeyDown(ImGuiKey_LeftArrow) ? 0.1f : 1.0f;
	ImGui::Text("Position X");
	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderInt("", &slider_x, -screenSize.X, screenSize.X, "%d")) {
		changePositionX = slider_x;
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		changePositionX -= 1;
		slider_x -= 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		changePositionX += 1;
		slider_x += 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Position X"))
	{
		updateJsonFieldInt(keyPreset, settingsItems, "int1", 0);
		changePositionX = 0;
		changePositionY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginPosition = false;
	}
	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderInt("Position Y", &slider_y, -screenSize.X, screenSize.X, "%d")) {
		changePositionY = slider_y;
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		changePositionY -= 1;
		slider_y -= 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		changePositionY += 1;
		slider_y += 1;
	}
	ImGui::SameLine();
	if (ImGui::Button("Reset Position Y"))
	{
		updateJsonFieldInt(keyPreset, settingsItems, "int2", 0);
		changePositionX = 0;
		changePositionY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginPosition = false;
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

	SettingsItems& settings = getSettings(allPresets[keyPreset], settingsItems);
	if (!changingBeginSize) {
		changeSizeX = settings.float1;
		changeSizeY = settings.float2;
		changingBeginSize = true;
	}
	static float slider_x = settings.float1;
	static float slider_y = settings.float2;

	/*float aspectRatio = static_cast<float>(screenSize.X) / static_cast<float>(screenSize.Y);
	bool isAspectRatio16_9 = fabs(aspectRatio - (16.0f / 9.0f)) < 0.01f;*/

	//static bool isCheckedAspectRatio= false; // Variable pour suivre l'état de la case

	//ImGui::Checkbox("Unlink X & Y size", &isCheckedAspectRatio);

	ImGui::SetNextItemWidth(200.0f);
	if (ImGui::SliderFloat("Size X", &slider_x, 0, 5, "%.2f")) {
		changeSizeX = slider_x;
		//if (!isCheckedAspectRatio) {
			changeSizeY = slider_x;
		//}
	}
	ImGui::SameLine();
	if (ImGui::Button("-"))
	{
		changeSizeX -= .01f;
		//if (!isCheckedAspectRatio) {
			changeSizeY -= .01f;
		//}
		slider_x -= .01f;
	}
	ImGui::SameLine();
	if (ImGui::Button("+"))
	{
		changeSizeX += .01f;
		//if (!isCheckedAspectRatio) {
			changeSizeY += .01f;
		//}
		slider_x += .01f;
	}
	//std::string buttonTextSizeX = isCheckedAspectRatio ? "Reset Scale X" : "Reset Scale";
	ImGui::SameLine();
	if (ImGui::Button("Reset Scale"))
	{
		//if (!isCheckedAspectRatio) {
			updateJsonFieldInt(keyPreset, settingsItems, "float2", 0);
		//}
		updateJsonFieldInt(keyPreset, settingsItems, "float1", 0);
		changeSizeX = 0;
		changeSizeY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}

	//if (isCheckedAspectRatio) {
	//	ImGui::SetNextItemWidth(200.0f);
	//	if (ImGui::SliderFloat("Size Y", &slider_y, 0, 5, "%.2f")) {
	//		changeSizeY = slider_y;
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button("-"))
	//	{
	//		changeSizeY -= 1;
	//		slider_y -= 1;
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button("+"))
	//	{
	//		changeSizeY += 1;
	//		slider_y += 1;
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button("Reset Scale Y"))
	//	{
	//		updateJsonFieldInt(keyPreset, settingsItems, "float2", 0);
	//		changeSizeY = 0;
	//		showPositionEditor = false;
	//		showSizeEditor = false;
	//		changingBeginSize = false;
	//	}
	//}

	if (ImGui::Button("Save Size")) {

		updateJsonFieldFloat(keyPreset, settingsItems, "float1", changeSizeX);
		updateJsonFieldFloat(keyPreset, settingsItems, "float2", changeSizeY);
		changeSizeX = 0;
		changeSizeY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeSizeX = 0;
		changeSizeY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
}

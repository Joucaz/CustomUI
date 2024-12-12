#include "pch.h"
#include "CustomUI.h"
#include <cmath>

using namespace std;


void CustomUI::RenderMenu() {


	ImGuiStyle& style = ImGui::GetStyle();
	UserStyle = style;

	style.WindowBorderSize = 0.0f;
	style.WindowRounding = 15;
	style.FramePadding = ImVec2(7, 7);
	style.FrameRounding = 5;
	style.TabRounding = 2.0f;
	style.ItemSpacing = ImVec2(5.0f, 10.0f); //spacing between object
	style.ItemInnerSpacing = ImVec2(5, 5);
	style.Alpha = 1.0f;

	style.Colors[ImGuiCol_Button] = baseYellow;
	style.Colors[ImGuiCol_ButtonHovered] = baseYellowHovered;
	style.Colors[ImGuiCol_ButtonActive] = baseYellowActive;
	style.Colors[ImGuiCol_CheckMark] = baseYellow;

	style.Colors[ImGuiCol_TabActive] = baseYellow;
	style.Colors[ImGuiCol_TabHovered] = baseYellowHovered;
	style.Colors[ImGuiCol_Tab] = baseGrey;

	style.Colors[ImGuiCol_ResizeGrip] = baseGrey;
	style.Colors[ImGuiCol_ResizeGripActive] = baseYellow;
	style.Colors[ImGuiCol_ResizeGripHovered] = baseYellowHovered;

	style.Colors[ImGuiCol_FrameBg] = baseGrey;
	style.Colors[ImGuiCol_FrameBgActive] = baseGrey;
	style.Colors[ImGuiCol_FrameBgHovered] = baseYellowHovered;

	style.Colors[ImGuiCol_Header] = baseGrey;
	style.Colors[ImGuiCol_HeaderActive] = baseGrey;
	style.Colors[ImGuiCol_HeaderHovered] = baseYellowHovered;

	style.Colors[ImGuiCol_SliderGrab] = baseYellow;
	style.Colors[ImGuiCol_SliderGrabActive] = baseYellowHovered;

	style.Colors[ImGuiCol_TextSelectedBg] = baseYellowHovered;

	style.Colors[ImGuiCol_WindowBg] = baseBlack;

	ImVec2 minSize(650.0f, 650.0f);
	ImVec2 maxSize(1920, 1080);

	// Appliquer les contraintes avant de créer la fenêtre
	ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

	static bool no_titlebar = true;
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
	CVarWrapper itemsPositionSelected2 = cvarManager->getCvar("CustomUI_itemsNamePosition2");
	if (!itemsPositionSelected2) { return; }
	CVarWrapper itemsPositionSelected3 = cvarManager->getCvar("CustomUI_itemsNamePosition3");
	if (!itemsPositionSelected3) { return; }
	CVarWrapper itemsPositionSelected4 = cvarManager->getCvar("CustomUI_itemsNamePosition4");
	if (!itemsPositionSelected4) { return; }

	string keyPreset = getCvarString("CustomUI_choosenPresets");

	int maxComboCount;
	if (isArtistMode) {
		maxComboCount = 3;
	}
	else {
		maxComboCount = 1;
	}
	
	static int currentPosition = 0;

	if (!basicFontMenu) {
		initFonts();
	}
	/*if (!basicFontTitle) {
		initFonts();
	}*/

	if (basicFontMenu) {
		ImGui::PushFont(basicFontMenu);
	}


	if (auto renderImageLogoText = imageLogoText->GetImGuiTex()) {
		auto size = imageLogoText->GetSizeF();

		ImGui::Image(
			renderImageLogoText,
			ImVec2(size.X, size.Y),
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
		);
	}
	ImGui::SameLine();

	// Calculer l'espace disponible
	float availableWidth = ImGui::GetContentRegionAvail().x;

	if (auto renderImageDiscord = discordLogo->GetImGuiTex()) {
		auto sizeDiscord = discordLogo->GetSizeF();
		ImVec2 discordImageSize(sizeDiscord.X, sizeDiscord.Y);

		// Calculer le décalage horizontal pour aligner l'image à droite
		float offsetX = availableWidth - discordImageSize.x;
		if (offsetX > 0) {
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
		}

		// Dessiner l'image Discord
		ImGui::Image(
			renderImageDiscord,
			discordImageSize,
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
		);

		// Ajouter les interactions pour l'image Discord
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::BeginTooltip();
			ImGui::Text("Join the discord to show all the available features");
			ImGui::EndTooltip();
		}
		if (ImGui::IsItemClicked()) {
			system("Start https://discord.gg/NQ8Qw4Mw2w");
		}
	}


	

	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem("Preset"))
		{
			ImGui::Spacing();
			ImGui::Indent(20.0f);
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

			ImGui::Text("ChoosePreset");
			ImGui::SetNextItemWidth(400.0f);
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
			std::vector<std::string> settingsItemsList = {
				getCvarString("CustomUI_itemsNamePosition"),
				getCvarString("CustomUI_itemsNamePosition2"),
				getCvarString("CustomUI_itemsNamePosition3"),
				getCvarString("CustomUI_itemsNamePosition4")
			};

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);

			if (ImGui::Button("Reset the preset")) {
				for (size_t i = 1; i < itemsPosition.size(); ++i) {
					updateJsonFieldFloat(keyPreset, itemsPosition[i], "positionX", 0.5f);
					updateJsonFieldFloat(keyPreset, itemsPosition[i], "positionY", 0.5f);
					updateJsonFieldFloat(keyPreset, itemsPosition[i], "sizeX", 1.0f);
					updateJsonFieldFloat(keyPreset, itemsPosition[i], "sizeY", 1.0f);
					changePositionX = 0.5f;
					changePositionX = 0.5f;
					changeSizeX = 1.0f;
					changeSizeY = 1.0f;
					changeColorR = 255;
					changeColorG = 255;
					changeColorB = 255;
					changeColorA = 255;
					for (const auto& settingsItem : settingsItemsList) {
						try {
							string colorSettingsJson = getStringSettingsColor(settingsItem);
							array<int, 4> colorValues = { changeColorR, changeColorG, changeColorB, changeColorA };
							LOG("NON Erreur lors du traitement de '" + settingsItem);
							updateJsonColor(keyPreset, colorSettingsJson, colorValues);
						}
						catch (const std::exception& e) {
							LOG("Erreur lors du traitement de '" + settingsItem + "': " + std::string(e.what()));
						}
					}
					changeColorR = 0;
					changeColorG = 0;
					changeColorB = 0;
					changeColorA = 0;
				}
			}

			ImGui::PopStyleColor(3);

			// Conversion du vector en tableau pour ImGui
			std::vector<std::string> stringItems(itemsPositionCombo.begin(), itemsPositionCombo.end());
			std::vector<const char*> cStringItems;
			for (const auto& str : stringItems) {
				cStringItems.push_back(str.c_str());
			}

			ImGui::Text("Choose Items to move and resize");
			ImGui::SetNextItemWidth(400.0f);
			if (ImGui::Combo("##ItemsToModify", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
				changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
				setCvarString(itemsPositionSelected, itemsPosition[currentPosition]);
				showPositionEditor = false;
				showSizeEditor = false;
				changingBeginPosition = false;
				writeCvar();
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Select which item you want to move or resize");
			}

			ImGui::SameLine();
			if (ImGui::Button(" - ##RemoveItem")) {
				if (!itemsAdded.empty()) {
					itemsAdded.pop_back();
					intComboSelections.pop_back();
					changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(" + ##AddItem")) {
				if (intComboSelections.size() < maxComboCount) {
					itemsAdded.push_back("Nouvel élément " + std::to_string(itemsAdded.size() + 1));
					intComboSelections.push_back(0);
					changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
				}
				
			}
			if (!itemsAdded.empty()) {
				for (size_t i = 0; i < itemsAdded.size(); ++i) {

					ImGui::SetNextItemWidth(400.0f);
					if (ImGui::Combo(("##Combo_" + std::to_string(i)).c_str(),
						&intComboSelections[i], itemsPositionCombo.data(),
						itemsPositionCombo.size())) 
					{
						changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
						//ImGui::Text("Option sélectionnée : %s", itemsPositionCombo[intComboSelections[i]]);
					}
				}
			}

			static bool showError = false;
			static float errorTimer = 0.0f;
			static bool showErrorColor = false;
			static float errorTimerColor = 0.0f;

			if (ImGui::Button("Edit position"))
			{
				if (currentPosition != 0) {
					showPositionEditor = true;
					showSizeEditor = false;
					showColorEditor = false;
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
					showColorEditor = false;
					showError = false;
				}
				else {
					showError = true;
					errorTimer = 2.0f;
				}
			}
			if (isArtistMode) 
			{
				ImGui::SameLine();
				if (ImGui::Button("Edit color"))
				{
					if (currentPosition != 0 && cvarIsText(itemsPositionSelected) && cvarIsText(itemsPositionSelected2) && cvarIsText(itemsPositionSelected3) && cvarIsText(itemsPositionSelected4)) {
						showColorEditor = true;
						showPositionEditor = false;
						showSizeEditor = false;
						showErrorColor = false;
					}
					else {
						showErrorColor = true;
						errorTimerColor = 2.0f;
					}
				}
			}

			if (showError) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Select an item before editing on the droplist");
				errorTimer -= ImGui::GetIO().DeltaTime;

				if (errorTimer <= 0.0f) {
					showError = false;
				}
			}
			if (showErrorColor) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Select only text items to modify their color");
				errorTimerColor -= ImGui::GetIO().DeltaTime;

				if (errorTimerColor <= 0.0f) {
					showErrorColor = false;
				}
			}


			if (showPositionEditor) {
				showRenderEditPosition();
			}
			if (showSizeEditor) {
				showRenderEditSize();
			}
			if (showColorEditor) {
				showRenderEditColor();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Artist"))
		{
			ImGui::Spacing();
			ImGui::Indent(20.0f);

			if (ImGui::Checkbox("Artist Mode", &isArtistMode)) {
				currentPosition = 0;
				setCvarString(itemsPositionSelected, "");
				setCvarString(itemsPositionSelected2, "");
				setCvarString(itemsPositionSelected3, "");
				setCvarString(itemsPositionSelected4, "");
				intComboSelections.clear();
				itemsAdded.clear();
			};
			ImGui::PushStyleColor(ImGuiCol_Text, redCaution); // Rouge vif
			ImGui::TextWrapped("Warning: When Artist Mode is active, all items are editable, and presets can be broken. Proceed with caution!");
			ImGui::PopStyleColor();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Others"))
		{
			ImGui::Spacing();
			ImGui::Indent(20.0f);
			ImGui::Text("Creator & Developer :");
			ImGui::Spacing();
			ImGui::Indent(15.0f);
			ImGui::Text("Joucaz");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.5f, size.Y/1.5f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Joucaz Portfolio/Website");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://joudcazeaux.fr");
			}
			ImGui::SameLine();
			if (auto renderImageLogoText = githubLogo->GetImGuiTex()) {
				auto size = githubLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.5f, size.Y/1.5f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Joucaaz");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://github.com/Joucaaz");
			}
			ImGui::SameLine();
			if (auto renderImageLogoText = wesbiteLogo->GetImGuiTex()) {
				auto size = wesbiteLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.5f, size.Y/1.5f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@JoucazJC");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://twitter.com/JoucazJC");
			}

			ImGui::Indent(-15.0f);
			ImGui::Text("Preset Artist :");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Indent(15.0f);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	if (basicFontMenu) {
		ImGui::PopFont();
	}
	style = UserStyle;


	ImGui::End();

}

void CustomUI::changeCvarCombo(CVarWrapper cvar2, CVarWrapper cvar3, CVarWrapper cvar4, vector<const char*> itemsPosition) {

	// PAS DU TOUT OPTIMISE A CHANGER
	if (intComboSelections.size() > 0) {
		setCvarString(cvar2, itemsPosition[intComboSelections[0]]);
		setCvarString(cvar3, "");
		setCvarString(cvar4, "");
		writeCvar();
	}
	if (intComboSelections.size() > 1) {
		//itemsPositionSelected3.setValue(itemsPositionCombo[intComboSelections[1]]);
		setCvarString(cvar3, itemsPosition[intComboSelections[1]]);
		setCvarString(cvar4, "");
		writeCvar();
	}
	if (intComboSelections.size() > 2) {
		setCvarString(cvar4, itemsPosition[intComboSelections[2]]);
		writeCvar();
	}
	if (intComboSelections.size() == 0) {
		setCvarString(cvar2, "");
		setCvarString(cvar3, "");
		setCvarString(cvar4, "");
		writeCvar();
	}
	// JUSQUE LA
}

bool CustomUI::cvarIsText(CVarWrapper cvar) {
	// Liste des valeurs attendues
	const std::vector<std::string> validValues = {
		"",
		"settingsBoostText",
		"settingsScoreMyTeam",
		"settingsScoreOppositeTeam",
		"settingsGameTime"
	};

	// Récupérer la chaîne du CVar
	std::string value = getCvarString(cvar.getCVarName());

	// Parcourir les valeurs attendues
	for (const auto& validValue : validValues) {
		if (value == validValue) {
			return true;
		}
	}

	return false;
}


//void CustomUI::addNewItemCombo(string nameCombo) {
//	if (ImGui::Combo("##ItemsToModify", &currentPosition, itemsPositionCombo.data(), itemsPositionCombo.size())) {
//		setCvarString(itemsPositionSelected, itemsPosition[currentPosition]);
//		writeCvar();
//	}
//}

//void CustomUI::drawPositionSizeMenu(int currentPosition, string keyPreset, CVarWrapper itemsPositionSelected, CVarWrapper itemsPositionSelected2, CVarWrapper itemsPositionSelected3, CVarWrapper itemsPositionSelected4) {
//	
//	
//}

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
	std::vector<std::string> settingsItemsList = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};


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

	ImGui::Text("Position X");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderFloat("##PositionX", &sliderX, 0.0f, 1.0f, "%.3f"))
	{
		changePositionX = floatToIntPosition(sliderX, screenSize.X);

	}
	ImGui::SameLine();
	if (ImGui::Button(" - ##PositionX-"))
	{
		changePositionX -= 1;
		sliderX = intToFloatPosition(changePositionX, screenSize.X);
	}
	ImGui::SameLine();
	if (ImGui::Button(" + ##PositionX+"))
	{
		changePositionX += 1;
		sliderX = intToFloatPosition(changePositionX, screenSize.X);
	}
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset X"))
	{
		changePositionX = 0;
		sliderX = 0.5f;
	}
	ImGui::PopStyleColor(3);
	ImGui::Text("Position Y");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderFloat("##PositionY", &sliderY, 0.0f, 1.0f))
	{
		changePositionY = floatToIntPosition(sliderY, screenSize.Y);
	}
	ImGui::SameLine();
	if (ImGui::Button(" - ##PositionY-"))
	{
		changePositionY -= 1;
		sliderY = intToFloatPosition(changePositionY, screenSize.Y);
	}
	ImGui::SameLine();
	if (ImGui::Button(" + ##PositionY+"))
	{
		changePositionY += 1;
		sliderY = intToFloatPosition(changePositionY, screenSize.Y);
	}
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset Y"))
	{
		sliderY = 0.5f;
		changePositionY = 0;
	}
	ImGui::PopStyleColor(3);


	if (ImGui::Button("Save Position"))
	{
		/*updateJsonFieldFloat(keyPreset, settingsItems, "positionX", sliderX);
		updateJsonFieldFloat(keyPreset, settingsItems, "positionY", sliderY);*/
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonFieldFloat(keyPreset, settingsItem, "positionX", sliderX);
			updateJsonFieldFloat(keyPreset, settingsItem, "positionY", sliderY);
		}
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
	/*string settingsItems2 = getCvarString("CustomUI_itemsNamePosition2");
	string settingsItems3 = getCvarString("CustomUI_itemsNamePosition3");
	string settingsItems4 = getCvarString("CustomUI_itemsNamePosition4");*/
	std::vector<std::string> settingsItemsList = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

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
	ImGui::Text("Size");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderFloat("##Size", &slider_x, 0, 5, "%.2f")) {
		changeSizeX = slider_x;
		changeSizeY = slider_x;
	}
	ImGui::SameLine();
	if (ImGui::Button(" - ##Size-"))
	{
		changeSizeX -= .01f;
		changeSizeY -= .01f;
		slider_x -= .01f;
	}
	ImGui::SameLine();
	if (ImGui::Button(" + ##Size+"))
	{
		changeSizeX += .01f;
		changeSizeY += .01f;
		slider_x += .01f;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset Scale"))
	{
		changeSizeX = 1.0f;
		changeSizeY = 1.0f;
		slider_x = 1.0f;
		slider_y = 1.0f;
	}
	ImGui::PopStyleColor(3);


	if (ImGui::Button("Save Size")) {

		/*updateJsonFieldFloat(keyPreset, settingsItems, "sizeX", changeSizeX);
		updateJsonFieldFloat(keyPreset, settingsItems, "sizeY", changeSizeY);*/
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonFieldFloat(keyPreset, settingsItem, "sizeX", changeSizeX);
			updateJsonFieldFloat(keyPreset, settingsItem, "sizeY", changeSizeY);
		}
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

void CustomUI::showRenderEditColor() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	std::vector<std::string> settingsItemsList = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	static float baseColor[4];
	//ImGui::ColorPicker3("##picker", (float*)&col1, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
	//ImGui::ColorPicker4("##picker", (float*)&col2, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);*/
	//ImGui::ColorEdit3("color 1", col1);
	//ImGui::ColorPicker3("Text Color", (float*)&col2, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoOptions);
	
	array<int, 4>& settings = getSettingsSettingsColor(currentPreset, settingsItems);

	if (!changingBeginSize) {
		baseColor[0] = settings[0] / 255.0f;
		baseColor[1] = settings[1] / 255.0f;
		baseColor[2] = settings[2] / 255.0f;
		baseColor[3] = settings[3] / 255.0f;
		changeColorR = settings[0];
		changeColorG = settings[1];
		changeColorB = settings[2];
		changeColorA = settings[3];
		changingBeginSize = true;
	}

	ImGui::SetNextItemWidth(300);
	if (ImGui::ColorEdit4("Text Color", baseColor, ImGuiColorEditFlags_NoOptions | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_InputRGB)) {
		changeColorR = baseColor[0] * 255.0f;
		changeColorG = baseColor[1] * 255.0f;
		changeColorB = baseColor[2] * 255.0f;
		changeColorA = baseColor[3] * 255.0f;

	}
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset Color"))
	{
		changeColorR = 255;
		changeColorG = 255;
		changeColorB = 255;
		changeColorA = 255;
		for (const auto& settingsItem : settingsItemsList) {
			try {
				string colorSettingsJson = getStringSettingsColor(settingsItem);
				array<int, 4> colorValues = { changeColorR, changeColorG, changeColorB, changeColorA };
				LOG("NON Erreur lors du traitement de '" + settingsItem);
				updateJsonColor(keyPreset, colorSettingsJson, colorValues);
			}
			catch (const std::exception& e) {
				LOG("Erreur lors du traitement de '" + settingsItem + "': " + std::string(e.what()));
			}
		}
		changeColorR = 0;
		changeColorG = 0;
		changeColorB = 0;
		changeColorA = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
	ImGui::PopStyleColor(3);
	if (ImGui::Button("Save Color")) {

		for (const auto& settingsItem : settingsItemsList) {
			try {
				string colorSettingsJson = getStringSettingsColor(settingsItem);
				array<int, 4> colorValues = { changeColorR, changeColorG, changeColorB, changeColorA };
				LOG("NON Erreur lors du traitement de '" + settingsItem);
				updateJsonColor(keyPreset, colorSettingsJson, colorValues);
			}
			catch (const std::exception& e) {
				LOG("Erreur lors du traitement de '" + settingsItem + "': " + std::string(e.what()));
			}
		}

		changeColorR = 0;
		changeColorG = 0;
		changeColorB = 0;
		changeColorA = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeColorR = 0;
		changeColorG = 0;
		changeColorB = 0;
		changeColorA = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showSizeEditor = false;
		changingBeginSize = false;
	}
}
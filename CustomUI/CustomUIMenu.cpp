#include "pch.h"
#include "CustomUI.h"
#include <cmath>
#include <unordered_set>


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

	style.Colors[ImGuiCol_PopupBg] = baseBlackLight;

	ImVec2 minSize(800.0f, 700.0f);
	ImVec2 maxSize(1920, 1080);

	// Appliquer les contraintes avant de cr�er la fen�tre
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

	CVarWrapper enableCvar = cvarManager->getCvar("CustomUI_enabled");
	if (!enableCvar) { return; }

	CVarWrapper originalUICvar = cvarManager->getCvar("CustomUI_originalUI");
	if (!originalUICvar) { return; }

	CVarWrapper cvarHideFreeplay = cvarManager->getCvar("CustomUI_hideFreeplay");
	if (!cvarHideFreeplay) { return; }

	CVarWrapper cvarHideSpectator = cvarManager->getCvar("CustomUI_hideSpectator");
	if (!cvarHideSpectator) { return; }

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
	ImVec2 textSizeTextX = ImGui::CalcTextSize("x");

	if (auto renderImageDiscord = discordLogo->GetImGuiTex()) {
		auto sizeDiscord = discordLogo->GetSizeF();
		ImVec2 discordImageSize(sizeDiscord.X, sizeDiscord.Y);

		// Calculer le d�calage horizontal pour aligner l'image � droite
		float offsetX = availableWidth - discordImageSize.x - textSizeTextX.x - 20.0f;
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

		ImGui::SameLine();

		float availableWidthTextX = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidthTextX - textSizeTextX.x - 5.0f);

		ImGui::Text("x");
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::BeginTooltip();
			ImGui::Text("Close");
			ImGui::EndTooltip();
		}
		if (ImGui::IsItemClicked()) {
			isSettingsOpen = false;
		}

	}
	

	//availableWidth = ImGui::GetContentRegionAvail().x;

	//string numberUser = "Total CustomUI User : " + to_string(numberUserInt);

	std::string text = "Total CustomUI Users:";
	std::string numberStr = std::to_string(numberUserInt);

	ImGui::Text("%s", text.c_str());
	ImGui::SameLine(); 
	ImGui::TextColored(baseYellow, "%s", numberStr.c_str());


	/*ImVec2 textSizeNumberUser = ImGui::CalcTextSize(numberUser.c_str());
	float offsetX = availableWidth - textSizeNumberUser.x - 20.0f;

	if (offsetX > 0) {
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
	}*/
	//ImGui::Text(numberUser.c_str());

	//ImGui::SameLine();

	////ImVec2 cursorPosAfterSupport = ImGui::GetCursorPos();	

	//availableWidth = ImGui::GetContentRegionAvail().x;


	//if (auto renderImageSupport = supportDev->GetImGuiTex()) {

	//	auto sizeSupport = supportDev->GetSizeF();
	//	ImVec2 supportImageSize(sizeSupport.X, sizeSupport.Y);

	//	// Calculer le d�calage horizontal pour aligner l'image � droite
	//	float offsetX = availableWidth - supportImageSize.x - textSizeTextX.x - 20.0f;
	//	if (offsetX > 0) {
	//		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
	//	}

	//	// Dessiner l'image Discord
	//	ImGui::Image(
	//		renderImageSupport,
	//		supportImageSize,
	//		ImVec2(0, 0),
	//		ImVec2(1, 1),
	//		ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
	//	);

	//	// Ajouter les interactions pour l'image Discord
	//	if (ImGui::IsItemHovered()) {
	//		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
	//		ImGui::BeginTooltip();
	//		ImGui::Text("Support CustomUI and help bring more features (Get a special role for your donation on Discord)");
	//		ImGui::EndTooltip();
	//	}
	//	if (ImGui::IsItemClicked()) {
	//		system("Start https://www.paypal.com/donate/?hosted_button_id=DQTCWR5ZRBH46");
	//	}
	//}
	//ImGui::SetCursorPos(cursorPosAfterSupport);

	//ImGui::Spacing();
	//ImGui::Spacing();
	
	

	if (ImGui::BeginTabBar("MainTabBar"))
	{
		if (ImGui::BeginTabItem("Preset"))
		{
			ImGui::Spacing();
			ImGui::Indent(20.0f);

			//LOG(to_string(currentPreset.differentTeam));
			//LOG(to_string(pluginEnabled));
			pluginEnabled = getCvarBool("CustomUI_enabled");
			if (ImGui::Checkbox("Use CustomUI", &pluginEnabled)) {
				setCvarString(enableCvar, to_string(pluginEnabled));
			}

			//ImGui::SameLine();
			hideOriginalUI = getCvarBool("CustomUI_originalUI");
			if (ImGui::Checkbox("Auto disable of the original UI", &hideOriginalUI)) {
				setCvarString(originalUICvar, to_string(hideOriginalUI));
			}
			if (ImGui::IsItemHovered()) {

				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30);

				ImGui::Text("This option hides the original Rocket League UI while in-game");
				ImGui::Text("When the match starts, the UI disappear");
				ImGui::Text("If you pause or score a goal, the UI re appear automatically");
				ImGui::Text("Everything happens seamlessly, no action required");

				ImGui::Spacing();
				ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "/!\\ Caution:");
				ImGui::Text("The only way to hide the original UI with BakkesMod is to disable everything");
				ImGui::Text("This includes the scoreboard, chat, notifications, etc...");

				ImGui::Spacing();
				ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), ":)) Benefits:");
				ImGui::Text("Have only your own design UI");
				ImGui::Text("Avoid TILTING");
				ImGui::Text("No chat means no distractions & trash talk");
				ImGui::Text("If a teammate concedes, you won't see it unless you pause or the UI appear again");

				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();

			}
			ImGui::SameLine();
			if (ImGui::Button("Options"))
				ImGui::OpenPopup("advanced_options_popup");
			if (ImGui::BeginPopup("advanced_options_popup"))
			{
				hideFreeplay = getCvarBool("CustomUI_hideFreeplay");
				if (ImGui::Checkbox("Hide UI in Freeplay", &hideFreeplay)) {
					setCvarString(cvarHideFreeplay, to_string(hideFreeplay));
				}

				hideSpectator = getCvarBool("CustomUI_hideSpectator");
				if (ImGui::Checkbox("Hide UI when Spectator", &hideSpectator)) {
					setCvarString(cvarHideSpectator, to_string(hideSpectator));
				}

				ImGui::EndPopup();
			}



			//ImGui::PushStyleColor(ImGuiCol_Text, redCaution); // Rouge vif
			//ImGui::TextWrapped("Warning: Be careful, this command hides everything, including the main menu, pause menu, quick chat, etc... during the game, use it with caution!");
			//ImGui::PopStyleColor();

			//ImGui::Spacing();

			static vector<const char*> itemsPreset;
			static int currentChoosenPreset = 0;

			if (presetReload) {

				currentPreset = loadCurrentPreset(getCvarString("CustomUI_choosenPresets"));
				// Lire le CVar contenant le nom du preset s�lectionn�
				string savedPresetName = presetChoosenCvar.getStringValue();

				// Remplir le tableau des noms de presets
				itemsPreset.clear();
				int index = 0;
				for (const auto& pair : allPresets) {
					itemsPreset.push_back(pair.first.c_str());
					if (pair.first == savedPresetName) {
						currentChoosenPreset = index;  // Trouver l'indice correspondant au preset enregistr�
					}
					index++;
				}
				presetReload = false;
			}

			ImGui::Text("Choose a Preset");

			
			ImGui::SetNextItemWidth(400.0f);
			if (!itemsPreset.empty() && ImGui::BeginCombo("##ChoosePreset", itemsPreset[currentChoosenPreset])) {

				unordered_set<std::string> categoriesHandled; // Pour �viter les doublons de cat�gories

				for (size_t i = 0; i < itemsPreset.size(); i++) {
					const auto& presetName = itemsPreset[i];
					const auto& presetData = allPresets[presetName];

					if (!presetData.group.empty()) { // Si le preset appartient � une cat�gorie
						if (categoriesHandled.find(presetData.group) == categoriesHandled.end()) {
							if (ImGui::BeginMenu(presetData.group.c_str())) { // Ouvre le menu pour la cat�gorie
								for (size_t j = 0; j < itemsPreset.size(); j++) {
									const auto& otherPreset = itemsPreset[j];
									const auto& otherData = allPresets[otherPreset];

									if (otherData.group == presetData.group) {
										if (ImGui::MenuItem(otherPreset, nullptr, currentChoosenPreset == j)) {
											currentChoosenPreset = j;
											auto selectedPreset = allPresets.find(otherPreset);
											currentPosition = 0;
											setCvarString(presetChoosenCvar, selectedPreset->first);
											setCvarString(boostFormCvar, selectedPreset->second.boostForm);
											currentPreset = loadCurrentPreset(selectedPreset->first);
											loadThemeFont();
											appendFont();
										}
									}
								}
								ImGui::EndMenu();
							}
							categoriesHandled.insert(presetData.group);
						}
					}
					else { // Si le preset n'a pas de cat�gorie, l'afficher directement
						if (ImGui::Selectable(presetName, currentChoosenPreset == i)) {
							currentChoosenPreset = i;
							auto selectedPreset = allPresets.find(presetName);
							currentPosition = 0;
							setCvarString(presetChoosenCvar, selectedPreset->first);
							setCvarString(boostFormCvar, selectedPreset->second.boostForm);
							currentPreset = loadCurrentPreset(selectedPreset->first);
							loadThemeFont();
							appendFont();
						}
					}
				}

				ImGui::EndCombo();
			}


			


			//ImGui::SetNextItemWidth(400.0f);
			//if (!itemsPreset.empty() && ImGui::Combo("##ChoosePreset", &currentChoosenPreset, itemsPreset.data(), itemsPreset.size())) {
			//	// Appliquer le preset s�lectionn�
			//	auto selectedPreset = allPresets.begin();
			//	std::advance(selectedPreset, currentChoosenPreset);  // Aller au preset correspondant
			//	currentPosition = 0;

			//	setCvarString(presetChoosenCvar, selectedPreset->first);
			//	setCvarString(boostFormCvar, selectedPreset->second.boostForm);
			//	currentPreset = loadCurrentPreset(selectedPreset->first);
			//	loadThemeFont();
			//	appendFont();
			//}

			//if (ImGui::IsItemHovered()) {
			//	ImGui::SetTooltip("Choose the preset to apply");
			//}

			std::vector<const char*> itemsPositionCombo;
			std::vector<const char*> itemsPosition;

			if (!isArtistMode) {
				itemsPositionCombo = { "", "Boost All Items", "Score All Items" };
				itemsPosition = { "", "settingsBoostAllItems", "settingsScoreAllItems" };
			}
			else {
				if (isTexture) {
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
				else {
					itemsPositionCombo = {
						"",
						"Background Image Boost",
						"Texture Circle Boost",
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
				
			}
			std::vector<std::string> settingsItemsList = {
				getCvarString("CustomUI_itemsNamePosition"),
				getCvarString("CustomUI_itemsNamePosition2"),
				getCvarString("CustomUI_itemsNamePosition3"),
				getCvarString("CustomUI_itemsNamePosition4")
			};

			ImGui::SameLine();

			if (ImGui::Button("Refresh")) {
				refreshFiles();
			}
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
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Remove the last dropdown menu");
			}
			ImGui::SameLine();
			if (ImGui::Button(" + ##AddItem")) {
				if (intComboSelections.size() < maxComboCount) {
					itemsAdded.push_back("Nouvel �l�ment " + std::to_string(itemsAdded.size() + 1));
					intComboSelections.push_back(0);
					changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
				}
				
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Add a new dropdown menu for an additional item");
			}
			if (!itemsAdded.empty()) {
				for (size_t i = 0; i < itemsAdded.size(); ++i) {

					ImGui::SetNextItemWidth(400.0f);
					if (ImGui::Combo(("##Combo_" + std::to_string(i)).c_str(),
						&intComboSelections[i], itemsPositionCombo.data(),
						itemsPositionCombo.size())) 
					{
						changeCvarCombo(itemsPositionSelected2, itemsPositionSelected3, itemsPositionSelected4, itemsPosition);
						//ImGui::Text("Option s�lectionn�e : %s", itemsPositionCombo[intComboSelections[i]]);
					}
				}
			}

			static bool showError = false;
			static float errorTimer = 0.0f;
			static bool showErrorColor = false;
			static float errorTimerColor = 0.0f;
			static bool showErrorCircle = false;
			static float errorTimerCircle = 0.0f;

			if (ImGui::Button("Edit position"))
			{
				if (currentPosition != 0) {
					showPositionEditor = true;
					showSizeEditor = false;
					showColorEditor = false;
					showCircleEditor = false;
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
					showCircleEditor = false;
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
						showCircleEditor = false;
						showErrorColor = false;
					}
					else {
						showErrorColor = true;
						errorTimerColor = 2.0f;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Edit Circle Angle"))
				{
					if (currentPosition != 0 && cvarIsCircleBoost(itemsPositionSelected) && cvarIsCircleBoost(itemsPositionSelected2) && cvarIsCircleBoost(itemsPositionSelected3) && cvarIsCircleBoost(itemsPositionSelected4)) {
						showCircleEditor = true;
						showColorEditor = false;
						showPositionEditor = false;
						showSizeEditor = false;
						showErrorCircle = false;
					}
					else {
						showErrorCircle = true;
						errorTimerCircle = 2.0f;
					}
				}
			}
			if (currentPreset.differentTeam) {
				// D�finir la couleur du bouton en fonction de colorTeamBool
				if (!colorTeamBool) {
					ImGui::PushStyleColor(ImGuiCol_Button, blueRL);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, blueRLHovered);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, blueRLActive);
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, orangeRL);
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, orangeRLHovered);
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, orangeRLActive);
				}

				string buttonLabel = "Switch to " + getColorTeamByBool() + " Preset##ButtonChangeTeamColor";
				ImGui::SameLine();
				if (ImGui::Button(buttonLabel.c_str())) {
					colorTeamBool = !colorTeamBool;
				}

				// R�initialiser la couleur par d�faut
				ImGui::PopStyleColor(3);
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
			if (showErrorCircle) {
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Select only the Circle Texture to modify his start and end angle");
				errorTimerCircle -= ImGui::GetIO().DeltaTime;

				if (errorTimerCircle <= 0.0f) {
					showErrorCircle = false;
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
			if (showCircleEditor) {
				showRenderEditCircle();
			}
			/*if (ImGui::Checkbox("Disable Base UI", &isDisableBasicUI)) {
			
			};*/

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

			/*ImGui::Checkbox("Hide the Rocket League UI in game", &isDisableBasicUI);
			ImGui::Text(to_string(isDisableBasicUI).c_str());*/

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Credits"))
		{
			ImGui::Spacing();
			ImGui::Indent(20.0f);
			ImGui::Text("Creator & Developer :");
			ImGui::Spacing();
			ImGui::Indent(15.0f);
			ImGui::TextColored(baseYellow, "Joucaz");
			//ImGui::Text("Joucaz");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.7f, size.Y/1.7f),
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
			ImGui::SameLine();
			if (auto renderImageLogoText = githubLogo->GetImGuiTex()) {
				auto size = githubLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.7f, size.Y/1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Joucaz Repositories");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://github.com/Joucaz");
			}
			ImGui::SameLine();
			if (auto renderImageLogoText = wesbiteLogo->GetImGuiTex()) {
				auto size = wesbiteLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X/1.7f, size.Y/1.7f),
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

			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Indent(-15.0f);
			ImGui::Text("Preset Artist :");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Indent(15.0f);

			ImGui::TextColored(sucreDorgeColor, "SucreDorge");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@SucreDorgePSD");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://x.com/SucreDorgePSD");
			}

			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();

			ImGui::TextColored(kisuteColor, "Kisute");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@Kisute3");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://x.com/Kisute3");
			}

			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();

			ImGui::TextColored(kcColor, "Emmy");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@CaptainArteis");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://x.com/CaptainArteis");
			}
			ImGui::Spacing();
			ImGui::TextColored(krogezoColor, "Krogezo");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@Krogezo");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://x.com/Krogezo");
			}

			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();

			ImGui::TextColored(whiteGrey, "Tanderiz");
			ImGui::SameLine();
			if (auto renderImageLogoText = xLogo->GetImGuiTex()) {
				auto size = xLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@Tanderiz");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://x.com/Tanderiz");
			}

			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();

			ImGui::TextColored(joshColor, "Jo$h");
			ImGui::SameLine();
			if (auto renderImageLogoText = instagramLogo->GetImGuiTex()) {
				auto size = instagramLogo->GetSizeF();

				ImGui::Image(
					renderImageLogoText,
					ImVec2(size.X / 1.7f, size.Y / 1.7f),
					ImVec2(0, 0),
					ImVec2(1, 1),
					ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
				);
			}
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("@j0sh.psd");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://www.instagram.com/j0sh.psd/");
			}
			ImGui::Spacing();
			ImGui::Spacing();

			ImGui::Indent(-15.0f);
			ImGui::Text("Thanks to :");
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Indent(15.0f);
			ImGui::TextColored(redCautionHovered, "Arubinu");
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Twitch : Arubinu42");
				ImGui::EndTooltip();
			}
			if (ImGui::IsItemClicked()) {
				system("Start https://twitch.tv/Arubinu42");
			}

			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();

			ImGui::TextColored(baseGrey, "Vync");
			/*if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Vync Plugin");
				ImGui::EndTooltip();
			}*/
			/*ImGui::SameLine();
			if (ImGui::IsItemClicked()) {
				system("Start https://bakkesplugins.com/plugins/view/223");
			}*/
			ImGui::SameLine();
			ImGui::Text(" | ");
			ImGui::SameLine();
			ImGui::TextColored(soulDaMeepColor, "SoulDaMeep");
			if (ImGui::IsItemHovered()) {
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::BeginTooltip();
				ImGui::Text("Discord: @souldameep");
				ImGui::EndTooltip();
			}
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("Patch Notes")) {
			ImGui::Spacing();
			ImGui::Indent(20.0f);
			// Tableau des patch notes (titre et contenu)
			PatchNote patch_notes[] = {
				{
					"v1.2.0 (latest)", R"(
Patch Notes v1.2.0 
Date: 19/04/2025

-------------------

New Features:

- Added an option to automatically hide the original Rocket League UI. The UI is hidden when the game starts and reappears when a goal is scored or the pause menu is opened. You can enable/disable this feature separately for gameplay, freeplay, and spectating.
- You can now customize the start and end positions of the Circle Boost.
- Added the "RLCS 2024" preset.
- Added multiple design for content creators (Kaydop, Mawkzy, Feer, Yota).
- Introduced a support button to help fund the plugin's development and future features.

-------------------

Improvements and Changes:

- The total number of active plugin users is now displayed and updated in real time.
- Presets can now be grouped for better organization.
- Increased the minimum window size for improved usability.

-------------------

Bug Fixes:

- Fixed random crashes at the start of a game that could result in a loss.
- Fixed the bug where the CustomUI did not display automatically on Rocket League launch and required manually toggling the menu.
- Fixed the bug where replay mode made the game crash with CustomUI plugin.
- Fixed the bug where the boost would disappear if the game was paused and a replay ended.

					)"
				},
				{
					"v1.1.0", R"(
Patch Notes v1.1.0 
Date: 30/01/2025

-------------------

New Features:

- Added a button to enable/disable the plugin's UI directly from the plugin window.
- Added the ability to create distinct images for the blue and orange teams.
- Added the option to switch between the blue and orange teams in freeplay for presets that support this functionality.
- Added the ability to customize text and circle texture boost colors for each team (blue and orange).
- Added the detection of score and the boost amount in spectate mode.
- Added the "Patch Notes" tab item.

-------------------

Improvements and Changes:

- Integrated database connectivity to track statistics, including the number of users and the presets used.

-------------------

Bug Fixes:

- Fixed the bug where the Circle boost was not moving in the same way as the background boost image.
- Fixed the bug where the boost component was not showing in spectate mode due to the pause menu.
- Fixed the bug where the "+" and "-" buttons could not be used to precisely position items, ensuring they now stay in the correct place when saving.

					)"
				},
				/*{
					"v1.3.0", R"(
Patch Notes v1.2.0:
-------------------
- Added support for custom team overlays.
- Improved performance in large scenes.
- Fixed a bug where player names would not display correctly.
- Enhanced UI for esport features.

Known Issues:
- Minor visual glitches on high DPI screens.
					)"
				},*/
			};

			static int selected_patch = 0;

			// Combo pour choisir le patch note (les titres sont extraits directement du tableau patch_notes)
			const int patch_count = IM_ARRAYSIZE(patch_notes);
			const char** patch_titles = new const char* [patch_count];
			for (int i = 0; i < patch_count; ++i) {
				patch_titles[i] = patch_notes[i].title;
			}


			ImGui::SetNextItemWidth(400.0f);
			if (ImGui::Combo("Select Patch Note", &selected_patch, patch_titles, patch_count)) {
				
			}


			ImGui::TextWrapped("%s", patch_notes[selected_patch].content);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();



		
	}



	if (auto renderImageSupport = supportDev->GetImGuiTex()) {

		auto sizeSupport = supportDev->GetSizeF();
		ImVec2 supportImageSize(sizeSupport.X, sizeSupport.Y);

		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 cursorStartPos = ImGui::GetCursorPos(); // Sauvegarde de la position initiale du curseur
		float scrollY = ImGui::GetScrollY();

		// Calculer la position en bas � droite
		float posX = windowSize.x - supportImageSize.x - 20.0f;
		float posY = windowSize.y - supportImageSize.y - 20.0f + scrollY;

		// Placer le curseur � la bonne position
		ImGui::SetCursorPos(ImVec2(posX, posY));


		// Dessiner l'image Discord
		ImGui::Image(
			renderImageSupport,
			supportImageSize,
			ImVec2(0, 0),
			ImVec2(1, 1),
			ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
		);

		// Ajouter les interactions pour l'image Discord
		if (ImGui::IsItemHovered()) {
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			ImGui::BeginTooltip();
			ImGui::Text("Support CustomUI and help bring more features (Get a special role for your donation on Discord)");
			ImGui::EndTooltip();
		}
		if (ImGui::IsItemClicked()) {
			system("Start https://www.paypal.com/donate/?hosted_button_id=DQTCWR5ZRBH46");
		}
		ImGui::SetCursorPos(cursorStartPos);
	}

	//ImGui::SameLine();

	
	
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
		"settingsBoostTexture",
		"settingsScoreMyTeam",
		"settingsScoreOppositeTeam",
		"settingsGameTime"
	};

	// R�cup�rer la cha�ne du CVar
	std::string value = getCvarString(cvar.getCVarName());

	// Parcourir les valeurs attendues
	for (const auto& validValue : validValues) {
		if (value == validValue) {
			return true;
		}
	}

	return false;
}

bool CustomUI::cvarIsCircleBoost(CVarWrapper cvar) {
	// Liste des valeurs attendues
	const std::vector<std::string> validValues = {
		"",
		"settingsBoostTexture",
	};

	// R�cup�rer la cha�ne du CVar
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

	// Arrondi
	result = std::round(result * 10000.0f) / 10000.0f;

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
	if (ImGui::SliderFloat("##PositionX", &sliderX, 0.0f, 1.0f, "%.4f"))
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
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonFieldFloat(keyPreset, settingsItem, "positionX", sliderX);
			updateJsonFieldFloat(keyPreset, settingsItem, "positionY", sliderY);
		}
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginPosition = false;
	}
	ImGui::PopStyleColor(3);
	ImGui::Text("Position Y");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderFloat("##PositionY", &sliderY, 0.0f, 1.0f, "%.4f"))
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
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonFieldFloat(keyPreset, settingsItem, "positionX", sliderX);
			updateJsonFieldFloat(keyPreset, settingsItem, "positionY", sliderY);
		}
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginPosition = false;
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
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginPosition = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		changePositionX = 0;
		changePositionY = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
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
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonFieldFloat(keyPreset, settingsItem, "sizeX", changeSizeX);
			updateJsonFieldFloat(keyPreset, settingsItem, "sizeY", changeSizeY);
		}
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginSize = false;
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
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginSize = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeSizeX = 1;
		changeSizeY = 1;
		showPositionEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
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
	
	array<int, 4>& settings = getSettingsSettingsColor(currentPreset, settingsItems);

	if (!changingBeginColor) {
		baseColor[0] = settings[0] / 255.0f;
		baseColor[1] = settings[1] / 255.0f;
		baseColor[2] = settings[2] / 255.0f;
		baseColor[3] = settings[3] / 255.0f;
		changeColorR = settings[0];
		changeColorG = settings[1];
		changeColorB = settings[2];
		changeColorA = settings[3];
		changingBeginColor = true;
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
		showCircleEditor = false;
		showSizeEditor = false;
		changingBeginColor = false;
	}
	ImGui::PopStyleColor(3);
	if (ImGui::Button("Save Color")) {

		for (const auto& settingsItem : settingsItemsList) {
			try {
				string colorSettingsJson = getStringSettingsColor(settingsItem);
				LOG(colorSettingsJson);
				array<int, 4> colorValues = { changeColorR, changeColorG, changeColorB, changeColorA };
				updateJsonColor(keyPreset, colorSettingsJson, colorValues);
			}
			catch (const std::exception& e) {
				LOG("Erreur lors du caca traitement de '" + settingsItem + "': " + std::string(e.what()));
			}
		}

		changeColorR = 0;
		changeColorG = 0;
		changeColorB = 0;
		changeColorA = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showCircleEditor = false;
		showSizeEditor = false;
		changingBeginColor = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeColorR = 0;
		changeColorG = 0;
		changeColorB = 0;
		changeColorA = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showCircleEditor = false;
		showSizeEditor = false;
		changingBeginColor = false;
	}
}

void CustomUI::showRenderEditCircle() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	std::vector<std::string> settingsItemsList = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};


	static int sliderStart = 0;
	static int sliderMax = 65;

	Angle& circleAngle = currentPreset.circleAngle;

	if (!changingBeginCircle) {
		
		changeCircleStartAngle = circleAngle.startAngle;
		changeCircleMaxAngle = circleAngle.maxAngle;
		sliderStart = circleAngle.startAngle;
		sliderMax = circleAngle.maxAngle;
		changingBeginCircle = true;
	}

	ImGui::Text("Start Angle");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderInt("##StartAngle", &sliderStart, 0, 100, "%d"))
	{
		changeCircleStartAngle = sliderStart;
	}
	ImGui::SameLine();
	if (ImGui::Button(" - ##StartAngle-"))
	{
		changeCircleStartAngle -= 1;
		sliderStart -= 1;
	}
	ImGui::SameLine();
	if (ImGui::Button(" + ##StartAngle+"))
	{
		changeCircleStartAngle += 1;
		sliderStart += 1;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset Start Angle"))
	{
		changeCircleStartAngle = 0;
		sliderStart = 0;
		for (const auto& settingsItem : settingsItemsList) {			
			updateJsonCircleAngle(keyPreset, "circleAngle", sliderStart, sliderMax);
		}
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginCircle = false;
	}
	ImGui::PopStyleColor(3);

	ImGui::Text("Max Angle");
	ImGui::SetNextItemWidth(300.0f);
	if (ImGui::SliderInt("##MaxAngle", &sliderMax, 0, 100, "%d"))
	{
		changeCircleMaxAngle = sliderMax;
	}
	ImGui::SameLine();
	if (ImGui::Button(" - ##MaxAngle-"))
	{
		changeCircleMaxAngle -= 1;
		sliderMax -= 1;
	}
	ImGui::SameLine();
	if (ImGui::Button(" + ##MaxAngle+"))
	{
		changeCircleMaxAngle += 1;
		sliderMax += 1;
	}
	ImGui::PushStyleColor(ImGuiCol_Button, redCaution);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, redCautionActive);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, redCautionHovered);
	ImGui::SameLine();
	if (ImGui::Button("Reset Max Angle"))
	{
		changeCircleMaxAngle = 0;
		sliderMax = 65;
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonCircleAngle(keyPreset, "circleAngle", sliderStart, sliderMax);
		}
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginCircle = false;
	}
	ImGui::PopStyleColor(3);


	if (ImGui::Button("Save Angle"))
	{
		for (const auto& settingsItem : settingsItemsList) {
			updateJsonCircleAngle(keyPreset, "circleAngle", sliderStart, sliderMax);
		}
		changeCircleStartAngle = 0;
		changeCircleMaxAngle = 0;
		showPositionEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		showColorEditor = false;
		changingBeginCircle = false;
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {

		changeCircleStartAngle = 0;
		changeCircleMaxAngle = 0;
		showPositionEditor = false;
		showColorEditor = false;
		showSizeEditor = false;
		showCircleEditor = false;
		changingBeginCircle = false;
	}
}
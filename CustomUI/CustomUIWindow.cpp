#include "pch.h"
#include "CustomUI.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

using namespace std;

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif


int CustomUI::intChangePositionX(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showPositionEditor) {
			return changePositionX;
		}
	}
	return settings.int1;
	
}
int CustomUI::intChangePositionY(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showPositionEditor) {
			return changePositionY;
		}
	}
	return settings.int2;

}

float CustomUI::floatChangeSizeX(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showSizeEditor) {
			return changeSizeX;
		}
	}
	return settings.float1;

}
float CustomUI::floatChangeSizeY(SettingsItems settings, string settingsName) {
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	if (settingsName == settingsItems) {
		if (showSizeEditor) {
			return changeSizeY;
		}
	}
	return settings.float2;

}

void CustomUI::Render() {
	int idx = ImGui::GetKeyIndex(ImGuiKey_Escape);
	if (ImGui::IsKeyDown(idx))
		escape_state = true;
	else if (ImGui::IsKeyReleased(idx))
		escape_state = false;
	if (isInFreeplay() || isInGame()) {
		RenderWindow();
	}
	if (isSettingsOpen) {
		RenderMenu();
	}
	//LOG(" " + to_string(isSettingsOpen));
}


// Do ImGui rendering here
void CustomUI::RenderWindow()
{
	// The imgui window allows the quads to show on the screen
	ImVec2 windowPos = ImVec2((0), 0);
	ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(screenSize.X, screenSize.Y));
	//ImGui::SetNextWindowSize(ImVec2(screenSize.X - (1660 * xPercent) + (10 * xPercent), screenSize.Y + (10 * yPercent)));

	static bool no_titlebar = true;
	static bool no_scrollbar = true;
	static bool no_menu = true;
	static bool no_move = false;
	static bool no_resize = false;
	static bool no_collapse = true;
	static bool no_nav = false;
	static bool no_background = true;
	static bool no_bring_to_front = false;

	bool opened = true;

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
	//if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_)) {
	if (!ImGui::Begin(menuTitle_.c_str(), &opened, window_flags)) {

		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	if (!myFont) {
		auto gui = gameWrapper->GetGUIManager();
		myFont = gui.GetFont("Oswald200");
	}
	//LOG("font size " + to_string(gameWrapper->GetGUIManager().GetFont("Oswald200")->FontSize));

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (myFont) {
		ImGui::PushFont(myFont);
		//ImGui::Text("Custom UI Overlay");
	}

	if (isInGame() && gameDisplay) {
		drawScore(drawList);
		drawBoost(drawList);
	}
	else if (isInGame() && replayDisplay && !gameDisplay) {
		drawScore(drawList);
	}
	else if (isInFreeplay() && !zeroBoost(boost)) {
		drawBoost(drawList);
	}
		
	if (myFont) {
		ImGui::PopFont();
		//ImGui::Text("The custom font haven't been loaded yet");
	}

	ImGui::End();

	//if (!isWindowOpen_)
	//{
	//	_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	//}

}


void CustomUI::drawScore(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	if (imageScore[keyPreset]->IsLoadedForImGui()) {
		if (auto renderImageScore = imageScore[keyPreset]->GetImGuiTex()) {
			auto size = imageScore[keyPreset]->GetSizeF();
			ImGui::SetCursorPos(ImVec2(0, 0));
			drawList->AddImage(renderImageScore, ImVec2(0, 0),
				ImVec2(size.X * xPercent, size.Y * yPercent),
				ImVec2{ 0, 0 }, ImVec2{ 1, 1 },
				ImU32(0xFFFFFFFF));
		}

	}

	Vector2 PositionScoreA = { 802, -8 };
	Vector2 PositionGametime = { 897, -10 };
	Vector2 PositionScoreB = { 1082, -8 };

	if (isOvertime)
	{
		PositionGametime = { PositionGametime.X - 13 , PositionGametime.Y };
	}
	if (scoreA >= 10)
	{
		PositionScoreA = { PositionScoreA.X - 8 , PositionScoreA.Y };
	}
	if (scoreB >= 10)
	{
		PositionScoreB = { PositionScoreB.X - 8 , PositionScoreB.Y };
	}

	ImU32 colorScoreMyTeam = IM_COL32(allPresets[keyPreset].colorScoreMyTeam[0], allPresets[keyPreset].colorScoreMyTeam[1], allPresets[keyPreset].colorScoreMyTeam[2], allPresets[keyPreset].colorScoreMyTeam[3]);
	ImU32 colorScoreOppositeTeam = IM_COL32(allPresets[keyPreset].colorScoreOppositeTeam[0], allPresets[keyPreset].colorScoreOppositeTeam[1], allPresets[keyPreset].colorScoreOppositeTeam[2], allPresets[keyPreset].colorScoreOppositeTeam[3]);
	ImU32 colorGameTime = IM_COL32(allPresets[keyPreset].colorGameTime[0], allPresets[keyPreset].colorGameTime[1], allPresets[keyPreset].colorGameTime[2], allPresets[keyPreset].colorGameTime[3]);

	drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionScoreA.X * xPercent, PositionScoreA.Y * yPercent),
		colorScoreMyTeam, to_string(scoreA).c_str());
	drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionGametime.X * xPercent, PositionGametime.Y * yPercent),
		colorGameTime, (gameTime).c_str());
	drawList->AddText(myFont, 110 * xPercent, ImVec2(PositionScoreB.X * xPercent, PositionScoreB.Y * yPercent),
		colorScoreOppositeTeam, to_string(scoreB).c_str());
}

void CustomUI::drawBoost(ImDrawList* drawList) {
	
	drawBoostDisplay(drawList);
	if (isTexture) {
		drawBoostTexture(drawList);
		//drawBoostText(drawList, 1660, 825, 1700, 825, 1680, 825);
		drawBoostText(drawList, 140, 120, 180, 120, 160, 120);
	}
	else {
		drawBoostCircle(drawList);
		//drawBoostText(drawList, 1690, 825, 1730, 825, 1710, 825);
		drawBoostText(drawList, 125, 120, 170, 120, 150, 120);
	}

}


void CustomUI::drawBoostDisplay(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostDisplayArray = preset.settingsBoostDisplay;
	auto& settingsBoostAllItemsArray = preset.settingsBoostAllItems;

	if (!imageDisplayBoost[keyPreset]->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = imageDisplayBoost[keyPreset]->GetImGuiTex()) {
		auto size = imageDisplayBoost[keyPreset]->GetSizeF();

		const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
			? settingsBoostAllItemsArray
			: settingsBoostDisplayArray;
		const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
			? settingsBoostDisplayArray
			: settingsBoostAllItemsArray;

		const string& stringSettings = (settingsItems == "settingsBoostAllItems")
			? "settingsBoostAllItems"
			: "settingsBoostDisplay";

		ImVec2 position = ImVec2(
			0 + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings),
			0 + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)
		);

		ImVec2 fullSize = ImVec2(
			size.X * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) * xPercent,
			size.Y * baseSettings.float2 * floatChangeSizeY(sourceSettings, stringSettings) * yPercent
		);

		if (preset.format == "custom") {
			drawList->AddImage(
				renderImageBoost,
				ImVec2(position.x, position.y),
				ImVec2(position.x + fullSize.x, position.y + fullSize.y),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
		else {
			drawList->AddImage(
				renderImageBoost,
				ImVec2(
					0 + intChangePositionX(settingsBoostAllItemsArray, "settingsBoostAllItems"),
					0 + intChangePositionY(settingsBoostAllItemsArray, "settingsBoostAllItems")
				),
				ImVec2(
					(size.X * floatChangeSizeX(settingsBoostAllItemsArray, "settingsBoostAllItems") +
						intChangePositionX(settingsBoostAllItemsArray, "settingsBoostAllItems")) * xPercent,
					(size.Y * floatChangeSizeY(settingsBoostAllItemsArray, "settingsBoostAllItems") +
						intChangePositionY(settingsBoostAllItemsArray, "settingsBoostAllItems")) * yPercent
				),
				ImVec2(0, 0),
				ImVec2(1, 1),
				IM_COL32(255, 255, 255, 255)
			);
		}
	}
}

void CustomUI::drawBoostTexture(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostTexture = preset.settingsBoostTexture;
	auto& settingsBoostAllItems = preset.settingsBoostAllItems;

	if (!imageTextureBoost[keyPreset]->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = imageTextureBoost[keyPreset]->GetImGuiTex()) {
		auto size = imageTextureBoost[keyPreset]->GetSizeF();

		// Calcul des ratios et paramètres
		float boostRatio = boost / 100.0f;
		ImVec2 position = { 0, 0 };
		ImVec2 fullSize = { 0, 0 };

		// Récupération des paramètres selon le format
		if (preset.format == "custom") {
			const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
				? settingsBoostAllItems
				: settingsBoostTexture;
			const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
				? settingsBoostTexture
				: settingsBoostAllItems;

			const string& stringSettings = (settingsItems == "settingsBoostAllItems")
				? "settingsBoostAllItems"
				: "settingsBoostTexture";

			position = ImVec2(
				((80 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems")) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)) * xPercent,
				((92 * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems")) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)) * yPercent
			);
			fullSize = ImVec2(
				size.X * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) * xPercent,
				size.Y * baseSettings.float2 * floatChangeSizeY(sourceSettings, stringSettings) * yPercent
			);
		}
		else {
			position = ImVec2(
				((80 * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems")) + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")) * xPercent,
				((92 * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems")) + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems")) * yPercent
			);
			fullSize = ImVec2(
				size.X * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems") * xPercent,
				size.Y * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems") * yPercent
			);
		}

		// Calcul de la hauteur visible selon le boost (du bas vers le haut)
		float visibleHeight = fullSize.y * boostRatio;

		// Ajuster les UVs pour ne montrer que la hauteur visible
		ImVec2 uv_min = ImVec2(0.0f, 1 - boostRatio); // Bas de l'image en fonction du boost
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);           // Haut de l'image

		// Dessiner l'image
		drawList->AddImage(
			renderImageBoost,
			ImVec2(position.x, position.y + (fullSize.y - visibleHeight)), // Position ajustée
			ImVec2(position.x + fullSize.x, position.y + fullSize.y),     // Taille ajustée
			uv_min,
			uv_max,
			IM_COL32(255, 255, 255, 255)
		);
	}
}
//
//
//void CustomUI::drawBoostTexture(ImDrawList* drawList) {
//	string keyPreset = getCvarString("CustomUI_choosenPresets");
//	if (imageTextureBoost[keyPreset]->IsLoadedForImGui()) {
//		if (auto renderImageBoost = imageTextureBoost[keyPreset]->GetImGuiTex()) {
//			auto size = imageTextureBoost[keyPreset]->GetSizeF();
//
//			float boostRatio = boost / 100.0f; // Ratio du boost (entre 0 et 1)
//
//			// Position initiale et taille complète adaptées à l'écran
//			ImVec2 position = ImVec2(80 * xPercent, 92 * yPercent); // Origine (coin supérieur gauche)
//			ImVec2 fullSize = ImVec2(size.X * xPercent, size.Y * yPercent);
//
//			// Calcul de la hauteur visible selon le boost (du bas vers le haut)
//			float visibleHeight = fullSize.y * boostRatio;
//
//			// Ajuster les UVs pour ne montrer que la hauteur visible
//			ImVec2 uv_min = ImVec2(0.0f, 1 - boostRatio); // Bas de l'image en fonction du boost
//			ImVec2 uv_max = ImVec2(1.0f, 1.0f);           // Haut de l'image
//
//			// Ajuster les coordonnées de l'image rognée
//			drawList->AddImage(
//				renderImageBoost,
//				ImVec2(position.x, position.y + (fullSize.y - visibleHeight)), // Position ajustée
//				ImVec2(position.x + fullSize.x, position.y + fullSize.y),     // Taille ajustée
//				uv_min,
//				uv_max,
//				IM_COL32(255, 255, 255, 255)
//			);
//		}
//	}
//}




void CustomUI::drawBoostCircle(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");

	// Paramètres pour le cercle
	ImVec2 center = ImVec2(200 * xPercent, 198 * yPercent); // Centre du cercle
	float radius = 140.0f * xPercent; // Rayon du cercle
	float startAngle = IM_PI / 2.0f; // Début de l'arc (à 3h sur un cadran)
	float maxAngle = IM_PI * 2.0f * 0.65f; // 75% du cercle entier
	float endAngle = startAngle + (maxAngle * (boost / 100.0f)); // Calculer l'angle final en fonction du boost
	float strokeThickness = 26.0f * xPercent; // Échelle de l'épaisseur du trait

	// Définir la couleur du contour du cercle
	ImU32 color = IM_COL32(allPresets[keyPreset].colorBoost[0], allPresets[keyPreset].colorBoost[1], allPresets[keyPreset].colorBoost[2], allPresets[keyPreset].colorBoost[3]);

	// Commencer à dessiner le path (cercle)
	drawList->PathClear();
	drawList->PathArcTo(center, radius, startAngle, endAngle, 100); // 100 segments pour le contour lisse
	drawList->PathStroke(color, false, strokeThickness);
}



void CustomUI::drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y) {

	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	ImU32 color = IM_COL32(allPresets[keyPreset].colorBoost[0], allPresets[keyPreset].colorBoost[1], allPresets[keyPreset].colorBoost[2], allPresets[keyPreset].colorBoost[3]);

	auto& preset = allPresets[keyPreset];
	auto& settingsBoostText = preset.settingsBoostText;
	auto& settingsBoostAllItems = preset.settingsBoostAllItems;

	const auto& sourceSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostAllItems
		: settingsBoostText;
	const auto& baseSettings = (settingsItems == "settingsBoostAllItems")
		? settingsBoostText
		: settingsBoostAllItems;

	const string& stringSettings = (settingsItems == "settingsBoostAllItems")
		? "settingsBoostAllItems"
		: "settingsBoostText";


	int vx = 0, vy = 0;

	if (boost == 100) {
		vx = v1x;
		vy = v1y;
	}
	else if (boost < 10) {
		vx = v2x;
		vy = v2y;
	}
	else {
		vx = v3x;
		vy = v3y;
	}

	if (allPresets[keyPreset].format == "custom") {
		if (settingsItems != "settingsBoostAllItems") {

			drawList->AddText(myFont, 160 * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) * xPercent,
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)) * xPercent,
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)) * yPercent),
				color, to_string(boost).c_str());
		}
		else {
			drawList->AddText(myFont, 160 * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) * xPercent,
				ImVec2((vx * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int1 + intChangePositionX(sourceSettings, stringSettings)) * xPercent,
					(vy * baseSettings.float1 * floatChangeSizeX(sourceSettings, stringSettings) + baseSettings.int2 + intChangePositionY(sourceSettings, stringSettings)) * yPercent),
				color, to_string(boost).c_str());
		}
	}
	else {
		drawList->AddText(myFont, 160 * floatChangeSizeX(sourceSettings, stringSettings) * xPercent,
			ImVec2((vx * floatChangeSizeX(sourceSettings, stringSettings) + intChangePositionX(sourceSettings, stringSettings)) * xPercent,
				(vy * floatChangeSizeX(sourceSettings, stringSettings) + intChangePositionY(sourceSettings, stringSettings)) * yPercent),
			color, to_string(boost).c_str());
	}


}






// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void CustomUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
	auto gui = gameWrapper->GetGUIManager();


	// This syntax requires c++17
	auto [res, font] = gui.LoadFont("Oswald200", "Oswald-VariableFont_wght.ttf", 200);

	if (res == 0) {
		cvarManager->log("Failed to load the font!");
	}
	else if (res == 1) {
		cvarManager->log("The font will be loaded");
	}
	else if (res == 2 && font) {
		myFont = font;
	}
}

string CustomUI::GetMenuName(){
	return menuName_;
}

string CustomUI::GetMenuTitle(){
	return menuTitle_;
}
bool CustomUI::ShouldBlockInput(){
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}
bool CustomUI::IsActiveOverlay(){
	return isSettingsOpen;
}	
void CustomUI::OnOpen(){
	
}
void CustomUI::OnClose(){
	if (escape_state)
	{
		escape_state = false;
		isSettingsOpen = false;
		gameWrapper->SetTimeout([&](GameWrapper* gameWrapper) {
			cvarManager->executeCommand("togglemenu " + GetMenuName());
			}, 0.02f);
	}
}
#include "pch.h"
#include "CustomUI.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

using namespace std;

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif


int CustomUI::intChangePositionX(SettingsItems settings, string settingsName) {
	std::vector<std::string> settingsItems = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	if (std::find(settingsItems.begin(), settingsItems.end(), settingsName) != settingsItems.end()) {
		if (showPositionEditor) {
			return changePositionX;
		}
	}
	return floatToIntPosition(settings.positionX, screenSize.X);
}
int CustomUI::intChangePositionY(SettingsItems settings, string settingsName) {
	std::vector<std::string> settingsItems = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	if (std::find(settingsItems.begin(), settingsItems.end(), settingsName) != settingsItems.end()) {
		if (showPositionEditor) {
			return changePositionY;
		}
	}
	return floatToIntPosition(settings.positionY, screenSize.Y);
}

float CustomUI::floatChangeSizeX(SettingsItems settings, string settingsName) {
	std::vector<std::string> settingsItems = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	if (std::find(settingsItems.begin(), settingsItems.end(), settingsName) != settingsItems.end()) {
		if (showSizeEditor) {
			return changeSizeX;
		}
	}
	return settings.sizeX;

}
float CustomUI::floatChangeSizeY(SettingsItems settings, string settingsName) {
	std::vector<std::string> settingsItems = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	if (std::find(settingsItems.begin(), settingsItems.end(), settingsName) != settingsItems.end()) {
		if (showSizeEditor) {
			return changeSizeY;
		}
	}
	return settings.sizeY;

}


ImU32 CustomUI::changeColorText(array<int, 4> settingsColor, string settingsName) {
	std::vector<std::string> settingsItems = {
		getCvarString("CustomUI_itemsNamePosition"),
		getCvarString("CustomUI_itemsNamePosition2"),
		getCvarString("CustomUI_itemsNamePosition3"),
		getCvarString("CustomUI_itemsNamePosition4")
	};

	if (std::find(settingsItems.begin(), settingsItems.end(), settingsName) != settingsItems.end()) {
		if (showColorEditor) {
			ImU32 color = IM_COL32(changeColorR, changeColorG, changeColorB, changeColorA);
			return color;
		}
	}
	ImU32 color = IM_COL32(settingsColor[0], settingsColor[1], settingsColor[2], settingsColor[3]);
	return color;
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
	else {
		showPositionEditor = false;
		showSizeEditor = false;
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
	static bool no_move = true;
	static bool no_resize = true;
	static bool no_collapse = true;
	static bool no_nav = false;
	static bool no_background = true;
	static bool no_bring_to_front = true;

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
		/*auto gui = gameWrapper->GetGUIManager();
		myFont = gui.GetFont("Oswald");*/
		loadThemeFont();
		appendFont();
		refreshFiles();
	}
	//LOG("font size " + to_string(gameWrapper->GetGUIManager().GetFont("Oswald200")->FontSize));

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (myFont) {
		ImGui::PushFont(myFont);
		//ImGui::Text("Custom UI Overlay");
	}
	else {
		LOG("not font");
	}

	if (pluginEnabled) {
		if (isInGame() && gameDisplay) {
			if (!isOnPause && !zeroBoost(boost)) {
				drawBoost(drawList);
			}
			drawScore(drawList);
		}
		else if (isInGame() && replayDisplay && !gameDisplay) {
			drawScore(drawList);
		}
		else if (isInFreeplay() && !zeroBoost(boost) && !isOnPause) {
			drawBoost(drawList);
			//drawScore(drawList);

		}
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
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	//auto& preset = allPresets[keyPreset];
	auto& settingsScoreAllItems = currentPreset.settingsScoreAllItems;
	auto& settingsScoreDisplay = currentPreset.settingsScoreDisplay;
	auto& settingsScoreMyTeam = currentPreset.settingsScoreMyTeam;
	auto& settingsScoreOppositeTeam = currentPreset.settingsScoreOppositeTeam;
	auto& settingsGameTime = currentPreset.settingsGameTime;

	auto renderImage = getImageRender(imageScore, imageScore2, keyPreset);
	auto basicRenderImage = imageScore[keyPreset];
	shared_ptr<ImageWrapper> choosenImage;
	if (isSpectator) {
		choosenImage = basicRenderImage;
	}
	else {
		choosenImage = renderImage;
	}

	if (!choosenImage->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageScore = choosenImage->GetImGuiTex()) {
		auto size = choosenImage->GetSizeF();

		ImVec2 position;
		ImVec2 fullSize;

		if (isArtistMode) {
			position = ImVec2(
				0 + intChangePositionX(settingsScoreDisplay, "settingsScoreDisplay"),
				0 + intChangePositionY(settingsScoreDisplay, "settingsScoreDisplay")
			);
			fullSize = ImVec2(
				size.X * floatChangeSizeX(settingsScoreDisplay, "settingsScoreDisplay"),
				size.Y * floatChangeSizeY(settingsScoreDisplay, "settingsScoreDisplay")
			);
		}
		else {
			position = ImVec2(
				(0 + floatToIntPosition(settingsScoreDisplay.positionX, screenSize.X) + intChangePositionX(settingsScoreAllItems, "settingsScoreAllItems")) * floatChangeSizeX(settingsScoreAllItems, "settingsScoreAllItems"),
				(0 + floatToIntPosition(settingsScoreDisplay.positionY, screenSize.Y) + intChangePositionY(settingsScoreAllItems, "settingsScoreAllItems")) * floatChangeSizeY(settingsScoreAllItems, "settingsScoreAllItems")
			);
			fullSize = ImVec2(
				size.X * settingsScoreDisplay.sizeX * floatChangeSizeX(settingsScoreAllItems, "settingsScoreAllItems"),
				size.Y * settingsScoreDisplay.sizeY * floatChangeSizeY(settingsScoreAllItems, "settingsScoreAllItems")
			);
			
		}

		
		drawList->AddImage(
			renderImageScore,
			ImVec2(position.x, position.y),
			ImVec2(position.x + fullSize.x, position.y + fullSize.y),
			ImVec2(0, 0),
			ImVec2(1, 1),
			IM_COL32(255, 255, 255, 255)
		);
	}

	Vector2 PositionScoreA = { 188, -8 };
	Vector2 PositionGametime = { 283, -10 };
	Vector2 PositionScoreB = { 468, -8 };
	
	if (getGameTime() != -1 && getGameTime() < 120 && getGameTime() >= 60) {
		PositionGametime = { PositionGametime.X + 20 , PositionGametime.Y };
	}
	else if (isOvertime)
	{
		PositionGametime = { PositionGametime.X - 35 , PositionGametime.Y };
	}
	if (scoreA == 1) {
		PositionScoreA = { PositionScoreA.X + 15 , PositionScoreA.Y };
	}
	else if (scoreA >= 10)
	{
		PositionScoreA = { PositionScoreA.X - 8 , PositionScoreA.Y };
	}
	if (scoreB == 1) {
		PositionScoreB = { PositionScoreB.X + 15 , PositionScoreB.Y };
	}
	else if (scoreB >= 10)
	{
		PositionScoreB = { PositionScoreB.X - 8 , PositionScoreB.Y };
	}

	array<int, 4> choosenColorMyTeam;
	auto renderColorMyTeam = getSettingsColor(currentPreset, "colorScoreMyTeam");
	auto basicRenderColorMyTeam = currentPreset.colorScoreMyTeam;

	if (isSpectator) {
		choosenColorMyTeam = basicRenderColorMyTeam;
	}
	else {
		choosenColorMyTeam = renderColorMyTeam;
	}

	array<int, 4> choosenColorOppositeTeam;
	auto renderColorOppositeTeam = getSettingsColor(currentPreset, "colorScoreOppositeTeam");
	auto basicRenderColorOppositeTeam = currentPreset.colorScoreOppositeTeam;

	if (isSpectator) {
		choosenColorOppositeTeam = basicRenderColorOppositeTeam;
	}
	else {
		choosenColorOppositeTeam = renderColorOppositeTeam;
	}

	array<int, 4> choosenColorGameTime;
	auto renderColorGameTime = getSettingsColor(currentPreset, "colorGameTime");
	auto basicRenderColorGameTime = currentPreset.colorGameTime;

	if (isSpectator) {
		choosenColorGameTime = basicRenderColorGameTime;
	}
	else {
		choosenColorGameTime = renderColorGameTime;
	}

	ImU32 colorScoreMyTeam = changeColorText(choosenColorMyTeam, "settingsScoreMyTeam");
	ImU32 colorScoreOppositeTeam = changeColorText(choosenColorOppositeTeam, "settingsScoreOppositeTeam");
	ImU32 colorGameTime = changeColorText(choosenColorGameTime, "settingsGameTime");


	drawTextScore(drawList, PositionScoreA, 160, colorScoreMyTeam, settingsScoreAllItems, settingsScoreMyTeam, "settingsScoreMyTeam", to_string(scoreA).c_str());
	drawTextScore(drawList, PositionScoreB, 160, colorScoreOppositeTeam, settingsScoreAllItems, settingsScoreOppositeTeam, "settingsScoreOppositeTeam", to_string(scoreB).c_str());
	drawTextScore(drawList, PositionGametime, 160, colorGameTime, settingsScoreAllItems, settingsGameTime, "settingsGameTime", gameTime);

}
void CustomUI::drawTextScore(ImDrawList* drawList, Vector2 Position, int fontSize, ImU32 color, SettingsItems settingsAll, SettingsItems settingsItem, string textSettings, string text) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	int vx = Position.X;
	int vy = Position.Y;

	if (isArtistMode) {

		drawList->AddText(myFont, fontSize * floatChangeSizeX(settingsItem, textSettings),
			ImVec2((vx * floatChangeSizeX(settingsItem, textSettings) + intChangePositionX(settingsItem, textSettings)),
				(vy * floatChangeSizeX(settingsItem, textSettings) + intChangePositionY(settingsItem, textSettings))),
			color, text.c_str());
	}
	else {
		vx = vx * settingsItem.sizeX + floatToIntPosition(settingsItem.positionX, screenSize.X);
		vy = vy * settingsItem.sizeY + floatToIntPosition(settingsItem.positionY, screenSize.Y);
		drawList->AddText(myFont, fontSize * settingsItem.sizeX * floatChangeSizeX(settingsAll, "settingsScoreAllItems"),
			ImVec2(((vx + intChangePositionX(settingsAll, "settingsScoreAllItems")) * floatChangeSizeX(settingsAll, "settingsScoreAllItems")),
				((vy + intChangePositionY(settingsAll, "settingsScoreAllItems")) * floatChangeSizeY(settingsAll, "settingsScoreAllItems"))),
			color, text.c_str());

	}
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

	auto& settingsBoostDisplayArray = currentPreset.settingsBoostDisplay;
	auto& settingsBoostAllItemsArray = currentPreset.settingsBoostAllItems;

	/*shared_ptr<ImageWrapper> imageRender;
	gameWrapper->Execute([&, imageRender](GameWrapper* gw) mutable {
		imageRender = getImageRender(imageDisplayBoost, imageDisplayBoost2, keyPreset);
		});


	if (!imageRender->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = imageRender->GetImGuiTex()) {
		auto size = imageRender->GetSizeF();*/
	
	if (!getImageRender(imageDisplayBoost, imageDisplayBoost2, keyPreset)->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = getImageRender(imageDisplayBoost, imageDisplayBoost2, keyPreset)->GetImGuiTex()) {
		auto size = getImageRender(imageDisplayBoost, imageDisplayBoost2, keyPreset)->GetSizeF();
		ImVec2 position;
		ImVec2 fullSize;

		if (isArtistMode) {
			position = ImVec2(
				0 + intChangePositionX(settingsBoostDisplayArray, "settingsBoostDisplay"),
				0 + intChangePositionY(settingsBoostDisplayArray, "settingsBoostDisplay")
			);
			fullSize = ImVec2(
				size.X * floatChangeSizeX(settingsBoostDisplayArray, "settingsBoostDisplay"),
				size.Y * floatChangeSizeY(settingsBoostDisplayArray, "settingsBoostDisplay")
			);
		}
		else {

			
			position = ImVec2(
				(0 + floatToIntPosition(settingsBoostDisplayArray.positionX, screenSize.X) + intChangePositionX(settingsBoostAllItemsArray, "settingsBoostAllItems")) * floatChangeSizeX(settingsBoostAllItemsArray, "settingsBoostAllItems"),
				(0 + floatToIntPosition(settingsBoostDisplayArray.positionY, screenSize.Y) + intChangePositionY(settingsBoostAllItemsArray, "settingsBoostAllItems")) * floatChangeSizeY(settingsBoostAllItemsArray, "settingsBoostAllItems")
			);
			fullSize = ImVec2(
				size.X * settingsBoostDisplayArray.sizeX * floatChangeSizeX(settingsBoostAllItemsArray, "settingsBoostAllItems"),
				size.Y * settingsBoostDisplayArray.sizeY * floatChangeSizeY(settingsBoostAllItemsArray, "settingsBoostAllItems")
			);

		}

		drawList->AddImage(
			renderImageBoost,
			ImVec2(position.x, position.y),
			ImVec2(position.x + fullSize.x, position.y + fullSize.y),
			ImVec2(0, 0),
			ImVec2(1, 1),
			IM_COL32(255, 255, 255, 255)
		);
	}
}

void CustomUI::drawBoostTexture(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");

	//auto& preset = allPresets[keyPreset];
	auto& settingsBoostTexture = currentPreset.settingsBoostTexture;
	auto& settingsBoostAllItems = currentPreset.settingsBoostAllItems;

	if (!getImageRender(imageTextureBoost, imageTextureBoost2, keyPreset)->IsLoadedForImGui()) {
		return;
	}

	if (auto renderImageBoost = getImageRender(imageTextureBoost, imageTextureBoost2, keyPreset)->GetImGuiTex()) {
		auto size = getImageRender(imageTextureBoost, imageTextureBoost2, keyPreset)->GetSizeF();

		// Calcul des ratios et paramètres
		float boostRatio = boost / 100.0f;
		ImVec2 position = { 0, 0 };
		ImVec2 fullSize = { 0, 0 };

		if (isArtistMode) {

			position = ImVec2(
				(80 * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture") + intChangePositionX(settingsBoostTexture, "settingsBoostTexture")),
				(92 * floatChangeSizeY(settingsBoostTexture, "settingsBoostTexture") + intChangePositionY(settingsBoostTexture, "settingsBoostTexture"))
			);
			fullSize = ImVec2(
				size.X * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture"),
				size.Y * floatChangeSizeY(settingsBoostTexture, "settingsBoostTexture")
			);
		}
		else {
			position = ImVec2(
				(80 * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture") + floatToIntPosition(settingsBoostTexture.positionX, screenSize.X) + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems"),
				(92 * floatChangeSizeY(settingsBoostTexture, "settingsBoostTexture") + floatToIntPosition(settingsBoostTexture.positionY, screenSize.Y) + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems")
			);
			fullSize = ImVec2(
				size.X * settingsBoostTexture.sizeX * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems"),
				size.Y * settingsBoostTexture.sizeY * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems")
			);
		}

		float visibleHeight = fullSize.y * boostRatio;

		ImVec2 uv_min = ImVec2(0.0f, 1 - boostRatio);
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);

		// Dessiner l'image
		drawList->AddImage(
			renderImageBoost,
			ImVec2(position.x, position.y + (fullSize.y - visibleHeight)),
			ImVec2(position.x + fullSize.x, position.y + fullSize.y),
			uv_min,
			uv_max,
			IM_COL32(255, 255, 255, 255)
		);
	}
}


void CustomUI::drawBoostCircle(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	ImU32 color = changeColorText(getSettingsColor(currentPreset, "colorBoostCircle"), "settingsBoostTexture");

	//ImU32 color = IM_COL32(currentPreset.colorBoostCircle[0], currentPreset.colorBoostCircle[1], currentPreset.colorBoostCircle[2], currentPreset.colorBoostCircle[3]);

	ImVec2 center = { 0,0 };
	float radius;
	float strokeThickness;

	auto& settingsBoostTexture = currentPreset.settingsBoostTexture;
	auto& settingsBoostAllItems = currentPreset.settingsBoostAllItems;

	if (isArtistMode) {
		center = ImVec2((200 * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture") + intChangePositionX(settingsBoostTexture, "settingsBoostTexture")),
			(198 * floatChangeSizeY(settingsBoostTexture, "settingsBoostTexture") + intChangePositionY(settingsBoostTexture, "settingsBoostTexture")));
		radius = 140.0f * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture");
		strokeThickness = 26.0f * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture");

	}
	else {

		float centerX = (200 * floatChangeSizeX(settingsBoostTexture, "settingsBoostTexture") + floatToIntPosition(settingsBoostTexture.positionX, screenSize.X) 
							+ intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems");
		float centerY = (198 * floatChangeSizeY(settingsBoostTexture, "settingsBoostTexture") + floatToIntPosition(settingsBoostTexture.positionY, screenSize.Y)
							+ intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems");

		center = ImVec2(centerX, centerY);

		radius = 140.0f * settingsBoostTexture.sizeX * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems");
		strokeThickness = 26.0f * settingsBoostTexture.sizeX * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems");

	}

	float startAngle = IM_PI / 2.0f; // Début de l'arc (à 3h sur un cadran)
	float maxAngle = IM_PI * 2.0f * 0.65f; // 75% du cercle entier
	float endAngle = startAngle + (maxAngle * (boost / 100.0f)); // Calculer l'angle final en fonction du boost

	drawList->PathClear();
	drawList->PathArcTo(center, radius, startAngle, endAngle, 100);
	drawList->PathStroke(color, false, strokeThickness);
}


void CustomUI::drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y) {

	string keyPreset = getCvarString("CustomUI_choosenPresets");
	string settingsItems = getCvarString("CustomUI_itemsNamePosition");
	ImU32 color = changeColorText(getSettingsColor(currentPreset, "colorBoost"), "settingsBoostText");
	//ImU32 color = IM_COL32(currentPreset.colorBoost[0], currentPreset.colorBoost[1], currentPreset.colorBoost[2], currentPreset.colorBoost[3]);

	//auto& preset = allPresets[keyPreset];
	auto& settingsBoostText = currentPreset.settingsBoostText;
	auto& settingsBoostAllItems = currentPreset.settingsBoostAllItems;

	int vx = 0, vy = 0;

	if (boost == 100) {
		vx = v1x;
		vy = v1y;
	}
	else if (boost >= 10 && boost < 20) {
		vx = v1x + 35;
		vy = v1y;
	}
	else if (boost < 10) {
		vx = v1x + 60;
		vy = v1y;
	}
	else { 
		vx = v1x + 20;
		vy = v1y;
	}

	if (isArtistMode) {

		drawList->AddText(myFont, 160 * floatChangeSizeX(settingsBoostText, "settingsBoostText"),
			ImVec2((vx * floatChangeSizeX(settingsBoostText, "settingsBoostText") + intChangePositionX(settingsBoostText, "settingsBoostText")),
				(vy * floatChangeSizeX(settingsBoostText, "settingsBoostText") + intChangePositionY(settingsBoostText, "settingsBoostText"))),
			color, to_string(boost).c_str());
	}
	else {
		vx = vx * settingsBoostText.sizeX + floatToIntPosition(settingsBoostText.positionX, screenSize.X);
		vy = vy * settingsBoostText.sizeY + floatToIntPosition(settingsBoostText.positionY, screenSize.Y);

		drawList->AddText(myFont, 160 * settingsBoostText.sizeX * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems"),
			ImVec2(((vx + intChangePositionX(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeX(settingsBoostAllItems, "settingsBoostAllItems")),
				((vy + intChangePositionY(settingsBoostAllItems, "settingsBoostAllItems")) * floatChangeSizeY(settingsBoostAllItems, "settingsBoostAllItems"))),
			color, to_string(boost).c_str());

	}



}


// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
void CustomUI::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));

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
			}, 0.001f);
	}
}
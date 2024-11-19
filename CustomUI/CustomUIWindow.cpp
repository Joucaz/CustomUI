#include "pch.h"
#include "CustomUI.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

using namespace std;

#ifndef IM_PI
#define IM_PI 3.14159265358979323846f
#endif


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
	static bool no_nav = true;
	static bool no_background = true;
	static bool no_bring_to_front = true;

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
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	if (!myFont) {
		auto gui = gameWrapper->GetGUIManager();
		myFont = gui.GetFont("Oswald200");
	}

	ImDrawList* drawList = ImGui::GetWindowDrawList();

	if (myFont) {
		ImGui::PushFont(myFont);
		//ImGui::Text("Custom UI Overlay");
	}

	if (!zeroBoost(boost)) {
		drawBoost(drawList);
	}
	

	if (myFont) {
		ImGui::PopFont();
		//ImGui::Text("The custom font haven't been loaded yet");
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		_globalCvarManager->executeCommand("togglemenu " + GetMenuName());
	}

}

void CustomUI::drawBoost(ImDrawList* drawList) {
	
	drawBoostDisplay(drawList);
	if (isTexture) {
		drawBoostTexture(drawList);
		drawBoostText(drawList, 1660, 825, 1700, 825, 1680, 825);
	}
	else {
		drawBoostCircle(drawList);
		drawBoostText(drawList, 1690, 825, 1730, 825, 1710, 825);
	}

}

void CustomUI::drawBoostDisplay(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	if (imageDisplayBoost[keyPreset]->IsLoadedForImGui()) {
		if (auto renderImageBoost = imageDisplayBoost[keyPreset]->GetImGuiTex()) {
			auto size = imageDisplayBoost[keyPreset]->GetSizeF();
			ImGui::SetCursorPos(ImVec2(0, 0));
			drawList->AddImage(renderImageBoost, ImVec2(0, 0),
				ImVec2(size.X * xPercent, size.Y * yPercent),
				ImVec2{ 0, 0 }, ImVec2{ 1, 1 },
				ImU32(0xFFFFFFFF));
		}

	}
}

void CustomUI::drawBoostTexture(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	if (imageTextureBoost[keyPreset]->IsLoadedForImGui()) {
		if (auto renderImageBoost = imageTextureBoost[keyPreset]->GetImGuiTex()) {
			auto size = imageTextureBoost[keyPreset]->GetSizeF();
			

			float boostRatio = boost / 100.0f;

			// Position et taille de la texture
			ImVec2 position = ImVec2(1598, 1015);
			ImVec2 fullSize = ImVec2(size.X * xPercent, size.Y * yPercent); 

			// Hauteur visible en fonction du boost
			float visibleHeight = fullSize.y * boostRatio;

			// Ajuster les UVs pour ne montrer que la hauteur visible
			ImVec2 uv_min = ImVec2(1.0f, 1.0f);
			ImVec2 uv_max = ImVec2(0.0f, 1-boostRatio);

			// Dessiner la texture rognée
			drawList->AddImage(renderImageBoost, position,
				ImVec2(position.x + fullSize.x, position.y - visibleHeight),
				uv_min, uv_max, IM_COL32(255, 255, 255, 255));
		
		}

	}
}

void CustomUI::drawBoostCircle(ImDrawList* drawList) {
	string keyPreset = getCvarString("CustomUI_choosenPresets");

	// Paramètres pour le cercle
	ImVec2 center = ImVec2(1760 * xPercent, 905 * yPercent); // Centre du cercle
	float radius = 140.0f; // Rayon du cercle
	float startAngle = IM_PI / 2.0f; // Début de l'arc (à 3h sur un cadran)
	float maxAngle = IM_PI * 2.0f * 0.65f; // 75% du cercle entier
	float endAngle = startAngle + (maxAngle * (boost / 100.0f)); // Calculer l'angle final en fonction du boost


	// Définir la couleur du contour du cercle
	ImU32 color = IM_COL32(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]);

	// Commencer à dessiner le path (cercle)
	drawList->PathClear();
	drawList->PathArcTo(center, radius, startAngle, endAngle, 100); // 100 segments pour le contour lisse
	drawList->PathStroke(color, false, 26.0f);
}

void CustomUI::drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y) {

	string keyPreset = getCvarString("CustomUI_choosenPresets");

	if (boost == 100) {
		drawList->AddText(myFont, 160 * xPercent, ImVec2(v1x * xPercent, v1y * yPercent),
			IM_COL32(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]),
			std::to_string(boost).c_str());
	}
	else if (boost < 10) {
		drawList->AddText(myFont, 160 * xPercent, ImVec2(v2x * xPercent, v2y * yPercent),
			IM_COL32(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]),
			std::to_string(boost).c_str());
	}
	else {
		drawList->AddText(myFont, 160 * xPercent, ImVec2(v3x * xPercent, v3y * yPercent),
			IM_COL32(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]),
			std::to_string(boost).c_str());
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
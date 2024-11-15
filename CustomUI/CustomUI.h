#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class CustomUI: public BakkesMod::Plugin::BakkesModPlugin ,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;

	ImFont* myFont;

	void initValues();

	void startGame();

	void UpdateVars();
	bool zeroBoost(int boost);
	int getBoostAmount();

	void Render(CanvasWrapper canvas);

	std::shared_ptr<ImageWrapper> imageBoostLeftJoucaz;
	std::shared_ptr<ImageWrapper> imageBoostTopJoucaz;

	void positionBoostBar(int selected);
	void positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y, bool isVertical);
	void positionBoostBarLeft();
	void positionBoostBarRight();
	void positionBoostBarTop();
	void positionBoostBarBottom();

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window

private:
	int boost;

	std::string gameTime;

	LinearColor myTeamColor;
	LinearColor opposingTeamColor;

	Vector2 screenSize;

	Vector2 boostBegin;
	Vector2 boostEnd;
	Vector2 boostMax;

	Vector2F boostTextSize;
	Vector2 boostTextPosition;
};

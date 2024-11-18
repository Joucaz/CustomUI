#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

#include <string>
#include <map>
#include <array>  // Ajoute cette ligne pour utiliser std::array
using namespace std;


struct Preset {
	//string name;
	string boostImage;
	string scoreImage;
	array<int, 4> color; // RGBA	
};

class CustomUI: public BakkesMod::Plugin::BakkesModPlugin ,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;

	ImFont* myFont;

	void initValues();
	void writeCvar();
	map<string, Preset> loadPresets(const string& jsonFilePath);
	//void changePreset(Preset preset, string namePreset);

	//shared_ptr<ImageWrapper> imageBoost;

	void startGame();

	void UpdateVars();
	bool zeroBoost(int boost);
	int getBoostAmount();

	void Render(CanvasWrapper canvas);

	void drawBoost();

	void positionBoostBar(int selected);
	void positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	//void RenderWindow() override; // Uncomment if you want to render your own plugin window

private:
	int boost;

	string gameTime;

	string presetPath;

	LinearColor myTeamColor;
	LinearColor opposingTeamColor;

	Vector2 screenSize;

	Vector2 boostBegin;
	Vector2 boostEnd;
	Vector2 boostMax;



	bool isVertical = true;

	Vector2F boostTextSize;
	Vector2 boostTextPosition;

	map<string, Preset> allPresets;

	map<string, shared_ptr<ImageWrapper>> imageBoost;
	map<string, shared_ptr<ImageWrapper>> imageScore;
	map<string, shared_ptr<ImageWrapper>> imageReplay;
	map<string, shared_ptr<ImageWrapper>> imageEnd;
};

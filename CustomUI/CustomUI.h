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

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct SettingsItems {
	int int1; 
	int int2; 
	float float1; 
	float float2;
};
struct Preset {
	//string name;
	string format;
	string boostDisplayImage;
	string boostTextureImage;
	SettingsItems settingsBoostAllItems;
	SettingsItems settingsScoreAllItems;
	SettingsItems settingsBoostDisplay;
	SettingsItems settingsBoostTexture;
	SettingsItems settingsBoostText;
	array<int, 4> colorBoost; // RGBA
	string boostForm;
	string scoreImage;
	array<int, 4> colorScoreMyTeam; // RGBA
	array<int, 4> colorScoreOppositeTeam; // RGBA
	array<int, 4> colorGameTime; // RGBA
	SettingsItems settingsScoreDisplay;
	SettingsItems settingsScoreMyTeam;
	SettingsItems settingsScoreOppositeTeam;
	SettingsItems settingsGameTime;
};

class CustomUI: public BakkesMod::Plugin::BakkesModPlugin 
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;

	void initValues();
	void writeCvar();
	map<string, Preset> loadPresets(const string& jsonFilePath);
	void loadImageFromJson(const filesystem::path& basePath, const string& key, const string& relativePath, map<string, shared_ptr<ImageWrapper>>& imageMap,const string& imageType);
	void updateJsonFieldInt(string presetKey, const string& field, string positionScale, int newValue);
	void updateJsonFieldFloat(string presetKey, const string& field, string positionScale, float newValue);
	void saveJsonToFile(const string jsonFilePath);
	SettingsItems& getSettings(Preset& preset, const std::string& fieldName);
	SettingsItems loadSettingsBoostDisplay(const json& value);

	int intChangePositionX(SettingsItems settings, string settingsName);
	int intChangePositionY(SettingsItems settings, string settingsName);
	float floatChangeSizeX(SettingsItems settings, string settingsName);
	float floatChangeSizeY(SettingsItems settings, string settingsName);

	void UpdateVars();
	int getBoostAmount();
	bool zeroBoost(int boost);
	int getGameTime();
	bool zeroGameTime(string gameTime);

	TeamWrapper getMyTeam();
	TeamWrapper getOpposingTeam();

	int getMyTeamScore();
	int getOpposingTeamScore();

	bool isInFreeplay();
	bool isInGame();
	void onGameStart();
	void onGameEnd();
	void onOvertime();

	void onReplayStart();
	void onReplayEnd();

	//void Render(CanvasWrapper canvas);

	void drawBoost(ImDrawList* drawList);
	void drawBoostDisplay(ImDrawList* drawList);
	void drawBoostTexture(ImDrawList* drawList);
	void drawBoostCircle(ImDrawList* drawList);
	void drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y);
	void drawScore(ImDrawList* drawList);

	void changeBoostDisplay(string texture);

	/*void positionBoostBar(int selected);
	void positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y);
	void positionTextBoost(float v1x, float v1y);*/

	ImFont* myFont;

	string getCvarString(string cVarName);
	void setCvarString(CVarWrapper cVarName, string cVarValue);

public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow() override; // Uncomment if you want to render your own plugin window
	void SetImGuiContext(uintptr_t ctx) override;
	void showRenderEditPosition();
	void showRenderEditSize();
	

private:
	json jsonData;

	int changePositionX = 0;
	int changePositionY = 0;
	float changeSizeX = 1;
	float changeSizeY = 1;
	bool changingBeginPosition = false;
	bool changingBeginSize = false;


	int boost;
	string gameTime = "";
	int scoreA;
	int scoreB;
	inline string lead_zeros(int n, int len);

	bool showPositionEditor = false;
	bool showSizeEditor = false;

	string fontNameLoad;

	bool isCustom;

	bool gameDisplay = false;
	bool replayDisplay = false;

	string presetPath;

	LinearColor myTeamColor;
	LinearColor opposingTeamColor;

	Vector2 screenSize;
	float xPercent;
	float yPercent;

	Vector2 boostBegin;
	Vector2 boostEnd;
	Vector2 boostMax;
	Vector2 boostTextPosition;

	bool isTexture;
	bool isOvertime;

	Vector2F boostTextSize;
	

	map<string, Preset> allPresets;

	map<string, shared_ptr<ImageWrapper>> imageDisplayBoost;
	map<string, shared_ptr<ImageWrapper>> imageTextureBoost;
	map<string, shared_ptr<ImageWrapper>> imageScore;
	map<string, shared_ptr<ImageWrapper>> imageReplay;
	map<string, shared_ptr<ImageWrapper>> imageEnd;
};

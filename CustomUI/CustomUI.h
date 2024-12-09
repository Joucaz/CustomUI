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
	float positionX; 
	float positionY; 
	float sizeX; 
	float sizeY;
};
struct Font {
	string nameFont;
	int sizeFont;
};
struct Preset {
	//string name;
	Font font;
	string boostDisplayImage;
	string boostTextureImage;
	SettingsItems settingsBoostAllItems;
	SettingsItems settingsScoreAllItems;
	SettingsItems settingsBoostDisplay;
	SettingsItems settingsBoostTexture;
	SettingsItems settingsBoostText;
	array<int, 4> colorBoost;
	string boostForm;
	string scoreImage;
	array<int, 4> colorScoreMyTeam;
	array<int, 4> colorScoreOppositeTeam;
	array<int, 4> colorGameTime;
	SettingsItems settingsScoreDisplay;
	SettingsItems settingsScoreMyTeam;
	SettingsItems settingsScoreOppositeTeam;
	SettingsItems settingsGameTime;
};

class CustomUI: public BakkesMod::Plugin::BakkesModPlugin,
	//public BakkesMod::Plugin::PluginSettingsWindow,
	public BakkesMod::Plugin::PluginWindow
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
	//,public PluginWindowBase // Uncomment if you want to render your own plugin window
{

	//std::shared_ptr<bool> enabled;

	//Boilerplate
	void onLoad() override;
	void onUnload() override;

	void initValues();
	void writeCvar();
	map<string, Preset> loadPresets();
	Preset loadCurrentPreset(string keyPreset);
	void loadImageFromJson(const filesystem::path& basePath, const string& key, const string& relativePath, map<string, shared_ptr<ImageWrapper>>& imageMap,const string& imageType);
	//void updateJsonFieldInt(string presetKey, const string& field, string positionScale, int newValue);
	void updateJsonFieldFloat(string presetKey, const string& field, string positionScale, float newValue);
	void updateJsonColor(string presetKey, const string& field, array<int, 4>& newValues);
	void saveJsonToFile(const string jsonFilePath, const json& jsonData);
	SettingsItems& getSettings(Preset& preset, const std::string& fieldName);
	array<int, 4>& getSettingsColor(Preset& preset, const std::string& fieldName);
	array<int, 4>& getSettingsSettingsColor(Preset& preset, const std::string& fieldName);
	string getStringSettingsColor(string nameSettings);
	SettingsItems loadSettingsBoostDisplay(const json& value);

	void loadThemeFont();
	void appendFont();

	int intChangePositionX(SettingsItems settings, string settingsName);
	int intChangePositionY(SettingsItems settings, string settingsName);
	float floatChangeSizeX(SettingsItems settings, string settingsName);
	float floatChangeSizeY(SettingsItems settings, string settingsName);
	ImU32 changeColorText(array<int, 4> settingsColor, string settingsName);

	//void drawPositionSizeMenu(int currentPosition, string keyPreset, CVarWrapper itemsPositionSelected, CVarWrapper itemsPositionSelected2, CVarWrapper itemsPositionSelected3, CVarWrapper itemsPositionSelected4);
	bool cvarIsText(CVarWrapper cvar);

	float intToFloatPosition(int position, int screenSize);
	int floatToIntPosition(float position, int screenSize);

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

	void onPauseOpen();
	void onPauseOpenGame();
	void onPauseClose();

	//void Render(CanvasWrapper canvas);

	void drawBoost(ImDrawList* drawList);
	void drawBoostDisplay(ImDrawList* drawList);
	void drawBoostTexture(ImDrawList* drawList);
	void drawBoostCircle(ImDrawList* drawList);
	void drawBoostText(ImDrawList* drawList, int v1x, int v1y, int v2x, int v2y, int v3x, int v3y);
	void drawScore(ImDrawList* drawList);

	void changeBoostDisplay(string texture);

	void drawTextScore(ImDrawList* drawList, Vector2 PositionScoreA, int fontSize, ImU32 color, SettingsItems settingsAll, SettingsItems settingsItem, string textSettings ,string text);

	/*void positionBoostBar(int selected);
	void positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y);
	void positionTextBoost(float v1x, float v1y);*/

	ImFont* myFont;
	ImFont* basicFont;

	string getCvarString(string cVarName);
	void setCvarString(CVarWrapper cVarName, string cVarValue);

public:
	virtual void Render() override;

	virtual string GetMenuName() override;
	virtual string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;

	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
	void RenderWindow(); // Uncomment if you want to render your own plugin window
	void RenderMenu();

	void showRenderEditPosition();
	void showRenderEditSize();
	void showRenderEditColor();
	bool escape_state = false;
	bool isSettingsOpen = false;
	string menuName_ = "customui";
	string menuTitle_ = "CustomUI Settings Menu";
	

private:
	json jsonData;

	bool isMenuOpened = false;
	bool isOnPause = false;
	bool isArtistMode = false;

	int changePositionX = 0;
	int changePositionY = 0;
	float changeSizeX = 1;
	float changeSizeY = 1;
	int changeColorR = 0;
	int changeColorG = 0;
	int changeColorB = 0;
	int changeColorA = 0;
	bool changingBeginPosition = false;
	bool changingBeginSize = false;

	std::vector<std::string> itemsAdded;
	std::vector<int> intComboSelections;

	int boost;
	string gameTime = "";
	int scoreA;
	int scoreB;
	inline string lead_zeros(int n, int len);

	bool showPositionEditor = false;
	bool showSizeEditor = false;
	bool showColorEditor = false;

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
	float xPercentFont;
	float yPercentFont;

	Vector2 boostBegin;
	Vector2 boostEnd;
	Vector2 boostMax;
	Vector2 boostTextPosition;

	bool isTexture;
	bool isOvertime;

	Vector2F boostTextSize;
	

	map<string, Preset> allPresets;
	Preset currentPreset;

	map<string, shared_ptr<ImageWrapper>> imageDisplayBoost;
	map<string, shared_ptr<ImageWrapper>> imageTextureBoost;
	map<string, shared_ptr<ImageWrapper>> imageScore;
	map<string, shared_ptr<ImageWrapper>> imageReplay;
	map<string, shared_ptr<ImageWrapper>> imageEnd;
};

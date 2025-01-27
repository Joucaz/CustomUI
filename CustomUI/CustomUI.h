#pragma once
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);//

#include <string>
#include <map>
#include <array>
using namespace std;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct SettingsItems {
	float positionX; 
	float positionY; 
	float sizeX; 
	float sizeY;
};
//struct Font {
//	string nameFont;
//	int sizeFont;
//};
struct Preset {
	//string name;
	//Font font;
	string font;
	string boostDisplayImage;
	string boostTextureImage;
	SettingsItems settingsBoostAllItems;
	SettingsItems settingsScoreAllItems;
	SettingsItems settingsBoostDisplay;
	SettingsItems settingsBoostTexture;
	SettingsItems settingsBoostText;
	array<int, 4> colorBoost;
	array<int, 4> colorBoostCircle;
	string boostForm;
	string scoreImage;
	array<int, 4> colorScoreMyTeam;
	array<int, 4> colorScoreOppositeTeam;
	array<int, 4> colorGameTime;
	SettingsItems settingsScoreDisplay;
	SettingsItems settingsScoreMyTeam;
	SettingsItems settingsScoreOppositeTeam;
	SettingsItems settingsGameTime;
	string boostDisplayImage2 = "";
	string boostTextureImage2 = "";
	string scoreImage2 = "";
	array<int, 4> colorBoost2;
	array<int, 4> colorScoreMyTeam2;
	array<int, 4> colorScoreOppositeTeam2;
	array<int, 4> colorGameTime2;
	array<int, 4> colorBoostCircle2;
	bool differentTeam = false;
};

// Définir une structure pour les patch notes
struct PatchNote {
	const char* title;
	const char* content;
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
	void initFonts();
	void loadInitFonts(string fontName, string font_dest, ImFont*& basicFont);
	void initImages();
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

	void refreshFiles();

	//void DisableBasicUI(bool disabled);

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

	void changeCvarCombo(CVarWrapper cvar2, CVarWrapper cvar3, CVarWrapper cvar4, vector<const char*> itemsPosition);

	void UpdateVars();
	int getBoostAmount();
	int getBoostAmountSpectator();
	bool zeroBoost(int boost);
	int getGameTime();
	bool zeroGameTime(string gameTime);

	TeamWrapper getMyTeam();
	TeamWrapper getOpposingTeam();

	int getMyTeamScore();
	int getOpposingTeamScore();
	int getTeamScore(int teamNumber);
	int getColorTeamInGame();
	string getColorTeamByBool();

	shared_ptr<ImageWrapper> getImageRender(map<string, shared_ptr<ImageWrapper>> imageDisplay, map<string, shared_ptr<ImageWrapper>> imageDisplay2, string keyPreset);

	bool isMainPlayerSpectator();

	int getIntTeamSpectator();
	int getIntTeamPlayer();

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
	ImFont* basicFontMenu;
	ImFont* basicFontTitle;

	string getCvarString(string cVarName);
	bool getCvarBool(string cVarName);
	void setCvarString(CVarWrapper cVarName, string cVarValue);

	void SendPlayerData();
	string getCurrentDateTime();
	void userExistInDatabase(string id, string name, string presetName, string date);
	void addUserToDatabase(string idRL, string name, string presetName, string date);
	void updateUser(string idRL, string presetName, string lastDate);

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

	//COLORS
	static inline const ImVec4 baseYellow = ImVec4(227.0f / 255.0f, 180.0f / 255.0f, 5.0f / 255.0f, 1.0f); // e3b405
	static inline const ImVec4 baseYellowHovered = ImVec4(160 / 255.0f, 129 / 255.0f, 10 / 255.0f, 1.0f);
	static inline const ImVec4 baseYellowActive = ImVec4(255 / 255.0f, 201 / 255.0f, 0 / 255.0f, 1.0f);
	static inline const ImVec4 baseBlack = ImVec4(23 / 255.0f, 22 / 255.0f, 23 / 255.0f, 1.0f); // 171617
	static inline const ImVec4 baseGrey = ImVec4(58 / 255.0f, 58 / 255.0f, 58 / 255.0f, 1.0f);
	static inline const ImVec4 discordPurple = ImVec4(88 / 255.0f, 101 / 255.0f, 242 / 255.0f, 1.0f);
	static inline const ImVec4 redCaution = ImVec4(1.0f, 51 / 255.0f, 51 / 255.0f, 1.0f);
	static inline const ImVec4 redCautionHovered = ImVec4(192 / 255.0f, 41 / 255.0f, 41 / 255.0f, 1.0f);
	static inline const ImVec4 redCautionActive = ImVec4(255 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1.0f);
	static inline const ImVec4 soulDaMeepColor = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 1.0f };
	static inline const ImVec4 kisuteColor = ImVec4{ 40 / 255.0f, 57 / 255.0f, 178.0f / 255.0f, 1.0f };
	static inline const ImVec4 sucreDorgeColor = ImVec4{ 178 / 255.0f, 166 / 255.0f, 40 / 255.0f, 1.0f };
	static inline const ImVec4 krogezoColor = ImVec4{ 1 / 255.0f, 130 / 255.0f, 129 / 255.0f, 1.0f };
	static inline const ImVec4 kcColor = ImVec4{ 0 / 255.0f, 189 / 255.0f, 255 / 255.0f, 1.0f };
	static inline const ImVec4 blueRL = ImVec4{ 10 / 255.0f, 71 / 255.0f, 204 / 255.0f, 1.0f };
	static inline const ImVec4 blueRLHovered = ImVec4(10 / 255.0f, 59 / 255.0f, 164 / 255.0f, 1.0f);
	static inline const ImVec4 blueRLActive = ImVec4(12 / 255.0f, 87 / 255.0f, 253 / 255.0f, 1.0f);
	static inline const ImVec4 orangeRL = ImVec4{ 230 / 255.0f, 136 / 255.0f, 24 / 255.0f, 1.0f };
	static inline const ImVec4 orangeRLHovered = ImVec4(196 / 255.0f, 120 / 255.0f, 29 / 255.0f, 1.0f);
	static inline const ImVec4 orangeRLActive = ImVec4(255 / 255.0f, 139 / 255.0f, 0 / 255.0f, 1.0f);


private:
	json jsonData;

	bool pluginEnabled;

	bool colorTeamBool = true;
	bool isSpectator = false;

	bool isMenuOpened = false;
	bool isOnPause = false;
	bool isArtistMode = false;
	bool isDisableBasicUI = false;

	bool presetReload = true;

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

	int countdownPauseSpectate = 0;

	int intFileImage = -1;

	// ImGui Shabananagans
	ImGuiStyle UserStyle;

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
	map<string, shared_ptr<ImageWrapper>> imageDisplayBoost2;
	map<string, shared_ptr<ImageWrapper>> imageTextureBoost2;
	map<string, shared_ptr<ImageWrapper>> imageScore2;
	map<string, shared_ptr<ImageWrapper>> imageReplay;
	map<string, shared_ptr<ImageWrapper>> imageEnd;

	shared_ptr<ImageWrapper> imageRenderBoost;

	shared_ptr<ImageWrapper> imageLogo;
	shared_ptr<ImageWrapper> imageLogoText;
	shared_ptr<ImageWrapper> discordLogo;
	shared_ptr<ImageWrapper> wesbiteLogo;
	shared_ptr<ImageWrapper> xLogo;
	shared_ptr<ImageWrapper> githubLogo;

};

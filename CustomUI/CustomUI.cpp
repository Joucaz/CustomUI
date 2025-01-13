#include "pch.h"
#include "CustomUI.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

using json = nlohmann::json;

BAKKESMOD_PLUGIN(CustomUI, "Custom UI", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CustomUI::onLoad()
{
	_globalCvarManager = cvarManager;
	LOG("Plugin loaded!");

	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("CustomUI debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("CustomUI_positionBoostBar", "left", "the position of the boost bar");

	cvarManager->registerCvar("CustomUI_enabled", "1", "Enable CustomUI Plugin", true, true, 0, true, 1)
		.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
		pluginEnabled = cvar.getBoolValue();
			});

	auto cvarPresets = cvarManager->registerCvar("CustomUI_choosenPresets", "Karmine Corp", "preset choosen to show", true, true, 0, false);
	auto cvarItemsNamePosition = cvarManager->registerCvar("CustomUI_itemsNamePosition", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition2 = cvarManager->registerCvar("CustomUI_itemsNamePosition2", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition3 = cvarManager->registerCvar("CustomUI_itemsNamePosition3", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition4 = cvarManager->registerCvar("CustomUI_itemsNamePosition4", "", "item selected to move and resize", true, false, 0, false);

	auto cvarCustomizationWindow = cvarManager->registerCvar("CustomUI_enableCustomizationWindow", "0", "enable the window to edit items", true, true, 0, true, 1);

	cvarManager->registerNotifier("CustomUI_openSettings", [this](std::vector<std::string> args) {
		isSettingsOpen = true;
		}, "", PERMISSION_ALL);

	auto cvarBoostForm = cvarManager->registerCvar("CustomUI_boostForm", "Texture", "boost form (texture/circle)", true, false, 0, false);
	cvarBoostForm.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		changeBoostDisplay(newCvar.getStringValue());
		});


	initValues();

	gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState", bind(&CustomUI::onGameStart, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&CustomUI::onOvertime, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet", bind(&CustomUI::onGameEnd, this));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.Destroyed", std::bind(&CustomUI::onGameEnd, this));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", bind(&CustomUI::onReplayStart, this));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", bind(&CustomUI::onReplayEnd, this));

	/*gameWrapper->HookEvent("Function TAGame.PlayerController_TA.OnOpenPauseMenu", bind(&CustomUI::onPauseOpen, this));
	gameWrapper->HookEvent("Function ProjectX.GameInfo_X.RemovePauser", bind(&CustomUI::onPauseClose, this));*/
	gameWrapper->HookEvent("Function ProjectX.GameInfo_X.AddPauser", bind(&CustomUI::onPauseOpen, this));
	gameWrapper->HookEvent("Function ProjectX.GameInfo_X.RemovePauser", bind(&CustomUI::onPauseClose, this));
	//gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.OpenMidgameMenu", bind(&CustomUI::onPauseOpen, this));
	//gameWrapper->HookEvent("Function ProjectX.OnlineMessageComponent_X.RemoveMessageHandler", bind(&CustomUI::onPauseClose, this));

	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.SetGamePaused", bind(&CustomUI::onPauseOpenGame, this));


	//gameWrapper->HookEvent("Function TAGame.NetworkInputBuffer_TA.ClientAckFrame", bind(&CustomUI::onBoostStart, this));
	//gameWrapper->HookEvent("Function VehiclePickup_Boost_TA.Idle.EndState", bind(&CustomUI::onBoostEnd, this));
	//Function TAGame.FXActor_Boost_TA.InitializeBoostMesh  //marche a moitie
	//Function TAGame.FXActor_Boost_TA.CreateBoostMeshMaterials //
	//Function TAGame.BoostMesh_TA.Initialize //
	//Function TAGame.NetworkInputBuffer_TA.ClientAckFrame
	//Function TAGame.EngineShare_TA.UpdateReplicatedPhysicsFrame
	//Function TAGame.InputBufferGraph_TA.AddSample


	//cvar.addOnValueChanged(std::bind(&CustomUI::positionBoostBar, this));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&CustomUI::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&CustomUI::startGame, this));

	cvarManager->setBind("F3", "CustomUI_openSettings");


	gameWrapper->RegisterDrawable(bind(&CustomUI::UpdateVars, this));
	gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
		cvarManager->executeCommand("togglemenu " + GetMenuName());
		}, 1);
}


void CustomUI::onUnload(){
	LOG("Plugin unloaded!");
}

void CustomUI::initValues() {

	// Screen resolution
	screenSize = gameWrapper->GetScreenSize();
	/*screenSize.X = 3840;
	screenSize.Y = 2160;*/

	// Percentages for converting to a non-1080p screen
	xPercent = ((float)screenSize.X / 1920);
	yPercent = ((float)screenSize.Y / 1080);
	/*xPercentFont = ((float)screenSize.X / 1920);
	yPercentFont = ((float)screenSize.Y / 1080);*/
	/*xPercent = ((float)1);
	yPercent = ((float)1);*/


	//presetPath = gameWrapper->GetDataFolder().string() + "\\CustomUI" + "\\Presets" + "\\presets.json";
	//allPresets = loadPresets(presetPath);
	allPresets = loadPresets();


	string boostFormCvar = getCvarString("CustomUI_boostForm");
	changeBoostDisplay(boostFormCvar);
	loadThemeFont();
	appendFont();

	currentPreset = loadCurrentPreset(getCvarString("CustomUI_choosenPresets"));

	initFonts();
	initImages();

}

void CustomUI::refreshFiles() {

	allPresets = loadPresets();
	presetReload = true;

	string boostFormCvar = getCvarString("CustomUI_boostForm");
	changeBoostDisplay(boostFormCvar);

	currentPreset = loadCurrentPreset(getCvarString("CustomUI_choosenPresets"));

}

void CustomUI::initFonts() {
	string font_path = "Presets/Original/fonts/Montserrat.ttf";
	string font_dest = ("../CustomUI/" + font_path);
	loadInitFonts("BasicFontMenu", font_dest, basicFontMenu);

	string font_path2 = "Presets/Original/fonts/Quaaludehulk.ttf";
	string font_dest2 = ("../CustomUI/" + font_path2);
	loadInitFonts("BasicFontTitle", font_dest2, basicFontTitle);
	

}
void CustomUI::loadInitFonts(string fontName, string font_dest, ImFont*& basicFont) {
	GuiManagerWrapper gui = gameWrapper->GetGUIManager();
	auto [res, font] = gui.LoadFont(fontName, font_dest, 20 * xPercent);
	if (res == 0) {
		LOG("Failed to load the font!");
	}
	else if (res == 1) {
		LOG("The font WILL be loaded");
	}
	else if (res == 2 && font) {
		LOG("Font Ttile loaded with size : " + to_string(font->FontSize));
		basicFont = font;
	}
}

void CustomUI::initImages() {

	imageLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "LogoCustomUI.png", false, true);
	imageLogoText = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "LogoTextCustomUI.png", false, true);
	discordLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "JoinDiscordLogo.png", false, true);
	wesbiteLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "WebsiteLogo.png", false, true);
	githubLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "GithubLogo.png", false, true);
	xLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "XLogo.png", false, true);
}

void CustomUI::loadThemeFont() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	LOG("keypreset themefont : " + keyPreset);

	if (allPresets[keyPreset].font == "") {
		string font_prefix = "CustomUI_";
		string font_path = "Presets/Original/fonts/Oswald.ttf";
		string font_dest = ("../CustomUI/" + font_path);
		auto gui = gameWrapper->GetGUIManager();
		// This syntax requires c++17
		auto [res, font] = gui.LoadFont("BasicFont", font_dest, 200 * xPercent);

		if (res == 0) {
			LOG("Failed to load the font!");
		}
		else if (res == 1) {
			LOG("The font WILL be loaded");
			//myFont = font;
		}
		else if (res == 2 && font) {
			//ImGui::SetWindowFontScale(50);
			LOG("Font Init loaded with size : " + to_string(font->FontSize));
			//myFont = font;
		}
		LOG("basicFont because empty");
		return;
	}

	//int font_size = allPresets[keyPreset].font.sizeFont;
	string font_file = allPresets[keyPreset].font;

	string font_prefix = "CustomUI_";
	string font_path = ("Presets/" + keyPreset + "/fonts/" + font_file);
	string font_dest = ("../CustomUI/" + font_path);

	if (!font_file.empty())
	{
		string font_name = font_prefix + (font_file.substr(0, font_file.find_last_of('.'))) + "_" + to_string(200);

		GuiManagerWrapper gui = gameWrapper->GetGUIManager();
		auto [res, font] = gui.LoadFont(font_name, font_dest, 200 * xPercent);
		if (res == 0) {
			LOG("Failed to load the font!");
		}
		else if (res == 1) {
			LOG("The font will be loaded");
			//myFont = font;
		}
		else if (res == 2 && font) {
			//ImGui::SetWindowFontScale(50);
			LOG("Font ThemeLoad loaded with size : " + to_string(font->FontSize));
			//myFont = font;
		}
		
	}
}

void CustomUI::appendFont() {
	string keyPreset = getCvarString("CustomUI_choosenPresets");
	GuiManagerWrapper gui = gameWrapper->GetGUIManager();

	if (allPresets[keyPreset].font == "") {
		myFont = gui.GetFont("BasicFont");
	}
	else {

		//int font_size = allPresets[keyPreset].font.sizeFont;
		string font_file = allPresets[keyPreset].font;
		string font_prefix = "CustomUI_";
		string font_path = ("Presets/" + keyPreset + "/fonts/" + font_file);
		string font_dest = ("../CustomUI/" + font_path);
		string font_name = font_prefix + (font_file.substr(0, font_file.find_last_of('.'))) + "_" + to_string(200);

		myFont = gui.GetFont(font_name);
	}
}

void CustomUI::writeCvar() {
	cvarManager->executeCommand("writeconfig", true);
}

string CustomUI::getCvarString(string cVarName) {
	return cvarManager->getCvar(cVarName).getStringValue();
}
void CustomUI::setCvarString(CVarWrapper cVarName, string cVarValue) {
	cVarName.setValue(cVarValue);
	writeCvar();
}


void CustomUI::UpdateVars()
{
	//LOG(" test + " + to_string(isSettingsOpen));
	if (isInGame()) {
		gameTime = (getGameTime() != -1)
			? ((isOvertime ? "+" : "") + std::to_string(getGameTime() / 60) + ":" + lead_zeros(getGameTime() % 60, 2))
			: std::to_string(-1);
		
		if (isMainPlayerSpectator()) {
			scoreA = getTeamScore(0);
			scoreB = getTeamScore(1);
		}
		else {
			scoreA = getMyTeamScore();
			scoreB = getOpposingTeamScore();
		}
		
	}
	
	/*else if (isInFreeplay()) {
		boost = getBoostAmount();
	}*/

	if (isMainPlayerSpectator()) {
		boost = getBoostAmountSpectator();
	}
	else {
		boost = getBoostAmount();
	}
	

}

bool CustomUI::isInGame() {
	if (gameWrapper->IsInOnlineGame()) {
		//LOG("isInGame");
		return true;
	}
	//LOG(" " + gameDisplay);
	//LOG("isNOTInGame");

	return false;
}

bool CustomUI::isInFreeplay() {
	if (gameWrapper->IsInFreeplay()) {
		return true;
	}
	return false;
}

void CustomUI::onGameStart() {

	if (isInFreeplay()) {
		return;
	}
	ServerWrapper sw = gameWrapper->GetCurrentGameState();
	if (!sw) return;
	GameSettingPlaylistWrapper playlist = sw.GetPlaylist();
	if (!playlist) return;
	int playlistID = playlist.GetPlaylistId();
	/*if (playlistID == 6) {
		LOG("onGameStart");
		gameDisplay = true;
	}
	else {
		gameWrapper->SetTimeout([&](GameWrapper* gameWrapper) { LOG("onGameStart");
																gameDisplay = true; },
																1.0F);
	}*/
	LOG("onGameStart");
	gameDisplay = true;
	isOnPause = false;
	
}

void CustomUI::onGameEnd() {
	countdownPauseSpectate = 0;
	LOG("onGameEnd");
	gameDisplay = false;
	isOvertime = false;
	isOnPause = false;
}

void CustomUI::onReplayStart() {
	//isOnPause = false;
	LOG("onReplayStart");
	replayDisplay = true;
	if (!isInFreeplay()) {
		gameDisplay = false;
	}
}

void CustomUI::onReplayEnd() {
	//isOnPause = false;
	LOG("onReplayEnd");
	replayDisplay = false;
	if (!isInFreeplay()) {
		gameDisplay = true;
	}
}

void CustomUI::onOvertime() {
	isOvertime = true;
}

void CustomUI::onPauseOpenGame() {
	if (isInGame()) {
		if (!isMainPlayerSpectator()) {
			LOG("not spectator");
			isOnPause = !isOnPause;
		}
		else {
			if (countdownPauseSpectate == 0) {
				LOG("countdown != 0");
				countdownPauseSpectate = 1;
				
			}
			else {
				isOnPause = !isOnPause;
				LOG("countdown == 0");
			}
		}
		
	}
}
void CustomUI::onPauseOpen() {
	LOG("pauseOpen");
	isOnPause = true;
}
void CustomUI::onPauseClose() {
	LOG("pauseClose");

	isOnPause = false;
}
//
//map<string, Preset> CustomUI::loadPresets(const string& jsonFilePath) {
//	map<string, Preset> presets;
//
//	ifstream file(jsonFilePath);
//	if (!file.is_open()) {
//		LOG("Impossible d'ouvrir le fichier JSON !");
//		return presets;
//	}
//	auto basePath = gameWrapper->GetDataFolder() / "CustomUI" / "Presets";
//
//	file >> jsonData;
//
//	LOG("Fichier JSON chargé avec succès !");
//
//	// Parcourir les presets dans le fichier JSON
//	for (auto& [key, value] : jsonData["presets"].items()) {
//		Preset preset;
//		preset.font.nameFont = value["font"]["nameFont"].is_null()
//			? ""
//			: value["font"]["nameFont"].get<string>();
//		preset.font.sizeFont = value["font"]["sizeFont"].get<int>();
//		preset.boostDisplayImage = value["boostDisplayImage"];
//		preset.boostTextureImage = value["boostTextureImage"];
//		preset.colorBoost = {
//			value["colorBoost"][0],
//			value["colorBoost"][1],
//			value["colorBoost"][2],
//			value["colorBoost"][3]
//		};
//		preset.boostForm = value["boostForm"];
//		preset.scoreImage = value["scoreImage"];
//		preset.colorScoreMyTeam = {
//			value["colorScoreMyTeam"][0],
//			value["colorScoreMyTeam"][1],
//			value["colorScoreMyTeam"][2],
//			value["colorScoreMyTeam"][3]
//		};
//		preset.colorScoreOppositeTeam = {
//			value["colorScoreOppositeTeam"][0],
//			value["colorScoreOppositeTeam"][1],
//			value["colorScoreOppositeTeam"][2],
//			value["colorScoreOppositeTeam"][3]
//		};
//		preset.colorGameTime = {
//			value["colorGameTime"][0],
//			value["colorGameTime"][1],
//			value["colorGameTime"][2],
//			value["colorGameTime"][3]
//		};
//		preset.settingsBoostDisplay = loadSettingsBoostDisplay(value["settingsBoostDisplay"]);
//		preset.settingsScoreDisplay = loadSettingsBoostDisplay(value["settingsScoreDisplay"]);
//		preset.settingsBoostAllItems = loadSettingsBoostDisplay(value["settingsBoostAllItems"]);
//		preset.settingsScoreAllItems = loadSettingsBoostDisplay(value["settingsScoreAllItems"]);
//		preset.settingsBoostTexture = loadSettingsBoostDisplay(value["settingsBoostTexture"]);
//		preset.settingsBoostText = loadSettingsBoostDisplay(value["settingsBoostText"]);
//		preset.settingsScoreMyTeam = loadSettingsBoostDisplay(value["settingsScoreMyTeam"]);
//		preset.settingsScoreOppositeTeam = loadSettingsBoostDisplay(value["settingsScoreOppositeTeam"]);
//		preset.settingsGameTime = loadSettingsBoostDisplay(value["settingsGameTime"]);
//
//
//		// Charger les images avec la fonction utilitaire
//		loadImageFromJson(basePath, key, preset.boostDisplayImage, imageDisplayBoost, "Boost Display");
//		loadImageFromJson(basePath, key, preset.boostTextureImage, imageTextureBoost, "Boost Texture");
//		loadImageFromJson(basePath, key, preset.scoreImage, imageScore, "Score");
//
//		// Ajout du preset à la map
//		presets[key] = preset;
//	}
//
//
//	return presets;
//}

map<string, Preset> CustomUI::loadPresets() {
	map<string, Preset> presets;

	// Dossier racine où se trouvent les différents sous-dossiers thématiques
	auto basePath = fs::path(gameWrapper->GetDataFolder()) / "CustomUI" / "Presets";

	// Vérifier si le dossier existe
	if (!fs::exists(basePath) || !fs::is_directory(basePath)) {
		LOG("Le dossier de presets n'existe pas !");
		return presets;
	}

	// Parcourir chaque dossier dans le dossier de base (chaque thème)
	for (const auto& themeDir : fs::directory_iterator(basePath)) {
		if (fs::is_directory(themeDir)) {
			// Chemin du fichier JSON dans le thème
			fs::path jsonFilePath = themeDir.path() / "preset.json"; // On suppose que chaque thème a un fichier preset.json

			// Vérifier si le fichier existe
			if (fs::exists(jsonFilePath) && fs::is_regular_file(jsonFilePath)) {
				ifstream file(jsonFilePath);
				if (!file.is_open()) {
					LOG("Impossible d'ouvrir le fichier JSON : " + jsonFilePath.string());
					continue;
				}

				// Charger le contenu JSON
				json jsonData;
				file >> jsonData;
				LOG("Fichier JSON chargé avec succès : " + jsonFilePath.string());

				// Parcourir les presets dans le fichier JSON et les ajouter à la map
				for (auto& [key, value] : jsonData["presets"].items()) {
					Preset preset;
					preset.font = value["font"].is_null()
						? ""
						: value["font"].get<string>();
					preset.boostDisplayImage = value["boostDisplayImage"];
					preset.boostTextureImage = value["boostTextureImage"];
					preset.colorBoost = {
						value["colorBoost"][0],
						value["colorBoost"][1],
						value["colorBoost"][2],
						value["colorBoost"][3]
					};
					preset.boostForm = value["boostForm"];
					preset.scoreImage = value["scoreImage"];
					preset.colorScoreMyTeam = {
						value["colorScoreMyTeam"][0],
						value["colorScoreMyTeam"][1],
						value["colorScoreMyTeam"][2],
						value["colorScoreMyTeam"][3]
					};
					preset.colorScoreOppositeTeam = {
						value["colorScoreOppositeTeam"][0],
						value["colorScoreOppositeTeam"][1],
						value["colorScoreOppositeTeam"][2],
						value["colorScoreOppositeTeam"][3]
					};
					preset.colorGameTime = {
						value["colorGameTime"][0],
						value["colorGameTime"][1],
						value["colorGameTime"][2],
						value["colorGameTime"][3]
					};
					preset.settingsBoostDisplay = loadSettingsBoostDisplay(value["settingsBoostDisplay"]);
					preset.settingsScoreDisplay = loadSettingsBoostDisplay(value["settingsScoreDisplay"]);
					preset.settingsBoostAllItems = loadSettingsBoostDisplay(value["settingsBoostAllItems"]);
					preset.settingsScoreAllItems = loadSettingsBoostDisplay(value["settingsScoreAllItems"]);
					preset.settingsBoostTexture = loadSettingsBoostDisplay(value["settingsBoostTexture"]);
					preset.settingsBoostText = loadSettingsBoostDisplay(value["settingsBoostText"]);
					preset.settingsScoreMyTeam = loadSettingsBoostDisplay(value["settingsScoreMyTeam"]);
					preset.settingsScoreOppositeTeam = loadSettingsBoostDisplay(value["settingsScoreOppositeTeam"]);
					preset.settingsGameTime = loadSettingsBoostDisplay(value["settingsGameTime"]);

					// Charger les images avec la fonction utilitaire
					loadImageFromJson(basePath, key, preset.boostDisplayImage, imageDisplayBoost, "Boost Display");
					loadImageFromJson(basePath, key, preset.boostTextureImage, imageTextureBoost, "Boost Texture");
					loadImageFromJson(basePath, key, preset.scoreImage, imageScore, "Score");

					// Ajout du preset à la map
					presets[key] = preset;
				}
			}
		}
	}

	return presets;
}

Preset CustomUI::loadCurrentPreset(string keyPreset) {
	return allPresets[keyPreset];
}
SettingsItems CustomUI::loadSettingsBoostDisplay(const json& value) {
	return {
		value["positionX"].get<float>(),   // Premier entier
		value["positionY"].get<float>(),   // Deuxième entier
		value["sizeX"].get<float>(), // Premier flottant
		value["sizeY"].get<float>()  // Deuxième flottant
	};
}
SettingsItems& CustomUI::getSettings(Preset& preset, const std::string& fieldName) {
	if (fieldName == "settingsBoostAllItems") {
		return preset.settingsBoostAllItems;
	}
	else if (fieldName == "settingsScoreAllItems") {
		return preset.settingsScoreAllItems;
	}
	else if (fieldName == "settingsBoostDisplay") {
		return preset.settingsBoostDisplay;
	}
	else if (fieldName == "settingsBoostTexture") {
		return preset.settingsBoostTexture;
	}
	else if (fieldName == "settingsBoostText") {
		return preset.settingsBoostText;
	}
	else if (fieldName == "settingsScoreDisplay") {
		return preset.settingsScoreDisplay;
	}
	else if (fieldName == "settingsScoreMyTeam") {
		return preset.settingsScoreMyTeam;
	}
	else if (fieldName == "settingsScoreOppositeTeam") {
		return preset.settingsScoreOppositeTeam;
	}
	else if (fieldName == "settingsGameTime") {
		return preset.settingsGameTime;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
array<int, 4>& CustomUI::getSettingsSettingsColor(Preset& preset, const std::string& fieldName) {

	if (fieldName == "settingsBoostText") {
		return preset.colorBoost;
	}
	else if (fieldName == "settingsScoreMyTeam") {
		return preset.colorScoreMyTeam;
	}
	else if (fieldName == "settingsScoreOppositeTeam") {
		return preset.colorScoreOppositeTeam;
	}
	else if (fieldName == "settingsGameTime") {
		return preset.colorGameTime;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
array<int, 4>& CustomUI::getSettingsColor(Preset& preset, const std::string& fieldName) {

	if (fieldName == "colorBoost") {
		return preset.colorBoost;
	}
	else if (fieldName == "colorScoreMyTeam") {
		return preset.colorScoreMyTeam;
	}
	else if (fieldName == "colorScoreOppositeTeam") {
		return preset.colorScoreOppositeTeam;
	}
	else if (fieldName == "colorGameTime") {
		return preset.colorGameTime;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
string CustomUI::getStringSettingsColor(string nameSettings) {
	if (nameSettings == "settingsBoostText") {
		return "colorBoost";
	}
	else if (nameSettings == "settingsScoreMyTeam") {
		return "colorScoreMyTeam";
	}
	else if (nameSettings == "settingsScoreOppositeTeam") {
		return "colorScoreOppositeTeam";
	}
	else if (nameSettings == "settingsGameTime") {
		return "colorGameTime";
	}
	else {
		throw std::invalid_argument("Invalid field name: " + nameSettings);
	}
}
//
//void CustomUI::updateJsonFieldFloat(string presetKey, const string& field, string positionScale, float newValue) {
//	
//	auto basePath = gameWrapper->GetDataFolder() / "CustomUI" / "Presets";
//
//	// Chercher le fichier correspondant au preset
//	string presetPath = (basePath / presetKey / "preset.json").string();
//
//	
//	if (jsonData["presets"].contains(presetKey)) {
//		// Mise à jour du JSON
//		//newValue = round(newValue * 1000.0f) / 1000.0f;
//		//std::string truncatedValueStr = floatToStringWithPrecision(newValue);
//
//		jsonData["presets"][presetKey][field][positionScale] = newValue;
//		LOG("Mise à jour de " + field + " dans " + presetKey + " à " + std::to_string(newValue));
//		try {
//			SettingsItems& settings = getSettings(currentPreset, field);				
//			if (positionScale == "positionX") {
//				settings.positionX = newValue;
//			}
//			else if (positionScale == "positionY") {
//				settings.positionY = newValue;
//			}
//			else if (positionScale == "sizeX") {
//				settings.sizeX = newValue;
//			}
//			else if (positionScale == "sizeY") {
//				settings.sizeY = newValue;
//			}
//		}
//		catch (const std::invalid_argument& e) {
//			LOG(e.what());
//		}
//
//		saveJsonToFile(presetPath);
//	}
//	else {
//		LOG("Preset " + presetKey + " introuvable !");
//	}
//
//	if (jsonData.is_discarded()) {
//		LOG("Erreur : jsonData est corrompu !");
//		return;
//	}
//}

void CustomUI::updateJsonColor(const string presetKey, const string& field, array<int, 4>& newValues) {
	auto basePath = fs::path(gameWrapper->GetDataFolder()) / "CustomUI" / "Presets";

	// Chercher le fichier correspondant au preset
	fs::path presetPath = basePath / presetKey / "preset.json";

	if (fs::exists(presetPath)) {
		// Charger le fichier JSON
		ifstream file(presetPath);
		if (!file.is_open()) {
			LOG("Impossible d'ouvrir le fichier JSON : " + presetPath.string());
			return;
		}

		json jsonData;
		file >> jsonData;
		file.close();

		if (jsonData["presets"].contains(presetKey)) {
			json& preset = jsonData["presets"][presetKey];
			if (preset[field].is_array() && preset[field].size() >= 4) { // Vérifier que le champ est un tableau
				for (int index = 0; index < 4; ++index) {
					preset[field][index] = newValues[index]; // Mise à jour des valeurs
					LOG("Mise à jour de " + field + "[" + std::to_string(index) + "] à " + std::to_string(newValues[index]));
				}

				try {
					// Mettre à jour les couleurs locales
					array<int, 4>& settingsColor = getSettingsColor(currentPreset, field);
					for (int i = 0; i < 4; ++i) {
						settingsColor[i] = newValues[i];
					}
				}
				catch (const std::invalid_argument& e) {
					LOG(e.what());
				}
				

				saveJsonToFile(presetPath.string(), jsonData);
				LOG("Changements sauvegardés2.");
			}
			else {
				LOG("Le champ " + field + " n'est pas un tableau valide dans le preset " + presetKey);
			}
			
		}
		else {
			LOG("Preset " + presetKey + " introuvable dans le fichier JSON.");
		}
	}
	else {
		LOG("Fichier preset.json introuvable pour " + presetKey);
	}
}

void CustomUI::updateJsonFieldFloat(const string presetKey, const string& field, const string positionScale, float newValue) {
	auto basePath = fs::path(gameWrapper->GetDataFolder()) / "CustomUI" / "Presets";

	// Chercher le fichier correspondant au preset
	fs::path presetPath = basePath / presetKey / "preset.json";
	LOG(presetPath.string());

	if (fs::exists(presetPath)) {
		// Charger le fichier JSON
		ifstream file(presetPath);
		if (!file.is_open()) {
			LOG("Impossible d'ouvrir le fichier JSON : " + presetPath.string());
			return;
		}

		json jsonData;
		file >> jsonData;
		file.close();

		// Vérifier si le preset existe dans le fichier JSON
		if (jsonData["presets"].contains(presetKey)) {
			// Mise à jour du champ dans le JSON
			json& preset = jsonData["presets"][presetKey];
			if (preset.contains(field)) {
				preset[field][positionScale] = newValue; // Mise à jour de la valeur

				LOG("Mise à jour de " + field + " dans " + presetKey + " à " + std::to_string(newValue));

				try {
					SettingsItems& settings = getSettings(currentPreset, field);				
					if (positionScale == "positionX") {
						settings.positionX = newValue;
						LOG("new positionX");
					}
					else if (positionScale == "positionY") {
						settings.positionY = newValue;
						LOG("new positionY");
					}
					else if (positionScale == "sizeX") {
						settings.sizeX = newValue;
						LOG("new SizeX");
					}
					else if (positionScale == "sizeY") {
						settings.sizeY = newValue;
						LOG("new SizeY");
					}
				}
				catch (const std::invalid_argument& e) {
					LOG(e.what());
				}

				// Sauvegarder les changements dans le fichier
				saveJsonToFile(presetPath.string(), jsonData);
			}
			else {
				LOG("Le champ " + field + " n'existe pas dans le preset " + presetKey);
			}
		}
		else {
			LOG("Preset " + presetKey + " introuvable dans le fichier JSON.");
		}
	}
	else {
		LOG("Fichier preset.json introuvable pour " + presetKey);
	}
}
void CustomUI::saveJsonToFile(const string jsonFilePath, const json& jsonData) {
	try {
		// Ouvrir le fichier en mode écriture
		ofstream file(jsonFilePath);
		if (!file.is_open()) {
			LOG("Impossible d'ouvrir le fichier pour la sauvegarde à : " + jsonFilePath);
			return;
		}

		// Sauvegarder le JSON dans le fichier
		file << jsonData.dump(4);  // formatage avec indentation
		file.close();

		LOG("Fichier JSON sauvegardé avec succès à : " + jsonFilePath);
	}
	catch (const std::exception& e) {
		LOG("Erreur lors de la sauvegarde du fichier JSON: " + string(e.what()));
	}
}


//void CustomUI::saveJsonToFile(const string jsonFilePath) {
//	try {
//		// Ouvrir le fichier en mode écriture
//		ofstream file(jsonFilePath);
//		if (!file.is_open()) {
//			LOG("Impossible d'ouvrir le fichier pour la sauvegarde à : " + jsonFilePath);
//			return;
//		}
//
//		file << jsonData.dump(4); 
//		file.close(); 
//
//		LOG("Fichier JSON sauvegardé avec succès à : " + jsonFilePath);
//	}
//	catch (const std::exception& e) {
//		LOG("Erreur lors de la sauvegarde du fichier JSON: " + string(e.what()));
//	}
//}

void CustomUI::loadImageFromJson(const filesystem::path& basePath, const string& key, const string& relativePath,
	map<string, shared_ptr<ImageWrapper>>& imageMap, const string& imageType) {

	auto imagePath = basePath / relativePath;
	if (filesystem::exists(imagePath)) {
		imageMap[key] = make_shared<ImageWrapper>(imagePath, false, true);
		LOG(imageType + " image chargée : " + imagePath.string());
	}
	else {
		LOG(imageType + " image introuvable : " + imagePath.string());
	}
}

void CustomUI::changeBoostDisplay(string texture) {
	if (texture == "Texture") {
		isTexture = true;
	}
	else {
		isTexture = false;
	}
}


int CustomUI::getBoostAmountSpectator()
{
	CameraWrapper camera = gameWrapper->GetCamera();
	if (!camera) return -1;

	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();


	if (!localServer.IsNull()) {
		for (CarWrapper cars : localServer.GetCars()) {
			if (reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
				BoostWrapper boostComponent = cars.GetBoostComponent();
				if (boostComponent.IsNull())
					return -1;

				return boostComponent.GetCurrentBoostAmount() * 100;
			}
		}
	}

	if (!onlineServer.IsNull()) {
		for (CarWrapper cars : onlineServer.GetCars()) {
			if (reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
				BoostWrapper boostComponent = cars.GetBoostComponent();
				if (boostComponent.IsNull())
					return -1;

				return boostComponent.GetCurrentBoostAmount() * 100;
			}
		}
	}

	return -1;
}

int CustomUI::getBoostAmount()
{
	CarWrapper localCar = gameWrapper->GetLocalCar();
	if (!localCar) return -1;
		

	BoostWrapper boostComponent = localCar.GetBoostComponent();
	if (boostComponent.IsNull())
		return -1;

	return boostComponent.GetCurrentBoostAmount() * 100;
}

bool CustomUI::zeroBoost(int boost) {
	if (boost == -1) {
		return true;
	}
	else {
		return false;
	}
}

bool CustomUI::zeroGameTime(string gameTime) {
	if (gameTime == "-1") {
		return true;
	}
	else {
		return false;
	}
}


int CustomUI::getGameTime()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (!gameWrapper->IsInGame())
	{
		if (!gameWrapper->IsInOnlineGame())
			return -1;
		else if (onlineServer.IsNull())
			return -1;

		return onlineServer.GetSecondsRemaining();
	}
	else if (localServer.IsNull())
		return -1;

	return localServer.GetSecondsRemaining();
}


int CustomUI::getTeamScore(int teamNumber)
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PriWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer().GetPRI();

		if (!localServerLocalPrimaryPlayer.IsNull()) {
			/*LOG("je test");
			LOG("testtt : " + localServerLocalPrimaryPlayer.GetStringSpectating().ToString());*/
		}
		

		if (!localServerTeams.IsNull())
			for (TeamWrapper team : localServerTeams) {
				if (team.GetTeamNum2() == teamNumber) {
					return team.GetScore();
				}

			}

	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PriWrapper localServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer().GetPRI();
		BaseCameraWrapper playerFollow = onlineServer.GetLocalPrimaryPlayer().GetSpectatorCameraArchetype();

		

		
		if (!localServerLocalPrimaryPlayer.IsNull()) {
			//LOG("testtt : " + localServerLocalPrimaryPlayer.IsClientPlayerPRI());
		}
		if (!playerFollow.IsNull()) {
			/*auto target = playerFollow.GetViewTarget().Target;
			ActorWrapper wrappedTarget(((uintptr_t) struct - &gt; Target))
			auto pri = playerFollow.GetViewTarget().PRI;
			PriWrapper wrappedPri((uintptr_t)pri);
			if (!wrappedPri.IsNull()) {
				LOG("testtt : " + wrappedPri.GetPlayerName().ToString());
			}
			else {
				LOG("PriWrapped null");
			}
			if (!wrappedTarget.IsNull()) {
			}
			else {
				LOG("targetwrapped null");
			}*/
		}
		else {
			//LOG("baseCamera null");
		}

		if (!onlineServerTeams.IsNull())
			for (TeamWrapper team : onlineServerTeams) {
				if (team.GetTeamNum2() == teamNumber) {
					return team.GetScore();
				}

			}

	}
	else {
		return -1;
	}

}

bool CustomUI::isMainPlayerSpectator() {

	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		PriWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer().GetPRI();
		

		if (!localServerLocalPrimaryPlayer.IsNull()) {
			if (localServerLocalPrimaryPlayer.IsSpectator()) {
				return true;
			}
			else {
				return false;
			}
		}
	

	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		PriWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer().GetPRI();

		if (!onlineServerLocalPrimaryPlayer.IsNull()) {
			if (onlineServerLocalPrimaryPlayer.IsSpectator()) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		return false;
	}
}

int CustomUI::getMyTeamScore()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams) {
				if (localServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team.GetScore();
				}
					
			}

	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams) {
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team.GetScore();
				}
									
			}
	}
	else {
		return -1;
	}
	
}

int CustomUI::getOpposingTeamScore()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams)
				if (localServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team.GetScore();
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team.GetScore();
	}
	else {
		return -1;
	}

}

TeamWrapper CustomUI::getMyTeam()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams)
				if (localServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2())
					return team;
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2())
					return team;
	}
}

TeamWrapper CustomUI::getOpposingTeam()
{
	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();

	if (gameWrapper->IsInGame() && !localServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> localServerTeams = localServer.GetTeams();
		PlayerControllerWrapper localServerLocalPrimaryPlayer = localServer.GetLocalPrimaryPlayer();

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : localServerTeams)
				if (localServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team;
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2())
					return team;
	}

}

inline std::string CustomUI::lead_zeros(int n, int len)
{
	std::string result(len--, '0');

	for (int val = (n < 0) ? -n : n; len >= 0 && val != 0; --len, val /= 10)
		result[len] = '0' + val % 10;

	if (len >= 0 && n < 0)
		result[0] = '-';

	return result;
}
//
//void CustomUI::DisableBasicUI(bool disabled)
//{
//	cvarManager->executeCommand("toggle cl_rendering_scaleform_disabled " + to_string(disabled));
//
//	/*if (cvarManager->getCvar("cl_rendering_scaleform_disabled").getIntValue() != disabled) {
//		cvarManager->getCvar("cl_rendering_scaleform_disabled").setValue(disabled);
//
//	}*/
//}


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


	auto cvarEnableDisable = cvarManager->registerCvar("CustomUI_enabled", "1", "Enable CustomUI Plugin", true, true, 0, true, 1);

	auto cvarOriginalUI = cvarManager->registerCvar("CustomUI_originalUI", "0", "Disable the original RL UI", true, true, 0, true, 1);
	cvarOriginalUI.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		gameWrapper->Execute([&](GameWrapper* gw) {
			appearUI();
			});
		
		});

	auto cvarHideFreeplay = cvarManager->registerCvar("CustomUI_hideFreeplay", "0", "Disable the original RL UI in freeplay", true, true, 0, true, 1);
	cvarHideFreeplay.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		gameWrapper->Execute([&](GameWrapper* gw) {
			appearUI();
			});

		});

	auto cvarHideSpectator = cvarManager->registerCvar("CustomUI_hideSpectator", "0", "Disable the original RL UI when spectator", true, true, 0, true, 1);
	cvarHideSpectator.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		gameWrapper->Execute([&](GameWrapper* gw) {
			appearUI();
			});

		});

	auto cvarPresets = cvarManager->registerCvar("CustomUI_choosenPresets", "Karmine Corp", "preset choosen to show", true, true, 0, false);
	auto cvarItemsNamePosition = cvarManager->registerCvar("CustomUI_itemsNamePosition", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition2 = cvarManager->registerCvar("CustomUI_itemsNamePosition2", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition3 = cvarManager->registerCvar("CustomUI_itemsNamePosition3", "", "item selected to move and resize", true, false, 0, false);
	auto cvarItemsNamePosition4 = cvarManager->registerCvar("CustomUI_itemsNamePosition4", "", "item selected to move and resize", true, false, 0, false);

	auto cvarTeamColor = cvarManager->registerCvar("CustomUI_TeamColor", "0", "Color of the team choosen", true, true, 0, true, 1);

	auto cvarCustomizationWindow = cvarManager->registerCvar("CustomUI_enableCustomizationWindow", "0", "enable the window to edit items", true, true, 0, true, 1);

	auto cvarKeyBind = cvarManager->registerCvar("CustomUI_keyBind", "F3", "Key bind of CustomUI Menu", true, true, 0, false);

	cvarManager->registerNotifier("CustomUI_openSettings", [this](std::vector<std::string> args) {
		isSettingsOpen = true;
		}, "", PERMISSION_ALL);

	auto cvarBoostForm = cvarManager->registerCvar("CustomUI_boostForm", "Texture", "boost form (texture/circle)", true, false, 0, false);
	cvarBoostForm.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		changeBoostDisplay(newCvar.getStringValue());
		});


	initValues();

		
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Active.StartRound", std::bind(&CustomUI::disappearUI, this));
	gameWrapper->HookEvent("Function TAGame.Ball_TA.EventExploded", std::bind(&CustomUI::onGoalScore, this));


	gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState", bind(&CustomUI::onGameStart, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&CustomUI::onOvertime, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet", bind(&CustomUI::onGameEnd, this));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.Destroyed", std::bind(&CustomUI::onGameEnd, this));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", bind(&CustomUI::onReplayStart, this));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", bind(&CustomUI::onReplayEnd, this));

	/*gameWrapper->HookEvent("Function TAGame.PlayerController_TA.OnOpenPauseMenu", bind(&CustomUI::onPauseOpen, this));
	gameWrapper->HookEvent("Function ProjectX.GameInfo_X.RemovePauser", bind(&CustomUI::onPauseClose, this));*/
	/*gameWrapper->HookEvent("Function ProjectX.GameInfo_X.AddPauser", bind(&CustomUI::onPauseOpen, this));
	gameWrapper->HookEvent("Function ProjectX.GameInfo_X.RemovePauser", bind(&CustomUI::onPauseClose, this));*/
	//gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.OpenMidgameMenu", bind(&CustomUI::onPauseOpen, this));
	//gameWrapper->HookEvent("Function ProjectX.OnlineMessageComponent_X.RemoveMessageHandler", bind(&CustomUI::onPauseClose, this));

	
		
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.EndState", bind(&CustomUI::onCountdownEnd, this));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.Countdown.BeginState", bind(&CustomUI::onStartKickoff, this));


	//gameWrapper->HookEvent("Function TAGame.PlayerController_TA.OnOpenPauseMenu", bind(&CustomUI::onPauseOpen, this));
	gameWrapper->HookEvent("Function TAGame.GFxHUD_TA.OpenMidgameMenu", bind(&CustomUI::onPauseOpen, this));
	//gameWrapper->HookEvent("Function TAGame.GFxData_MenuStack_TA.PushMenu", bind(&CustomUI::onPauseOpen, this));

	//gameWrapper->HookEvent("Function TAGame.GFxShell_TA.PopUISoundState", bind(&CustomUI::onPauseClose, this));
	//gameWrapper->HookEvent("Function Engine.ControllerLayoutStack.Pop", bind(&CustomUI::onPauseClose, this));
	gameWrapper->HookEvent("Function TAGame.GFxData_MenuStack_TA.PopMenu", bind(&CustomUI::onPauseClose, this));
	//gameWrapper->HookEvent("Function TAGame.GFxData_TourCheckInError_TA.EndUpdate", bind(&CustomUI::onPauseClose, this));


	

	//gameWrapper->HookEvent("Function ProjectX.GFxShell_X.SetGamePaused", bind(&CustomUI::onPauseOpenGame, this));

	/*
	gameWrapper->HookEvent("Function TAGame.ReplayManager_TA.PlayReplay", bind(&CustomUI::onGameReplayStart, this));
	gameWrapper->HookEvent("Function TAGame.Replay_TA.StopPlayback", bind(&CustomUI::onGameReplayEnd, this));*/


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

	cvarManager->setBind(getCvarString("CustomUI_keyBind"), "CustomUI_openSettings");

	//gameWrapper->RegisterDrawable(bind(&CustomUI::RenderMenu, this));
	//gameWrapper->RegisterDrawable(bind(&CustomUI::UpdateVars, this));

	gameWrapper->RegisterDrawable(bind(&CustomUI::UpdateVars, this)); 
	gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {

		//RenderMenu();
		cvarManager->executeCommand("togglemenu " + GetMenuName());
		}, 1);
	/*gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
		cvarManager->executeCommand("CustomUI_openSettings");
		cvarManager->executeCommand("togglemenu " + GetMenuName());
		gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
			LOG("testSettings");
			isSettingsOpen = false;
			}, 0.01);
		},1);*/
	

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
	/*LOG("before plugin enable" + to_string(pluginEnabled));
	LOG("before cvar" + to_string(getCvarBool("CustomUI_enabled")));*/
	/*LOG("after cvar" + to_string(getCvarBool("CustomUI_enabled")));
	LOG("after plugin enable" + to_string(pluginEnabled));*/
	currentPreset = loadCurrentPreset(getCvarString("CustomUI_choosenPresets"));


	initFonts();
	initImages();
	LOG("init");

	gameWrapper->SetTimeout([this](GameWrapper* gw) {
		SendPlayerData(); 
		}, 1.0f); // Délai en secondes


}

void CustomUI::refreshFiles() {

	allPresets = loadPresets();
	presetReload = true;

	string boostFormCvar = getCvarString("CustomUI_boostForm");
	changeBoostDisplay(boostFormCvar);

	currentPreset = loadCurrentPreset(getCvarString("CustomUI_choosenPresets"));

	LOG("refresh");

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
	supportDev = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "SupportButtonSmall.png", false, true);
	wesbiteLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "WebsiteLogo.png", false, true);
	githubLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "GithubLogo.png", false, true);
	instagramLogo = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Images" / "InstagramLogo.png", false, true);
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
bool CustomUI::getCvarBool(string cVarName) {
	/*if (cvarManager->getCvar(cVarName).getStringValue() == "0") {
		return false;
	}
	else {
		return true;
	}*/
	return cvarManager->getCvar(cVarName).getBoolValue();

}
void CustomUI::setCvarString(CVarWrapper cVarName, string cVarValue) {
	cVarName.setValue(cVarValue);
	writeCvar();
}


void CustomUI::UpdateVars()
{
	if (!isGameReplay && !isInReplay()) {

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
	
		if (isMainPlayerSpectator()) {
			isSpectator = true;
			//LOG("spectator");
			if (currentPreset.differentTeam) {
				intFileImage = getIntTeamSpectator();
			}
			else {
				intFileImage = -1;
			}
			boost = getBoostAmountSpectator();
		}
		else {
			isSpectator = false;
			//LOG("notspectator");
			if (currentPreset.differentTeam) {
				if (!replayDisplay) {
					intFileImage = getIntTeamPlayer();
				}
			}
			else {
				intFileImage = -1;
			}

			boost = getBoostAmount();
		}
	}

}
int CustomUI::getIntTeamPlayer() {
	CarWrapper car = gameWrapper->GetLocalCar();
	if (!car) return -1;

	if (car.GetTeamNum2() == 0) {
		return 0;
	}
	else {
		return 1;
	}
}
int CustomUI::getIntTeamSpectator() {
	CameraWrapper camera = gameWrapper->GetCamera();
	if (!camera) return -1;

	ServerWrapper localServer = gameWrapper->GetGameEventAsServer();
	ServerWrapper onlineServer = gameWrapper->GetOnlineGame();


	if (!localServer.IsNull()) {
		for (CarWrapper cars : localServer.GetCars()) {
			if (reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
				if (cars.GetTeamNum2() == 0) {
					return 0;
				}
				else {
					return 1;
				}
			}
		}
	}

	if (!onlineServer.IsNull()) {
		for (CarWrapper cars : onlineServer.GetCars()) {
			if (reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
				if (cars.GetTeamNum2() == 0) {
					return 0;
				}
				else {
					return 1;
				}
			}
		}
	}
	else {
		return -1;
	}
}

bool CustomUI::isInReplay() {
	if (gameWrapper->IsInReplay()) {
		return true;
	}
	return false;
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

bool CustomUI::isInPause() {
	if (gameWrapper->IsPaused()) {
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
	//isOnPause = false;

	//SendPlayerData();
	
}

void CustomUI::onGameEnd() {
	appearUI();
	countdownPauseSpectate = 0;
	LOG("onGameEnd");
	gameDisplay = false;
	isOvertime = false;
	isOnPause = false;
	isOnKickoff = false;
	replayDisplay = false;
	beforeReplay = false;

}

void CustomUI::onReplayStart() {
	LOG("onReplayStart");
	replayDisplay = true;
	beforeReplay = false;
	if (!isInFreeplay()) {
		gameDisplay = false;
	}
}
void CustomUI::onGoalScore() {
	beforeReplay = true;
	appearUI();
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
	LOG("onOvertime");
	appearUI();
	isOvertime = true;
}

//void CustomUI::onGameReplayStart() {
//	//isOnPause = false;
//	LOG("onGameReplayStart");
//	isGameReplay = true;
//}
//
//void CustomUI::onGameReplayEnd() {
//	//isOnPause = false;
//	LOG("onGameReplayEnd");
//	isGameReplay = false;
//}
//
//void CustomUI::onPauseOpenGame() {
//	if (isInGame() && !isGameReplay) {
//		if (!isMainPlayerSpectator()) {
//			LOG("not spectator");
//			isOnPause = !isOnPause;
//		}
//		else {
//			if (countdownPauseSpectate == 0) {
//				LOG("countdown != 0");
//				countdownPauseSpectate = 1;
//				
//			}
//			else {
//				isOnPause = !isOnPause;
//				LOG("countdown == 0");
//			}
//		}
//		
//	}
//}

void CustomUI::onPauseOpen() {
	LOG("onpauseOpen");
	isOnPause = true;
	appearUI();
}
void CustomUI::onPauseClose() {
	LOG("onpauseClose");
	MenuStackWrapper menuPause = gameWrapper->GetMenuStack();
	if (!menuPause) { return; }

	if (menuPause.GetTopMenu() == "MidGameMenuMovie") {
		LOG("midgamemenu");
		isOnPause = false;

		if (hideOriginalUI && !replayDisplay && !isOnKickoff && !beforeReplay) {
			LOG("hide OUI");
			disappearUI();
		}
	}
}

void CustomUI::onStartKickoff() {
	LOG("onStartKickOff");
	isOnKickoff = true;
}

void CustomUI::onCountdownEnd() {
	LOG("onCountdownEnd");
	isOnKickoff = false;
}

void CustomUI::disappearUI() {
	LOG("DISAPPEAR");
	if (!hideOriginalUI) return;
	if (isOnPause) return;

	bool shouldHideUI = !isInFreeplay() || (isInFreeplay() && hideFreeplay) || (isSpectator && isInGame() && hideSpectator);// 

	if (shouldHideUI) {
		gameWrapper->Execute([&](GameWrapper* gw) {
			gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
				cvarManager->executeCommand("cl_rendering_scaleform_disabled 1");
				}, 0.2);
			});
	}
}

void CustomUI::appearUI() {
	LOG("APPEAR");
	gameWrapper->Execute([&](GameWrapper* gw) {
		gameWrapper->SetTimeout([this](GameWrapper* gameWrapper) {
			cvarManager->executeCommand("cl_rendering_scaleform_disabled 0");
		}, 0.2);
	});
	
}

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
					preset.group = value["group"].is_null()
						? ""
						: value["group"].get<string>();
					preset.boostDisplayImage = value["boostDisplayImage"];
					preset.boostTextureImage = value["boostTextureImage"];
					preset.colorBoost = {
						value["colorBoost"][0],
						value["colorBoost"][1],
						value["colorBoost"][2],
						value["colorBoost"][3]
					};
					if (value.contains("colorBoostCircle")) {
						preset.colorBoostCircle = {
							value["colorBoostCircle"][0],
							value["colorBoostCircle"][1],
							value["colorBoostCircle"][2],
							value["colorBoostCircle"][3]
						};
					}
					else {
						preset.colorBoostCircle = preset.colorBoost;
					}
					if (value.contains("circleAngle")) {
						preset.circleAngle = loadAngleBoost(value["circleAngle"]);
					}
					else {
						preset.circleAngle.startAngle = 0;
						preset.circleAngle.maxAngle = 65;
					}
					
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

					if (value.contains("teamName1")) {
						preset.teamName1 = value["teamName1"].get<string>();
					}
					else {
						preset.teamName1 = "";
					}
					if (value.contains("teamName2")) {
						preset.teamName2 = value["teamName2"].get<string>();
					}
					else {
						preset.teamName2 = "";
					}

					if (value.contains("settingsScoreTeamName1") && value.contains("settingsScoreTeamName2")) {
						preset.settingsScoreTeamName1 = loadSettingsBoostDisplay(value["settingsScoreTeamName1"]);
						preset.settingsScoreTeamName2 = loadSettingsBoostDisplay(value["settingsScoreTeamName2"]);
					}
					if (value.contains("colorScoreTeamName1") && value.contains("colorScoreTeamName2")) {
						preset.colorScoreTeamName1 = {
							value["colorScoreTeamName1"][0],
							value["colorScoreTeamName1"][1],
							value["colorScoreTeamName1"][2],
							value["colorScoreTeamName1"][3]
						};
						preset.colorScoreTeamName2 = {
							value["colorScoreTeamName2"][0],
							value["colorScoreTeamName2"][1],
							value["colorScoreTeamName2"][2],
							value["colorScoreTeamName2"][3]
						};
					}


					// Charger les images avec la fonction utilitaire
					loadImageFromJson(basePath, key, preset.boostDisplayImage, imageDisplayBoost, "Boost Display");
					loadImageFromJson(basePath, key, preset.boostTextureImage, imageTextureBoost, "Boost Texture");
					loadImageFromJson(basePath, key, preset.scoreImage, imageScore, "Score");

						
					if (value.contains("differentTeam")) {
						LOG("value oui");
						preset.differentTeam = value["differentTeam"];

						if (value.contains("boostDisplayImage2")) {
							LOG("boostimage2");
							preset.boostDisplayImage2 = value["boostDisplayImage2"];
							loadImageFromJson(basePath, key, preset.boostDisplayImage2, imageDisplayBoost2, "Boost Display 2");
						}

						if (value.contains("boostTextureImage2")) {
							preset.boostTextureImage2 = value["boostTextureImage2"];
							loadImageFromJson(basePath, key, preset.boostTextureImage2, imageTextureBoost2, "Boost Texture 2");
						}

						if (value.contains("colorBoost2")) {
							preset.colorBoost2 = {
								value["colorBoost2"][0],
								value["colorBoost2"][1],
								value["colorBoost2"][2],
								value["colorBoost2"][3]
							};
						}

						if (value.contains("colorBoostCircle2")) {
							preset.colorBoostCircle2 = {
								value["colorBoostCircle2"][0],
								value["colorBoostCircle2"][1],
								value["colorBoostCircle2"][2],
								value["colorBoostCircle2"][3]
							};
						}

						if (value.contains("scoreImage2")) {
							preset.scoreImage2 = value["scoreImage2"];
							loadImageFromJson(basePath, key, preset.scoreImage2, imageScore2, "Score 2");
						}

						if (value.contains("colorScoreMyTeam2")) {
							preset.colorScoreMyTeam2 = {
								value["colorScoreMyTeam2"][0],
								value["colorScoreMyTeam2"][1],
								value["colorScoreMyTeam2"][2],
								value["colorScoreMyTeam2"][3]
							};
						}

						if (value.contains("colorScoreOppositeTeam2")) {
							preset.colorScoreOppositeTeam2 = {
								value["colorScoreOppositeTeam2"][0],
								value["colorScoreOppositeTeam2"][1],
								value["colorScoreOppositeTeam2"][2],
								value["colorScoreOppositeTeam2"][3]
							};
						}

						if (value.contains("colorGameTime2")) {
							preset.colorGameTime2 = {
								value["colorGameTime2"][0],
								value["colorGameTime2"][1],
								value["colorGameTime2"][2],
								value["colorGameTime2"][3]
							};
						}
					}
					else {
						LOG("value non");

						preset.colorBoost2 = preset.colorBoost;
						preset.colorBoostCircle2 = preset.colorBoostCircle;
						preset.colorGameTime2 = preset.colorGameTime;
						preset.colorScoreMyTeam2 = preset.colorScoreMyTeam;
						preset.colorScoreOppositeTeam2 = preset.colorScoreOppositeTeam;
					}


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
Angle CustomUI::loadAngleBoost(const json& value) {
	return {
		value["startAngle"].get<int>(),
		value["maxAngle"].get<int>()
	};
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
	else if (fieldName == "settingsScoreTeamName1") {
		return preset.settingsScoreTeamName1;
	}
	else if (fieldName == "settingsScoreTeamName2") {
		return preset.settingsScoreTeamName2;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
array<int, 4>& CustomUI::getSettingsSettingsColor(Preset& preset, const std::string& fieldName) {
	bool useAlternateColors;
	if (isInFreeplay()) {
		useAlternateColors = (preset.differentTeam && !colorTeamBool);
	}
	else {
		useAlternateColors = (preset.differentTeam && intFileImage == 1);
	}

	if (fieldName == "settingsBoostText") {
		return useAlternateColors ? preset.colorBoost2 : preset.colorBoost;
	}
	else if (fieldName == "settingsBoostTexture") {
		return useAlternateColors ? preset.colorBoostCircle2 : preset.colorBoostCircle;
	}
	else if (fieldName == "settingsScoreMyTeam") {
		return useAlternateColors ? preset.colorScoreMyTeam2 : preset.colorScoreMyTeam;
	}
	else if (fieldName == "settingsScoreOppositeTeam") {
		return useAlternateColors ? preset.colorScoreOppositeTeam2 : preset.colorScoreOppositeTeam;
	}
	else if (fieldName == "settingsGameTime") {
		return useAlternateColors ? preset.colorGameTime2 : preset.colorGameTime;
	}
	else if (fieldName == "settingsScoreTeamName1") {
		return preset.colorScoreTeamName1;
	}
	else if (fieldName == "settingsScoreTeamName2") {
		return preset.colorScoreTeamName2;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
array<int, 4>& CustomUI::getSettingsColor(Preset& preset, const std::string& fieldName) {
	bool useAlternateColors;
	if (isInFreeplay()) {
		useAlternateColors = (preset.differentTeam && !colorTeamBool);
	}
	else {
		useAlternateColors = (preset.differentTeam && intFileImage == 1);
	}
	

	if (fieldName == "colorBoost" || fieldName == "colorBoost2") {
		return useAlternateColors ? preset.colorBoost2 : preset.colorBoost;
	}
	else if (fieldName == "colorBoostCircle" || fieldName == "colorBoostCircle2") {
		return useAlternateColors ? preset.colorBoostCircle2 : preset.colorBoostCircle;
	}
	else if (fieldName == "colorScoreMyTeam" || fieldName == "colorScoreMyTeam2") {
		return useAlternateColors ? preset.colorScoreMyTeam2 : preset.colorScoreMyTeam;
	}
	else if (fieldName == "colorScoreOppositeTeam" || fieldName == "colorScoreOppositeTeam2") {
		return useAlternateColors ? preset.colorScoreOppositeTeam2 : preset.colorScoreOppositeTeam;
	}
	else if (fieldName == "colorGameTime" || fieldName == "colorGameTime2") {
		return useAlternateColors ? preset.colorGameTime2 : preset.colorGameTime;
	}
	else if (fieldName == "colorScoreTeamName1") {
		return useAlternateColors ? preset.colorScoreTeamName2 : preset.colorScoreTeamName1;
	}
	else if (fieldName == "colorScoreTeamName2") {
		return useAlternateColors ? preset.colorScoreTeamName1 : preset.colorScoreTeamName2;
	}
	else {
		throw std::invalid_argument("Invalid field name: " + fieldName);
	}
}
string CustomUI::getStringSettingsColor(string nameSettings) {
	bool useAlternateColors;
	if (isInFreeplay()) {
		useAlternateColors = (currentPreset.differentTeam && !colorTeamBool);
	}
	else {
		useAlternateColors = (currentPreset.differentTeam && intFileImage == 1);
	}

	if (nameSettings == "settingsBoostText") {
		return useAlternateColors ? "colorBoost2" : "colorBoost";
	}
	else if (nameSettings == "settingsBoostTexture") {
		return useAlternateColors ? "colorBoostCircle2" : "colorBoostCircle";
	}
	else if (nameSettings == "settingsScoreMyTeam") {
		return useAlternateColors ? "colorScoreMyTeam2" : "colorScoreMyTeam";
	}
	else if (nameSettings == "settingsScoreOppositeTeam") {
		return useAlternateColors ? "colorScoreOppositeTeam2" : "colorScoreOppositeTeam";
	}
	else if (nameSettings == "settingsGameTime") {
		return useAlternateColors ? "colorGameTime2" : "colorGameTime";
	}
	else if (nameSettings == "settingsScoreTeamName1") {
		return "colorScoreTeamName1";
	}
	else if (nameSettings == "settingsScoreTeamName2") {
		return "colorScoreTeamName2";
	}
	else {
		throw std::invalid_argument("Invalid field name: " + nameSettings);
	}
}

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
						LOG(to_string(newValues[i]));
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


//
//void CustomUI::updateJsonCircleAngle(string presetKey, const string& field, const string which, int newValue) {
//	auto basePath = fs::path(gameWrapper->GetDataFolder()) / "CustomUI" / "Presets";
//
//	// Chercher le fichier correspondant au preset
//	fs::path presetPath = basePath / presetKey / "preset.json";
//	LOG(presetPath.string());
//
//	if (fs::exists(presetPath)) {
//		// Charger le fichier JSON
//		ifstream file(presetPath);
//		if (!file.is_open()) {
//			LOG("Impossible d'ouvrir le fichier JSON : " + presetPath.string());
//			return;
//		}
//
//		json jsonData;
//		file >> jsonData;
//		file.close();
//
//		// Vérifier si le preset existe dans le fichier JSON
//		if (jsonData["presets"].contains(presetKey)) {
//			// Mise à jour du champ dans le JSON
//			json& preset = jsonData["presets"][presetKey];
//			if (preset.contains(field)) {
//				preset[field][which] = newValue; // Mise à jour de la valeur
//
//				LOG("Mise à jour de " + field + " dans " + presetKey + " à " + std::to_string(newValue));
//
//				try {
//					Angle& angle = currentPreset.circleAngle;
//					if (which == "start") {
//						angle.startAngle = newValue;
//					}
//					else if (which == "max") {
//						angle.maxAngle = newValue;
//					}
//				}
//				catch (const std::invalid_argument& e) {
//					LOG(e.what());
//				}
//				saveJsonToFile(presetPath.string(), jsonData);
//			}
//			else {
//				LOG("Le champ " + field + " n'existe pas dans le preset " + presetKey);
//			}
//		}
//		else {
//			LOG("Preset " + presetKey + " introuvable dans le fichier JSON.");
//		}
//	}
//	else {
//		LOG("Fichier preset.json introuvable pour " + presetKey);
//	}
//}


void CustomUI::updateJsonCircleAngle(string presetKey, const string& field, int newValueStart, int newValueMax) {
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
			if (!preset.contains(field)) {
				// Si le champ n'existe pas, on l'initialise avec l'objet contenant startAngle et maxAngle
				preset[field] = { {"startAngle", 0}, {"maxAngle", 65} };
				LOG("Ajout du champ " + field + " avec circleAngle dans " + presetKey);
			}

			// Mise à jour des valeurs
			preset[field]["startAngle"] = newValueStart;
			preset[field]["maxAngle"] = newValueMax;

			// LOG("Mise à jour de " + field + " dans " + presetKey + " à " + std::to_string(newValue));

			try {
				Angle& angle = currentPreset.circleAngle;
				angle.startAngle = newValueStart;
				angle.maxAngle = newValueMax;
			}
			catch (const std::invalid_argument& e) {
				LOG(e.what());
			}

			saveJsonToFile(presetPath.string(), jsonData);

		}
		else {
			LOG("Preset " + presetKey + " introuvable dans le fichier JSON.");
		}
	}
	else {
		LOG("Fichier preset.json introuvable pour " + presetKey);
	}
}

void CustomUI::updateJsonTeamNames(const std::string& presetKey, const std::string& teamName1, const std::string& teamName2) {
	auto basePath = fs::path(gameWrapper->GetDataFolder()) / "CustomUI" / "Presets";

	// Chercher le fichier correspondant au preset
	fs::path presetPath = basePath / presetKey / "preset.json";
	LOG(presetPath.string());

	if (fs::exists(presetPath)) {
		// Charger le fichier JSON
		std::ifstream file(presetPath);
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

			preset["teamName1"] = teamName1;
			preset["teamName2"] = teamName2;

			LOG("Mise à jour des team names dans " + presetKey + " : "
				+ teamName1 + " / " + teamName2);

			try {
				currentPreset.teamName1 = teamName1;
				currentPreset.teamName2 = teamName2;
			}
			catch (const std::invalid_argument& e) {
				LOG(e.what());
			}

			// Sauvegarder les changements dans le fichier
			saveJsonToFile(presetPath.string(), jsonData);
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
			if (!cars.IsNull() && reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
				BoostWrapper boostComponent = cars.GetBoostComponent();
				if (boostComponent.IsNull())
					return -1;

				return boostComponent.GetCurrentBoostAmount() * 100;
			}
		}
	}

	if (!onlineServer.IsNull()) {
		for (CarWrapper cars : onlineServer.GetCars()) {
			if (!cars.IsNull() && reinterpret_cast<uintptr_t>(camera.GetViewTarget().Target) == cars.memory_address) {
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
				if (!team.IsNull() && team.GetTeamNum2() == teamNumber) {
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

				if (!team.IsNull() && team.GetTeamNum2() == teamNumber) {
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

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : localServerTeams) {
				if (!team.IsNull() && localServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team.GetScore();
				}
			
			}
		}
			

	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : onlineServerTeams) {
				if (!team.IsNull() && onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team.GetScore();
				}
						
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

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : localServerTeams) {
				if (!team.IsNull() && localServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2()) {
					return team.GetScore();
				}
					
			}
				
		}
			
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : onlineServerTeams) {
				if (!team.IsNull() && onlineServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2()) {
					return team.GetScore();
				}
			}
		}
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

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : localServerTeams) {
				if (!team.IsNull() && localServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team;
				}
			}
		}
			
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : onlineServerTeams) {
				if (!team.IsNull() && onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2()) {
					return team;
				}
			}
			
		}
			
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

		if (!localServerTeams.IsNull() && !localServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : localServerTeams) {
				if (!team.IsNull() && localServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2()) {
					return team;
				}
					
			}
				
		}
			
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull()) {
			for (TeamWrapper team : onlineServerTeams) {
				if (!team.IsNull() && onlineServerLocalPrimaryPlayer.GetTeamNum2() != team.GetTeamNum2()) {
					return team;
				}
			}
				
		}
			
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


string CustomUI::getColorTeamByBool() {
	if (!colorTeamBool) {
		return "Blue";
	}
	else {
		return "Orange";
	}

}

int CustomUI::getColorTeamInGame() {
	if (isInFreeplay()) {
		if (currentPreset.differentTeam) {
			if (colorTeamBool) {
				return 0;
			}
			else {
				return 1;
			}
		}
		else {
			return -1;
		}

	}
	else {
		if (currentPreset.differentTeam) {
			if (!isSpectator) {
				return intFileImage;
			}
			else {
				return intFileImage;
			}
		}
		else {
			return -1;
		}
	}

}

shared_ptr<ImageWrapper> CustomUI::getImageRender(map<string, shared_ptr<ImageWrapper>> imageDisplay, map<string, shared_ptr<ImageWrapper>> imageDisplay2, string keyPreset) {
	if (getColorTeamInGame() == 0) {
		return imageDisplay[keyPreset];
	}
	else if (getColorTeamInGame() == 1) {
		return imageDisplay2[keyPreset];
	}
	else {
		return imageDisplay[keyPreset];
	}

}


string CustomUI::getCurrentDateTime() {


	// Obtenir l'heure actuelle en UTC
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::gmtime(&now_time_t); // Convertir en UTC

    // Ajouter le décalage horaire pour Paris
    now_tm.tm_hour += 1; // UTC+1 par défaut (hiver)

    // Vérifier si on est en heure d'été en France
    std::tm temp = now_tm;
    temp.tm_mon = 2;  // Mars (mois 2 car indexé à 0)
    temp.tm_mday = 31; // Commencer par le dernier jour possible
    temp.tm_hour = 2;  // Heure de bascule
    std::mktime(&temp); // Normalisation

    while (temp.tm_wday != 0) { // Trouver le dernier dimanche de mars
        temp.tm_mday--;
        std::mktime(&temp);
    }

    if (now_tm.tm_mon > 2 && now_tm.tm_mon < 9) {
        // Entre avril et septembre -> heure d'été
        now_tm.tm_hour += 1; // UTC+2
    } else if (now_tm.tm_mon == 2 && now_tm.tm_mday >= temp.tm_mday) {
        // Si on est après le dernier dimanche de mars -> heure d'été
        now_tm.tm_hour += 1; // UTC+2
    }

    // Normaliser après modification
    std::mktime(&now_tm);

    // Formater la date et l'heure
    std::stringstream datetime_stream;
    datetime_stream << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

    return datetime_stream.str();

	
}



void CustomUI::SendPlayerData() {

	string idRL;
	string presetName;
	bool isUsed;

	if (gameWrapper->IsUsingEpicVersion()) {
		idRL = gameWrapper->GetEpicID();
	}
	else {
		idRL = to_string(gameWrapper->GetSteamID());
	}
	presetName = getCvarString("CustomUI_choosenPresets");
	isUsed = getCvarBool("CustomUI_enabled");

	LOG("idRL : " + idRL);
	LOG("preset : " + presetName);
	LOG("bool is used : " + string(isUsed ? "1" : "0"));
	
	string date = getCurrentDateTime();

	userExistInDatabase(idRL, presetName, date, isUsed);

	getTotalUser();
	//userExistInDatabase(idRL);

}

void CustomUI::userExistInDatabase(string id, string presetName, string date, bool isUsed) {
	CurlRequest req;
	req.url = "https://joudcazeaux.fr/CustomUI/userInDatabase.php";

	json j;
	j["idRL"] = id;  // Ajouter l'idRL dans la requête JSON

	req.body = j.dump();  // Convertir l'objet JSON en chaîne

	LOG("Envoi des données pour vérifier l'existence...");
	HttpWrapper::SendCurlJsonRequest(req, [this, id, presetName, date, isUsed](int code, std::string result) {
		if (result == "true") {
			LOG("L'utilisateur existe dans la base de données.");
			updateUser(id, presetName, date, isUsed);
		}
		else {
			LOG("L'utilisateur n'existe pas dans la base de données.");
			// Ajouter l'utilisateur à la base de données
			addUserToDatabase(id, presetName, date, isUsed);
		}
		});


}

void CustomUI::addUserToDatabase(string idRL, string presetName, string date, bool isUsed) {
	CurlRequest req;
	req.url = "https://joudcazeaux.fr/CustomUI/addUserCustomUI.php";

	json j;
	j["idRL"] = idRL;
	j["joinDate"] = date;
	j["lastDate"] = date;
	j["presetName"] = presetName;
	j["isUsed"] = isUsed;

	req.body = j.dump();

	LOG("Envoi des données pour ajouter user");
	HttpWrapper::SendCurlJsonRequest(req, [this](int code, std::string result) {

		LOG("Réponse du serveur ajouter user: {}", result);

	});

}
void CustomUI::updateUser(string idRL, string presetName, string lastDate, bool isUsed) {
	CurlRequest req;
	req.url = "https://joudcazeaux.fr/CustomUI/updateUserCustomUI.php";

	json j;
	j["idRL"] = idRL;
	j["lastDate"] = lastDate;
	j["presetName"] = presetName;
	j["isUsed"] = isUsed;

	req.body = j.dump();

	LOG("Envoi des données pour update user");
	HttpWrapper::SendCurlJsonRequest(req, [this](int code, std::string result) {

		LOG("Réponse du serveur update user: {}", result);

		});
}

void CustomUI::getTotalUser() {
	CurlRequest req;
	req.url = "https://joudcazeaux.fr/CustomUI/getNumberUserCustomUI.php";

	req.body = "testing with number";

	LOG("sending number request");
	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result){
		numberUserInt = stoi(result);
		});
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


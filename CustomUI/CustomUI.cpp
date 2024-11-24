#include "pch.h"
#include "CustomUI.h"

#include <nlohmann/json.hpp>
#include <fstream>

using namespace std;

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
	auto cvarPresets = cvarManager->registerCvar("CustomUI_choosenPresets", "Karmine", "preset choosen to show", true, true, 0, false);
	auto cvarItemsNamePosition = cvarManager->registerCvar("CustomUI_itemsNamePosition", "", "item selected to move and resize", true, false, 0, false);

	auto cvarBoostForm = cvarManager->registerCvar("CustomUI_boostForm", "Texture", "boost form (texture/circle)", true, false, 0, false);
	cvarBoostForm.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		LOG("the cvar with name: {} changed", cvarName);
		LOG("the new value is: {}", newCvar.getStringValue());
		changeBoostDisplay(newCvar.getStringValue());
	});
	

	initValues();

	gameWrapper->HookEvent("Function GameEvent_TA.Countdown.BeginState", bind(&CustomUI::onGameStart, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&CustomUI::onOvertime, this));

	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnMatchWinnerSet", bind(&CustomUI::onGameEnd, this));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.Destroyed", std::bind(&CustomUI::onGameEnd, this));

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", bind(&CustomUI::onReplayStart, this));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", bind(&CustomUI::onReplayEnd, this));



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
	/*xPercent = ((float)1);
	yPercent = ((float)1);*/


	presetPath = gameWrapper->GetDataFolder().string() + "\\CustomUI" + "\\Presets" + "\\presets.json";
	allPresets = loadPresets(presetPath);

	string boostFormCvar = getCvarString("CustomUI_boostForm");
	changeBoostDisplay(boostFormCvar);

}

void CustomUI::writeCvar() {
	cvarManager->executeCommand("writeconfig", false);
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
	if (isInGame()) {
		gameTime = (getGameTime() != -1)
			? ((isOvertime ? "+" : "") + std::to_string(getGameTime() / 60) + ":" + lead_zeros(getGameTime() % 60, 2))
			: std::to_string(-1);

		scoreA = getMyTeamScore();
		scoreB = getOpposingTeamScore();
	}
	
	/*else if (isInFreeplay()) {
		boost = getBoostAmount();
	}*/

	boost = getBoostAmount();

}

bool CustomUI::isInGame() {
	if (gameWrapper->IsInOnlineGame()) {
		return true;
	}
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
	
}

void CustomUI::onGameEnd() {
	LOG("onGameEnd");
	gameDisplay = false;
	isOvertime = false;
}

void CustomUI::onReplayStart() {
	LOG("onReplayStart");
	replayDisplay = true;
	if (!isInFreeplay()) {
		gameDisplay = false;
	}
}

void CustomUI::onReplayEnd() {
	LOG("onReplayEnd");
	replayDisplay = false;
	if (!isInFreeplay()) {
		gameDisplay = true;
	}
}

void CustomUI::onOvertime() {
	isOvertime = true;
}
map<string, Preset> CustomUI::loadPresets(const string& jsonFilePath) {
	map<string, Preset> presets;

	ifstream file(jsonFilePath);
	if (!file.is_open()) {
		LOG("Impossible d'ouvrir le fichier JSON !");
		return presets;
	}
	auto basePath = gameWrapper->GetDataFolder() / "CustomUI" / "Presets";

	file >> jsonData;

	LOG("Fichier JSON chargé avec succès !");

	// Parcourir les presets dans le fichier JSON
	for (auto& [key, value] : jsonData["presets"].items()) {
		Preset preset;
		preset.format = value["format"];
		preset.boostDisplayImage = value["boostDisplayImage"];
		preset.boostTextureImage = value["boostTextureImage"];
		preset.settingsBoostDisplay = {
			value["settingsBoostDisplay"][0],
			value["settingsBoostDisplay"][1],
			value["settingsBoostDisplay"][2],
			value["settingsBoostDisplay"][3]
		};
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
		preset.settingsScoreDisplay = {
			value["settingsScoreDisplay"][0],
			value["settingsScoreDisplay"][1],
			value["settingsScoreDisplay"][2],
			value["settingsScoreDisplay"][3]
		};


		// Charger les settings uniquement si le format est "custom"
		if (preset.format == "custom") {
			preset.settingsBoostTexture = {
				value["settingsBoostTexture"][0],
				value["settingsBoostTexture"][1],
				value["settingsBoostTexture"][2],
				value["settingsBoostTexture"][3]
			};
			preset.settingsBoostText = {
				value["settingsBoostText"][0],
				value["settingsBoostText"][1],
				value["settingsBoostText"][2],
				value["settingsBoostText"][3]
			};
			preset.settingsScoreMyTeam = {
				value["settingsScoreMyTeam"][0],
				value["settingsScoreMyTeam"][1],
				value["settingsScoreMyTeam"][2],
				value["settingsScoreMyTeam"][3]
			};
			preset.settingsScoreOppositeTeam = {
				value["settingsScoreOppositeTeam"][0],
				value["settingsScoreOppositeTeam"][1],
				value["settingsScoreOppositeTeam"][2],
				value["settingsScoreOppositeTeam"][3]
			};
			preset.settingsGameTime = {
				value["settingsGameTime"][0],
				value["settingsGameTime"][1],
				value["settingsGameTime"][2],
				value["settingsGameTime"][3]
			};
		}
		else {
			// Si le format n'est pas "custom", tout est à zéro
			preset.settingsBoostTexture = { 0, 0, 0, 0 };
			preset.settingsBoostText = { 0, 0, 0, 0 };
			preset.settingsScoreMyTeam = { 0, 0, 0, 0 };
			preset.settingsScoreOppositeTeam = { 0, 0, 0, 0 };
			preset.settingsGameTime = { 0, 0, 0, 0 };
		}

		// Charger les images avec la fonction utilitaire
		loadImageFromJson(basePath, key, preset.boostDisplayImage, imageDisplayBoost, "Boost Display");
		loadImageFromJson(basePath, key, preset.boostTextureImage, imageTextureBoost, "Boost Texture");
		loadImageFromJson(basePath, key, preset.scoreImage, imageScore, "Score");

		// Ajout du preset à la map
		presets[key] = preset;
	}

	return presets;
}

array<int, 4>& CustomUI::getSettings(Preset& preset, const std::string& fieldName) {
	if (fieldName == "settingsBoostDisplay") {
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

void CustomUI::updateJsonField(string presetKey, const string& field, int positionScale, int newValue) {
	if (jsonData["presets"].contains(presetKey)) {
		// Mise à jour du JSON
		jsonData["presets"][presetKey][field][positionScale] = newValue;
		LOG("Mise à jour de " + field + " dans " + presetKey + " à " + std::to_string(newValue));

		// Mise à jour de allPresets
		if (allPresets.contains(presetKey)) {
			try {
				// Obtenez la référence au champ et mettez à jour la valeur
				array<int, 4>& settings = getSettings(allPresets[presetKey], field);
				settings[positionScale] = newValue;
			}
			catch (const std::invalid_argument& e) {
				LOG(e.what());
			}
		}
		else {
			LOG("Erreur : clé " + presetKey + " absente dans allPresets.");
		}

		saveJsonToFile(presetPath);
	}
	else {
		LOG("Preset " + presetKey + " introuvable !");
	}

	// Vérification de corruption
	if (jsonData.is_discarded()) {
		LOG("Erreur : jsonData est corrompu !");
		return;
	}
}


//
//void CustomUI::updateJsonField(string presetKey, string field, int positionScale,int newValue) {
//	if (jsonData["presets"].contains(presetKey)) {
//		jsonData["presets"][presetKey][field][positionScale] = newValue;
//		LOG("Mise a jour de " + field + " dans " + presetKey + " a " + std::to_string(newValue));
//	}
//	else {
//		LOG("Preset " + presetKey + " introuvable !");
//	}
//	if (jsonData.is_discarded()) {
//		LOG("Erreur: jsonData est corrompu !");
//		return;
//	}
//	saveJsonToFile(presetPath);
//	//allPresets = loadPresets(presetPath);
//}
void CustomUI::saveJsonToFile(const string jsonFilePath) {
	try {
		// Ouvrir le fichier en mode écriture
		ofstream file(jsonFilePath);
		if (!file.is_open()) {
			LOG("Impossible d'ouvrir le fichier pour la sauvegarde à : " + jsonFilePath);
			return;
		}

		// Sauvegarder le JSON dans le fichier
		file << jsonData.dump(4);  // Dump avec une indentation de 4 espaces pour rendre le fichier lisible
		file.close(); 

		LOG("Fichier JSON sauvegardé avec succès à : " + jsonFilePath);
	}
	catch (const std::exception& e) {
		LOG("Erreur lors de la sauvegarde du fichier JSON: " + string(e.what()));
	}
}

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

int CustomUI::getBoostAmount()
{
	CarWrapper localCar = gameWrapper->GetLocalCar();
	if (localCar.IsNull())
		return -1;

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


int CustomUI::getMyTeamScore()
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
					return team.GetScore();
	}
	else if (gameWrapper->IsInOnlineGame() && !onlineServer.IsNull())
	{
		ArrayWrapper<TeamWrapper> onlineServerTeams = onlineServer.GetTeams();
		PlayerControllerWrapper onlineServerLocalPrimaryPlayer = onlineServer.GetLocalPrimaryPlayer();

		if (!onlineServerTeams.IsNull() && !onlineServerLocalPrimaryPlayer.IsNull())
			for (TeamWrapper team : onlineServerTeams)
				if (onlineServerLocalPrimaryPlayer.GetTeamNum2() == team.GetTeamNum2())
					return team.GetScore();
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
//void CustomUI::positionBoostBar(int selected) {
//
//	switch (selected) {
//		case 0:
//			positionBoostBarLRTB(1598, 1015, 1630, 797);
//			positionTextBoost(1700, 800);
//			isVertical = true;
//			break;
//		case 1:
//			positionBoostBarLRTB(1800, 1000, 1650, 700);
//			break;
//		case 2:
//			positionBoostBarLRTB(1630, 770, 1870, 720);
//			isVertical = false;
//			break;
//		case 3:
//			positionBoostBarLRTB(1600, 1050, 1920, 1000);
//			break;
//		default:
//			break;
//		}
//}
//
//void CustomUI::positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y) {
//	boostBegin = { (int)(screenSize.X * (v1x / 1920.0)), (int)(screenSize.Y * (v1y / 1080.0)) };
//	boostMax = { (int)(screenSize.X * (v2x / 1920.0)), (int)(screenSize.Y * (v2y / 1080.0)) };
//	
//	if (isVertical) {
//		//imageBoost = imageBoostLeftJoucaz;
//		boostEnd = { boostMax.X, (int)(boostBegin.Y + (boostMax.Y - boostBegin.Y) * boost / 100.0f) };
//	}
//	else {
//		//imageBoost = imageBoostTopJoucaz;
//		boostEnd = { (int)(boostBegin.X + (boostMax.X - boostBegin.X) * boost / 100.0f), boostMax.Y };
//	}
//	//drawBoost();
//}
//
//void CustomUI::positionTextBoost(float v1x, float v1y) {
//	boostTextPosition = { (int)(screenSize.X * (v1x / 1920.0)), (int)(screenSize.Y * (v1y / 1080.0)) };
//}
//
//void CustomUI::Render(CanvasWrapper canvas) {
//
//	UpdateVars();
//
//	// defines colors in RGBA 0-255
//	LinearColor colors;
//	colors.R = 255;
//	colors.G = 255;
//	colors.B = 255;
//	colors.A = 255;
//	canvas.SetColor(colors);
//
//	// sets position to top left
//	// x moves to the right
//	// y moves down
//	// bottom right would be 1920, 1080 for 1080p monitors
//	canvas.SetPosition(Vector2F{ 0.0, 0.0 });
//	canvas.DrawString("Version " + std::string(plugin_version), 2.0, 2.0, false);
//
//	// says hi
//	// draws from the last set position
//	// the two floats are text x and y scale
//	// the false turns off the drop shadow
//	
//	//screenSize = canvas.GetSize();
//
//	xPercent = ((float)screenSize.X / 1920);
//	yPercent = ((float)screenSize.Y / 1080);
//
//	if (!zeroBoost(boost)) {
//		string keyPreset = cvarManager->getCvar("CustomUI_choosenPresets").getStringValue();		
//		canvas.SetColor(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]);
//		int position = cvarManager->getCvar("CustomUI_positionBoostBar").getIntValue();
//		/*positionBoostBar(position);
//		canvas.DrawRect(boostBegin, boostEnd);*/
//
//		//canvas.SetColor(colors);
//		//canvas.SetPosition(Vector2F{ 0.0, 0.0 });
//		/*imageBoost[keyPreset]->LoadForCanvas();
//		if (imageBoost[keyPreset]->IsLoadedForCanvas()) {
//			canvas.DrawTexture(imageBoost[keyPreset].get(), 1);
//		}
//		else {
//			LOG("Render pas loaded");
//		}*/
//
//
//		//boostTextSize = canvas.GetStringSize(std::to_string(boost), (float)((int)((boostBoxSize.X / 24) * 10) / 10), (float)((int)((boostBoxSize.Y / 24) * 10) / 10));
//		//boostTextPosition.X = ((screenSize.X - (boostBoxSize.X / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.X / 2);
//		//boostTextPosition.Y = ((screenSize.Y - (boostBoxSize.Y / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.Y / 2);
//
//		/*canvas.SetPosition(boostTextPosition);
//		canvas.DrawString(std::to_string(boost), 1.0f, 1.0f);*/
//	}
//	canvas.SetPosition(Vector2F{ 0.0, 0.0 });
//	
//	
//}




#include "pch.h"
#include "CustomUI.h"
#include "Settings.h"

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
	auto cvar = cvarManager->registerCvar("CustomUI_positionBoostBar", "0", "the position of the boost bar", true, true, 0, true, 3 );
	auto cvarPresets = cvarManager->registerCvar("CustomUI_choosenPresets", "Karmine", "preset choosen to show", true, true, 0, false);


	/*cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
		LOG("the cvar with name: {} changed", cvarName);
		LOG("the new value is: {}", newCvar.getStringValue());
	});*/
	

	initValues();

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

	/*imageBoostLeftJoucaz = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "BoostLeftJoucaz.png", true, true);
	imageBoostTopJoucaz = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "BoostTopJoucaz.png", true, true);*/

	//imageBoost = make_shared<ImageWrapper>(gameWrapper->GetDataFolder().string() + "\\CustomUI\\Presets\\M8\\BoostM8.png", true, true);
	// ImageWrapper(std::string path, bool canvasLoad = false, bool ImGuiLoad = false);
	//imageBoost = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "Presets" / "M8" / "BoostM8Fond.png", false, false);

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});

}

void CustomUI::onUnload(){
	LOG("Plugin unloaded!");
}

void CustomUI::initValues() {

	presetPath = gameWrapper->GetDataFolder().string() + "\\CustomUI" + "\\Presets" + "\\presets.json";
	allPresets = loadPresets(presetPath);

	//string keyPreset = cvarManager->getCvar("CustomUI_choosenPresets").getStringValue();
	//changePreset(allPresets[keyPreset]);
}

void CustomUI::writeCvar() {
	cvarManager->executeCommand("writeconfig", false);
}
//
//void CustomUI::changePreset(Preset preset, string namePreset) {
//	CVarWrapper presetChoosenCvar = cvarManager->getCvar("CustomUI_choosenPresets");
//	if (!presetChoosenCvar) { return; }
//	presetChoosenCvar.setValue(namePreset);
//	writeCvar();
//	imageBoost[namePreset]->LoadForCanvas();
//	LOG("change preset to " + namePreset);
//}

map<string, Preset> CustomUI::loadPresets(const string& jsonFilePath) {
	map<string, Preset> presets;

	ifstream file(jsonFilePath);
	if (!file.is_open()) {
		LOG("Impossible d'ouvrir le fichier JSON !");
		//throw std::runtime_error("Impossible d'ouvrir le fichier JSON !");
	}

	json data;
	file >> data;

	LOG("Fichier JSON charge avec succes !");

	// Parcourir les presets dans le fichier JSON
	for (auto& [key, value] : data["presets"].items()) {
		LOG("Chargement du preset : " + key);

		Preset preset;
		preset.boostImage = value["boostImage"];
		preset.scoreImage = value["scoreImage"];
		preset.color = {
			value["color"][0],
			value["color"][1],
			value["color"][2],
			value["color"][3]
		};

		// Initialisation des images directement
		auto boostImagePath = gameWrapper->GetDataFolder() / "CustomUI" / "Presets" / preset.boostImage;
		if (std::filesystem::exists(boostImagePath)) {
			imageBoost[key] = std::make_shared<ImageWrapper>(boostImagePath, false, false);
			LOG("Boost image chargée : " + boostImagePath.string());
		}
		else {
			LOG("Boost image introuvable : " + boostImagePath.string());
		}

		auto scoreImagePath = gameWrapper->GetDataFolder() / "CustomUI" / "Presets" / preset.scoreImage;
		if (std::filesystem::exists(scoreImagePath)) {
			imageScore[key] = std::make_shared<ImageWrapper>(scoreImagePath, false, false);
			LOG("Score image chargée : " + scoreImagePath.string());
		}
		else {
			LOG("Score image introuvable : " + scoreImagePath.string());
		}

		// Affichage des valeurs du preset dans le log
		LOG("  Boost Image: " + preset.boostImage);
		LOG("  Score Image: " + preset.scoreImage);
		LOG("  Color: RGBA("
			+ to_string(preset.color[0]) + ", "
			+ to_string(preset.color[1]) + ", "
			+ to_string(preset.color[2]) + ", "
			+ to_string(preset.color[3]) + ")");

		// Ajout du preset à la map
		presets[key] = preset;
	}

	return presets;
}

void CustomUI::startGame() {
	LOG("starttttt");
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

void CustomUI::UpdateVars()
{
	boost = getBoostAmount();

	/*gameTime = (getGameTime() != -1) ? (std::to_string(getGameTime() / 60) + ":" + lead_zeros(getGameTime() % 60, 2)) : std::to_string(-1);

	myTeamColor = getTeamColor(getMyTeam());
	opposingTeamColor = getTeamColor(getOpposingTeam());*/
}

bool CustomUI::zeroBoost(int boost) {
	if (boost == -1) {
		return true;
	}
	else {
		return false;
	}
}

void CustomUI::positionBoostBar(int selected) {

	switch (selected) {
		case 0:
			positionBoostBarLRTB(1580, 1020, 1630, 780);
			isVertical = true;
			break;
		case 1:
			positionBoostBarLRTB(1800, 1000, 1650, 700);
			break;
		case 2:
			positionBoostBarLRTB(1630, 770, 1870, 720);
			isVertical = false;
			break;
		case 3:
			positionBoostBarLRTB(1600, 1050, 1920, 1000);
			break;
		default:
			break;
		}
}

void CustomUI::positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y) {
	boostBegin = { (int)(screenSize.X * (v1x / 1920.0)), (int)(screenSize.Y * (v1y / 1080.0)) };
	boostMax = { (int)(screenSize.X * (v2x / 1920.0)), (int)(screenSize.Y * (v2y / 1080.0)) };
	
	if (isVertical) {
		//imageBoost = imageBoostLeftJoucaz;
		boostEnd = { boostMax.X, (int)(boostBegin.Y + (boostMax.Y - boostBegin.Y) * boost / 100.0f) };
	}
	else {
		//imageBoost = imageBoostTopJoucaz;
		boostEnd = { (int)(boostBegin.X + (boostMax.X - boostBegin.X) * boost / 100.0f), boostMax.Y };
	}
	drawBoost();
}

void CustomUI::drawBoost() {
	
}

void CustomUI::Render(CanvasWrapper canvas) {

	UpdateVars();

	// defines colors in RGBA 0-255
	LinearColor colors;
	colors.R = 255;
	colors.G = 255;
	colors.B = 255;
	colors.A = 255;
	canvas.SetColor(colors);

	// sets position to top left
	// x moves to the right
	// y moves down
	// bottom right would be 1920, 1080 for 1080p monitors
	canvas.SetPosition(Vector2F{ 0.0, 0.0 });
	canvas.DrawString("Version " + std::string(plugin_version), 2.0, 2.0, false);

	// says hi
	// draws from the last set position
	// the two floats are text x and y scale
	// the false turns off the drop shadow
	

	screenSize = canvas.GetSize();

	if (!zeroBoost(boost)) {
		string keyPreset = cvarManager->getCvar("CustomUI_choosenPresets").getStringValue();		
		canvas.SetColor(allPresets[keyPreset].color[0], allPresets[keyPreset].color[1], allPresets[keyPreset].color[2], allPresets[keyPreset].color[3]);
		int position = cvarManager->getCvar("CustomUI_positionBoostBar").getIntValue();
		positionBoostBar(position);
		canvas.DrawRect(boostBegin, boostEnd);

		canvas.SetColor(colors);
		canvas.SetPosition(Vector2F{ 0.0, 0.0 });
		imageBoost[keyPreset]->LoadForCanvas();
		if (imageBoost[keyPreset]->IsLoadedForCanvas()) {
			canvas.DrawTexture(imageBoost[keyPreset].get(), 1);
		}
		else {
			LOG("Render pas loaded");
		}


		/*boostTextSize = canvas.GetStringSize(std::to_string(boost), (float)((int)((boostBoxSize.X / 24) * 10) / 10), (float)((int)((boostBoxSize.Y / 24) * 10) / 10));
		boostTextPosition.X = ((screenSize.X - (boostBoxSize.X / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.X / 2);
		boostTextPosition.Y = ((screenSize.Y - (boostBoxSize.Y / 2)) - (int)((screenSize.X * 3) / 100)) - (boostTextSize.Y / 2);

		canvas.SetPosition(boostTextPosition);
		if (boost > 80)
			canvas.SetColor(255, 215, 180, 255);
		else
			canvas.SetColor(255, 235, 130, 255);
		canvas.DrawString(std::to_string(boost), (float)((int)((boostBoxSize.X / 24) * 10) / 10), (float)((int)((boostBoxSize.Y / 24) * 10) / 10));*/
	}
	canvas.SetPosition(Vector2F{ 0.0, 0.0 });
	
	
}




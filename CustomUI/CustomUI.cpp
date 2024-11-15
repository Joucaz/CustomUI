#include "pch.h"
#include "CustomUI.h"
#include "Settings.h"

using namespace std;


BAKKESMOD_PLUGIN(CustomUI, "Custom UI", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

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

	gameWrapper->RegisterDrawable([this](CanvasWrapper canvas) {
		Render(canvas);
		});

	/*std::string imageLoc = gameWrapper->GetDataFolder().string();
	LOG("TestData" + imageLoc);*/
	imageBoostLeftJoucaz = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "BoostLeftJoucaz.png", true, true);
	imageBoostTopJoucaz = std::make_shared<ImageWrapper>(gameWrapper->GetDataFolder() / "CustomUI" / "BoostTopJoucaz.png", true, true);

	
}

void CustomUI::onUnload(){
	LOG("Plugin unloaded!");
}

void CustomUI::initValues() {

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
			positionBoostBarLRTB(1580, 1020, 1630, 780, true);
			break;
		case 1:
			positionBoostBarLRTB(1800, 1000, 1650, 700, true);
			break;
		case 2:
			positionBoostBarLRTB(1630, 770, 1870, 720, false);
			break;
		case 3:
			positionBoostBarLRTB(1600, 1050, 1920, 1000, false);
			break;
		default:
			break;
		}
}

void CustomUI::positionBoostBarLRTB(float v1x, float v1y, float v2x, float v2y, bool isVertical) {
	boostBegin = { (int)(screenSize.X * (v1x / 1920.0)), (int)(screenSize.Y * (v1y / 1080.0)) };
	boostMax = { (int)(screenSize.X * (v2x / 1920.0)), (int)(screenSize.Y * (v2y / 1080.0)) };
	
	if (isVertical) {
		boostEnd = { boostMax.X, (int)(boostBegin.Y + (boostMax.Y - boostBegin.Y) * boost / 100.0f) };
	}
	else {
		boostEnd = { (int)(boostBegin.X + (boostMax.X - boostBegin.X) * boost / 100.0f), boostMax.Y };
	}
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

	// says hi
	// draws from the last set position
	// the two floats are text x and y scale
	// the false turns off the drop shadow
	

	screenSize = canvas.GetSize();

	if (!zeroBoost(boost)) {

		int position = cvarManager->getCvar("CustomUI_positionBoostBar").getIntValue();
		positionBoostBar(position);

		canvas.DrawRect(boostBegin, boostEnd);

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
	canvas.DrawString("Version " + std::string(plugin_version), 2.0, 2.0, false);
	if (imageBoostLeftJoucaz->IsLoadedForCanvas()) {
		canvas.DrawTexture(imageBoostLeftJoucaz.get(), 1);
	}
}




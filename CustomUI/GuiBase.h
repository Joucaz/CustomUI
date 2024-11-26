#pragma once
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/plugin/pluginwindow.h"

class SettingsWindowBase : public BakkesMod::Plugin::PluginSettingsWindow
{
public:
	std::string GetPluginName() override {
		return "CustomUI";
	};
	void SetImGuiContext(uintptr_t ctx) override {
		ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
	};

};
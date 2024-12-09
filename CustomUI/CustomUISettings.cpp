#include "pch.h"
#include "CustomUI.h"
using namespace std;

void CustomUI::RenderSettings() {
    
    
            if (ImGui::Button("Open Menu"))
            {
                isSettingsOpen = true;
            }

            ImGui::Separator();

            ImGui::Text("Set A Bind To Open The Customizable Window");
            static char KeyBindInputBuf[200] = "F3";
            ImGui::InputText("##KeyBindInput", KeyBindInputBuf, IM_ARRAYSIZE(KeyBindInputBuf));

            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight() / 2), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("BindSet", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("To open the window, you can now press : %s", std::string(KeyBindInputBuf).c_str());
                ImGui::NewLine();

                if (ImGui::Button("OK", ImVec2(100.f, 25.f)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            if (ImGui::Button("Set Bind"))
            {
                gameWrapper->Execute([&](GameWrapper* gw) {
                    cvarManager->setBind(KeyBindInputBuf, "CustomUI_openSettings");
                    cvarManager->log("New key bind set : " + std::string(KeyBindInputBuf) + " -> CustomUI_openSettings");
                    });

                ImGui::OpenPopup("BindSet");
            }




    //ImGui::Separator();
    //ImGui::Text("Plugin made by Vync");
    // Commence l'affichage de la fenêtre ImGui
    /*ImGuiStyle& style = ImGui::GetStyle();
    UserStyle = style;

    style.FrameRounding = 1.5f;
    style.FramePadding = ImVec2{ 2.0f, 5.0f };
    style.FrameBorderSize = 0.5f;

    style.Colors[ImGuiCol_Button] = ImVec4{ 0.0f, 193.0f / 255.0f, 173.0f / 255.0f, 1.0f };
    style.Colors[ImGuiCol_CheckMark] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 1.0f };

    style.Colors[ImGuiCol_Border] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.2f };
    style.Colors[ImGuiCol_BorderShadow] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.0f };

    style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 1.0f };
    style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.0f / 255.0f, 198.0f / 255.0f, 173.0f / 255.0f, 0.75f };
    style.Colors[ImGuiCol_Tab] = ImVec4{ 1.0f, 1.0f, 1.0f, 0.2f };
    style.Colors[ImGuiCol_Header] = ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_FrameBg] = ImVec4{ 31.0f / 255.0f, 31.0f / 255.0f, 31.0f / 255.0f, 1.0f };

    style.TabRounding = 2.0f;
    style.FramePadding.x = 10.0f;*/
       //LOG("open : " + to_string(this->isWindowOpen_));
       //LOG("open : " + GetPluginName());
    
}

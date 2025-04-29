#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "process_helpers.h"
#include "patch_helpers.h"
#include "table.h"


int main(int argc, char* argv[]) {

    const std::string processName = "rpcs3";
    pid_t pid = getPidByName(processName);
    if (pid == -1) {
        std::cerr << "Process not found." << std::endl;
        return 1;
    }
    std::cout << "Found process: " << processName << " with PID: " << pid << std::endl;

    const std::string cheatTablePath = "Skate 3.CT";
    if (!loadCheatTable(cheatTablePath)) {
        std::cerr << "Failed to load cheat table." << std::endl;
        return 1;
    } else {
        std::cout << "Cheat table loaded successfully." << std::endl;
    }

    // Read default values for all cheats
    for (auto& cheat : cheatEntries) {
        float val = 0.0f;

        if (!ReadValueAsFloat(pid, cheat.address, cheat.variableType, val))
            continue;

        cheat.defaultValue = val;
        cheat.currentValue = val;
    }

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(260, 725, "<< CH3AT | LINUX VERSION >>", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(250, 600), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::Begin("<< CH3AT | LINUX VERSION >>");
        
        if (ImGui::Button("Reset All Cheats")) {
            for (auto& cheat : cheatEntries) {
                std::string strVal(cheat.stringValue, cheat.stringLength);

                bool success = WriteValueFromFloat(
                    pid,
                    cheat.address,
                    cheat.variableType,
                    cheat.defaultValue,
                    strVal
                );
        
                if (success) {
                    cheat.currentValue = cheat.defaultValue;
                    std::cout << "Reset " << cheat.description << " to " << cheat.defaultValue << std::endl;
                } else {
                    std::cerr << "Failed to reset " << cheat.description << std::endl;
                }
            }
        }

        for (auto& cheat : cheatEntries) {
            ImGui::PushID(cheat.description.c_str());
            ImGui::Text("%s", cheat.description.c_str());
        
            ImGui::InputFloat("##value", &cheat.currentValue);
        
            if (ImGui::Button("Patch")) {
                bool success = WriteValueFromFloat(
                    pid,
                    reinterpret_cast<uintptr_t>(cheat.address),
                    cheat.variableType,
                    cheat.currentValue,
                    std::string(cheat.stringValue, cheat.stringLength)
                );
            
                if (success)
                    std::cout << "Patched " << cheat.description << " to " << cheat.currentValue << std::endl;
                else
                    std::cerr << "Failed to patch " << cheat.description << std::endl;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("Reset")) {
                bool success = WriteValueFromFloat(
                    pid,
                    reinterpret_cast<uintptr_t>(cheat.address),
                    cheat.variableType,
                    cheat.defaultValue,
                    std::string(cheat.stringValue, cheat.stringLength)
                );
            
                if (success) {
                    cheat.currentValue = cheat.defaultValue;
                    std::cout << "Reset " << cheat.description << " to " << cheat.defaultValue << std::endl;
                } else {
                    std::cerr << "Failed to reset " << cheat.description << std::endl;
                }
            }
        
            ImGui::Separator();
            ImGui::PopID();
        }
        
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
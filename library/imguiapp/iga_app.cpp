#include "iga_app.h"

#include "iga_exec.h"

#include "imgui.h"
#include "implot.h"

#include <fmt/format.h>

#include <print>
#include <string>
#include <array>

#include <GLFW/glfw3.h>
#include <windows.h>

namespace iga::app {

void wait_for_debugger()
{
    std::println("Waiting for debugger to attach");
    while (!::IsDebuggerPresent()) {
        ::Sleep(250);
    }
    std::println("Debugger attached");
}


std::array<int, 4> getWindowLocation(GLFWwindow* window)
{
    std::array<int, 4> location;
    glfwGetWindowPos(window, &location[0], &location[1]);
    glfwGetWindowSize(window, &location[2], &location[3]);
    return location;
}

void restoreWindowLocation(GLFWwindow* window, std::array<int, 4> location)
{
    glfwSetWindowPos(window, location[0], location[1]);
    glfwSetWindowSize(window, location[2], location[3]);
}

Settings init_settings(iga::exec::State* state)
{
    static std::string ini;
#pragma warning(push)
#pragma warning(disable : 4996)
    auto local_app_data = getenv("LOCALAPPDATA");
    auto directory      = std::filesystem::path(local_app_data) / state->application_name;
#pragma warning(pop)

    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    ini = (directory / fmt::format("{}.ini", state->application_name)).string();

    auto& io       = ImGui::GetIO();
    io.IniFilename = ini.c_str();
    ImGui::LoadIniSettingsFromDisk(io.IniFilename);

    // clang-format off
    return Settings{
        .ini_file = directory / fmt::format("{}.ini", state->application_name),
        .json_file = directory / fmt::format("{}.json", state->application_name)
    };
    // clang-format on
}


void init_default_style()
{
    ImGui::StyleColorsDark();
    ImGuiStyle& style   = ImGui::GetStyle();
    style.FrameRounding = 6;
    style.FramePadding  = {10, 6};
    style.GrabRounding  = 6;
    style.ItemSpacing   = {8, 6};
    style.CellPadding   = {6, 4};
    style.WindowPadding = {12, 12};

    auto& heading_hovered = style.Colors[ImGuiCol_HeaderHovered];
    heading_hovered.w     = 60 / 255.0f;

    ImPlotStyle& plotStyle   = ImPlot::GetStyle();
    plotStyle.Use24HourClock = false;
    plotStyle.UseISO8601     = true;
    plotStyle.UseLocalTime   = true;
    plotStyle.LineWeight     = 1;
    plotStyle.FitPadding     = ImVec2(0.1f, 0.1f);
}


void render_menubar(iga::comp::MenuItem* menu)
{
    if (std::holds_alternative<iga::comp::MenuItem::Menu>(menu->data)) {
        if (ImGui::BeginMenuBar()) {
            for (auto& child : std::get<iga::comp::MenuItem::Menu>(menu->data).children) {
                child.render();
            }
            ImGui::EndMenuBar();
        }
    }
}

void render_dockspace(iga::comp::MenuItem* menu, ImGuiID* dockspace_id)
{
    // clang-format off
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    static ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoTitleBar;
    // clang-format on

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("##DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiIO& io   = ImGui::GetIO();
    *dockspace_id = ImGui::GetID("Application_DockSpace");
    ImGui::DockSpace(*dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (menu) {
        render_menubar(menu);
    }

    ImGui::End();
}


iga::comp::MenuItem::Menu create_debug_menu(DebugWindows& debug_windows)
{
    using iga::comp::MenuItem;
    // clang-format off
    return { {
#ifdef INCLUDE_DEBUG_FEATURES
        {"Mouse Overlay", MenuItem::Toggle{ &debug_windows.mouse_overlay }},
        {"Show FPS", MenuItem::Toggle{ &debug_windows.fps_overlay }},
        {"Metrics", MenuItem::Toggle{ &debug_windows.metrics }},
        {"Stack", MenuItem::Toggle{ &debug_windows.stack }},
        {"Style Editor", MenuItem::Toggle{ &debug_windows.style_editor }},
        {"ImGui Demo", MenuItem::Toggle{ &debug_windows.imgui_demo }},
        {"ImPlot Demo", MenuItem::Toggle{ &debug_windows.implot_demo }},
        {"Item Picker", MenuItem::Action{ [](void*) { ImGui::DebugStartItemPicker(); }}},
#endif
    } };
    // clang-format on
}

void render_debug_windows(DebugWindows* windows, iga::exec::State* state)
{
#ifdef INCLUDE_DEBUG_FEATURES
    if (windows->mouse_overlay) iga::comp::ShowMouseOverlay(&windows->mouse_overlay);
    if (windows->fps_overlay) iga::comp::ShowFPSOverlay(&windows->fps_overlay, state);
    if (windows->metrics) ImGui::ShowMetricsWindow(&windows->metrics);
    if (windows->stack) ImGui::ShowStackToolWindow(&windows->stack);
    if (windows->style_editor) {
        if (ImGui::Begin("Dear ImGui Style Editor", &windows->style_editor)) {
            ImGui::ShowStyleEditor();
            ImGui::End();
        }
    }
    if (windows->imgui_demo) ImGui::ShowDemoWindow(&windows->imgui_demo);
    if (windows->implot_demo) ImPlot::ShowDemoWindow(&windows->implot_demo);
#endif
}


} // namespace iga::app

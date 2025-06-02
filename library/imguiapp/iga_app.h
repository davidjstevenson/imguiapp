#pragma once

#include "iga_comp.h"
#include "iga_filesystem.h"

#include <nlohmann/json.hpp>
#include <filesystem>
#include <array>

namespace iga::exec {
struct State;
}

struct GLFWwindow;

namespace iga::app {

void wait_for_debugger();

struct Settings {
    std::filesystem::path ini_file;
    std::filesystem::path json_file;

    nlohmann::json json;

    void load()
    {
        if (std::filesystem::exists(json_file)) {
            auto data = iga::filesystem::read(json_file, true);
            json      = nlohmann::json::parse((const char*)data.data());
        }
    }

    void save() const
    {
        auto dump = json.dump(4);
        iga::filesystem::write(json_file, std::span<const std::byte>{(const std::byte*)dump.data(), dump.size()});
    }
};


std::array<int, 4> getWindowLocation(GLFWwindow* window);

void restoreWindowLocation(GLFWwindow* window, std::array<int, 4> location);

Settings init_settings(iga::exec::State* state);


void init_default_style();


void render_menubar(iga::comp::MenuItem* menu);

void render_dockspace(iga::comp::MenuItem* menu, ImGuiID* dockspace_id);


struct DebugWindows {
    bool mouse_overlay = false;
    bool fps_overlay   = false;
    bool metrics       = false;
    bool stack         = false;
    bool style_editor  = false;
    bool imgui_demo    = false;
    bool implot_demo   = false;
};

iga::comp::MenuItem::Menu create_debug_menu(DebugWindows& debug_windows);

void render_debug_windows(DebugWindows* windows, iga::exec::State* state);


} // namespace iga::app

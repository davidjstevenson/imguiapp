#pragma once

#include "imgui.h"


namespace iga::scope {

struct Window {
    Window(ImVec2 defaultSize = ImVec2{}, bool dock = true)
        : defaultSize(defaultSize)
    {}

    ~Window() { ImGui::End(); }
    bool Begin(const char* name, bool* p_open = nullptr, ImGuiWindowFlags flags = 0)
    {
        ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
        return ImGui::Begin(name, p_open, flags);
    }

    template <typename Func>
    std::optional<std::exception> Paint(Func func)
    {
        try {
            func();
            return {};
        }
        catch (const std::exception& e) {
            return e;
        }
    }

    ImVec2 defaultSize{128.f, 128.f};
};


struct TreeNodeW {
    bool open = false;
    TreeNodeW(const char* text, ImGuiTreeNodeFlags flags) { open = ImGui::TreeNodeEx(text, flags); }
    TreeNodeW(const std::string& text, ImGuiTreeNodeFlags flags)
        : TreeNodeW(text.c_str(), flags)
    {}

    ~TreeNodeW()
    {
        if (open) {
            ImGui::TreePop();
        }
    }
};


} // namespace iga::scope

namespace ImGuiScope = iga::scope;

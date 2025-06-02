#pragma once

#include "imgui.h"

#include <string>
#include <variant>
#include <vector>

namespace iga::exec {
struct State;
}

namespace iga::comp {

inline void VSpace(float height)
{
    ImGui::Dummy({height, 0});
}


void ShowAsOverlay(const std::string& title, bool* p_open, void (*render)(void*), void* data, int* corner);
void ShowMouseOverlay(bool* p_open);
void ShowFPSOverlay(bool* p_open, iga::exec::State* state);


struct RegexFilter {
    RegexFilter(const char* default_filter = "") {}
    bool Draw(const char* label = "Filter", float width = 0.0f) { return false; }
    bool PassFilter(const char* text, const char* text_end = nullptr) const { return false; }
    void Build() {}
    void Clear() {}
    bool IsActive() const { return false; }
};


struct MenuItem {
    struct Menu {
        std::vector<MenuItem> children = {};
    };
    struct Toggle {
        bool* selected       = nullptr;
        std::string shortcut = {};
    };
    struct Action {
        void (*callback)(void*) = nullptr;
        void* data              = nullptr;
        std::string shortcut    = {};
    };

    using Type = std::variant<Menu, Toggle, Action>;

    struct RenderLeaf {
        MenuItem& parent;

        void operator()(Menu& v)
        {
            if (ImGui::BeginMenu(parent.name.c_str(), parent.enabled)) {
                for (auto& child : v.children) {
                    child.render();
                }
                ImGui::EndMenu();
            }
        }
        void operator()(Toggle& v)
        {
            ImGui::MenuItem(parent.name.c_str(), v.shortcut.c_str(), v.selected, parent.enabled);
        }

        void operator()(Action& v)
        {
            if (ImGui::MenuItem(parent.name.c_str(), v.shortcut.c_str(), nullptr, parent.enabled)) {
                if (v.callback) {
                    v.callback(v.data);
                }
            }
        }
    };

    std::string name = {};
    Type data        = {};
    bool enabled     = true;

    void render() { std::visit(RenderLeaf{*this}, data); }
};


} // namespace iga::comp

namespace ImGuiComp = iga::comp;

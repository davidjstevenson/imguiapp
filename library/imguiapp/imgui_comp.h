#pragma once

#include "imgui.h"
#include "imgui_exec.h"

#include <variant>
#include <string>
#include <vector>


namespace iga::comp {

inline void VSpace(float height)
{
    ImGui::Dummy({height, 0});
}


inline void ShowAsOverlay(const std::string& title, bool* p_open, void (*render)(void*), void* data, int* corner)
{
    ImGuiIO& io                   = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking
                                    | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings
                                    | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (*corner != -1) {
        const float PAD               = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos               = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size              = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x       = (*corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y       = (*corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (*corner & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (*corner & 2) ? 1.0f : 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        ImGui::SetNextWindowViewport(viewport->ID);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
    if (ImGui::Begin(title.c_str(), p_open, window_flags)) {
        render(data);

        if (ImGui::BeginPopupContextWindow()) {
            if (ImGui::MenuItem("Custom", NULL, *corner == -1)) *corner = -1;
            if (ImGui::MenuItem("Top-left", NULL, *corner == 0)) *corner = 0;
            if (ImGui::MenuItem("Top-right", NULL, *corner == 1)) *corner = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, *corner == 2)) *corner = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, *corner == 3)) *corner = 3;
            if (p_open && ImGui::MenuItem("Close")) *p_open = false;
            ImGui::EndPopup();
        }
    }

    ImGui::End();
}

inline void ShowMouseOverlay(bool* p_open)
{
    static int corner = 3;
    ShowAsOverlay(
        "Mouse Overlay", p_open,
        [](void*) {
            ImGuiIO& io = ImGui::GetIO();
            if (ImGui::IsMousePosValid()) {
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            }
            else {
                ImGui::Text("Mouse Position: <invalid>");
            }
        },
        nullptr, &corner);
}

inline void ShowFPSOverlay(bool* p_open, iga::exec::State* state)
{
    static int corner = 3;
    ShowAsOverlay(
        "FPS", p_open,
        [](void* ptr) {
            auto state  = static_cast<iga::exec::State*>(ptr);
            ImGuiIO& io = ImGui::GetIO();

            if (state->idler.enableIdling) {
                ImGui::Text("Application FPS: %.1f (%.1f) [%.1f%s]", 1.f / io.DeltaTime, io.Framerate,
                            state->idler.fpsIdle, state->idler.isIdling ? "*" : "");
            }
            else {
                ImGui::Text("Application FPS: %.1f (%.1f)", 1.f / io.DeltaTime, io.Framerate);
            }

            ImGui::Text("%d vertices, %d indices (%d triangles)", io.MetricsRenderVertices, io.MetricsRenderIndices,
                        io.MetricsRenderIndices / 3);
        },
        state, &corner);
}


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

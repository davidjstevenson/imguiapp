#pragma once

#include "imgui.h"
#include "iga_time.h"

#include <ranges>
#include <string>
#include <string_view>
#include <chrono>

namespace ImGui {


inline void Text(const std::string& text)
{
    TextUnformatted(text.c_str(), text.c_str() + text.size());
}

inline void TextWrapped(const std::string& text)
{
    TextWrapped(text.c_str());
}

void TextWrapped(const std::string& text, float width);

inline bool Selectable(const std::string& label, bool* p_selected, ImGuiSelectableFlags flags = 0,
                       const ImVec2& size = ImVec2(0, 0))
{
    return Selectable(label.c_str(), p_selected, flags, size);
}

inline bool Selectable(const std::string& text, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg = {})
{
    return Selectable(text.c_str(), selected, flags, size_arg);
}

inline void SetTooltip(const std::string& text)
{
    SetTooltip(text.c_str());
}

inline void TextColored(const ImVec4& colour, const std::string& text)
{
    ImGui::TextColored(colour, text.c_str());
}

inline void TextTime(double tUTC, double now = 0.0)
{
    Text(iga::time::to_string(tUTC, now));
}

inline void TextTime(std::chrono::system_clock::time_point tUTC, double now = 0.0)
{
    Text(iga::time::to_string(iga::time::toDouble(tUTC), now));
}

inline bool SelectableTime(double tUTC, ImGuiSelectableFlags flags, double now = 0.0)
{
    bool selected = false;
    return Selectable(iga::time::to_string(tUTC, now), &selected, flags);
}
inline bool SelectableTime(std::chrono::system_clock::time_point tUTC, ImGuiSelectableFlags flags, double now = 0.0)
{
    bool selected = false;
    return Selectable(iga::time::to_string(iga::time::toDouble(tUTC), now), &selected, flags);
}

inline bool Button(const std::string& text, const ImVec2& size_arg)
{
    return Button(text.c_str(), size_arg);
}

template <typename T, typename Getter = std::identity>
inline bool ComboBox(std::string_view title, const T& values, int* index, Getter getter = {})
{
    bool changed = false;
    if (ImGui::BeginCombo(title.data(), *index >= 0 ? getter(values.at(*index)).c_str() : "---")) {
        for (const auto& [i, v] : std::ranges::views::enumerate(values)) {
            bool is_selected = *index == i;
            if (Selectable(getter(v), &is_selected, ImGuiSelectableFlags_None)) {
                *index  = static_cast<int>(i);
                changed = true;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}

template <typename T, typename Getter = std::identity>
inline bool FilterComboBox(std::string_view title, const T& values, int* index, Getter getter, ImGuiTextFilter& filter)
{
    bool changed = false;
    if (ImGui::BeginCombo(title.data(), *index >= 0 ? getter(values.at(*index)).c_str() : "---")) {

        auto items = std::ranges::views::enumerate(values) | std::ranges::views::filter([&filter](const auto& p) {
                         return filter.PassFilter(std::get<1>(p).c_str());
                     });

        for (const auto& [i, v] : items) {
            bool is_selected = *index == i;
            if (Selectable(getter(v), &is_selected, ImGuiSelectableFlags_None)) {
                *index  = static_cast<int>(i);
                changed = true;
            }

            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return changed;
}


bool Button(const std::string& text, const ImVec2& padding = {-1, -1}, const ImVec2& size_arg = {0, 0});
bool SmallButton(const std::string& text, const ImVec2& padding = {-1, -1});


} // namespace ImGui

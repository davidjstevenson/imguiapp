#include "iga_stl.h"


namespace ImGui {


void TextWrapped(const std::string& text, float width)
{
    PushTextWrapPos(width);
    Text(text);
    PopTextWrapPos();
}

bool Button(const std::string& text, const ImVec2& padding, const ImVec2& size_arg)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2{padding.x < 0 ? 10 : padding.x, padding.y < 0 ? 8 : padding.y});
    auto result = ImGui::Button(text.c_str(), size_arg);
    ImGui::PopStyleVar();
    return result;
}

bool SmallButton(const std::string& text, const ImVec2& padding)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2{padding.x < 0 ? 10 : padding.x, padding.y < 0 ? 8 : padding.y});
    auto result = ImGui::SmallButton(text.c_str());
    ImGui::PopStyleVar();
    return result;
}


} // namespace ImGui

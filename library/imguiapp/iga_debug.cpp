#include "iga_debug.h"

namespace iga::debug {

inline void CheckboxFlags_ImGuiChildFlags(const char* label, ImGuiChildFlags& flags)
{
    ImGui::PushID(label);
    ImGui::CheckboxFlags("Border", &flags, ImGuiChildFlags_Border);
    ImGui::CheckboxFlags("AlwaysUseWindowPadding", &flags, ImGuiChildFlags_AlwaysUseWindowPadding);
    ImGui::CheckboxFlags("ResizeX", &flags, ImGuiChildFlags_ResizeX);
    ImGui::CheckboxFlags("ResizeY", &flags, ImGuiChildFlags_ResizeY);
    ImGui::CheckboxFlags("AutoResizeX", &flags, ImGuiChildFlags_AutoResizeX);
    ImGui::CheckboxFlags("AutoResizeY", &flags, ImGuiChildFlags_AutoResizeY);
    ImGui::CheckboxFlags("AlwaysAutoResize", &flags, ImGuiChildFlags_AlwaysAutoResize);
    ImGui::CheckboxFlags("FrameStyle", &flags, ImGuiChildFlags_FrameStyle);
    ImGui::PopID();
}


inline void CheckboxFlags_ImGuiTableFlags(const char* label, ImGuiTableFlags& flags)
{
    ImGui::PushID(label);
    // ImGui::CheckboxFlags("", &flags, ImGuiTableFlags_);
    ImGui::PopID();
}

} // namespace iga::debug

namespace ImGuiDebug = iga::debug;

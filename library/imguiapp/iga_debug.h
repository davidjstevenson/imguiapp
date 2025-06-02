#pragma once

#include "imgui.h"

namespace iga::debug {

void CheckboxFlags_ImGuiChildFlags(const char* label, ImGuiChildFlags& flags);
void CheckboxFlags_ImGuiTableFlags(const char* label, ImGuiTableFlags& flags);

} // namespace iga::debug

namespace ImGuiDebug = iga::debug;

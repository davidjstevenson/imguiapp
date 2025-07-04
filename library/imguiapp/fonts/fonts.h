#pragma once

#include "imgui.h"
#include <map>
#include <array>
#include <string>
#include <span>
#include <set>
#include <optional>
#include <memory>

#include "imgui_impl_opengl3.h"

#include "IconsFontAwesome6.h"
#include "IconsForkAwesome.h"

extern const unsigned int Font_RobotoMono_compressed_size;
extern const unsigned int Font_RobotoMono_compressed_data[148096 / 4];

extern const unsigned int Font_OpenSans_compressed_size;
extern const unsigned int Font_OpenSans_compressed_data[423292 / 4];

extern const unsigned int Font_ForkAwesome_Regular_compressed_size;
extern const unsigned int Font_ForkAwesome_Regular_compressed_data[172744 / 4];

extern const unsigned int Font_FontAwesome_Solid_compressed_size;
extern const unsigned int Font_FontAwesome_Solid_compressed_data[239264 / 4];

extern const unsigned int Font_FontAwesome_Regular_compressed_size;
extern const unsigned int Font_FontAwesome_Regular_compressed_data[35252 / 4];


namespace iga::fonts {

struct Fonts;

struct FontData {
    FontData(Fonts* fonts, const unsigned int* d, const unsigned int s)
        : fonts(fonts)
        , data(d, s) {};

    Fonts* fonts{};
    ImFontConfig config{};
    std::optional<std::array<ImWchar, 3>> range = {};
    std::span<const unsigned int> data{};
    std::array<ImFont*, 64> ptrs{};

    std::set<uint8_t> needs;

    ImFont* get(uint8_t size);

    void generate()
    {
        ImGuiIO& io = ImGui::GetIO();
        for (auto size : needs) {
            ptrs[size] = io.Fonts->AddFontFromMemoryCompressedTTF(data.data(), static_cast<int>(data.size()), size,
                                                                  &config, range ? range->data() : nullptr);
        }
        needs.clear();
    }
};

struct Fonts {
    FontData OpenSans   = {this, Font_OpenSans_compressed_data, Font_OpenSans_compressed_size};
    FontData RobotoMono = {this, Font_RobotoMono_compressed_data, Font_RobotoMono_compressed_size};
    FontData FontAwesomeReg
        = {this, Font_ForkAwesome_Regular_compressed_data, Font_ForkAwesome_Regular_compressed_size};
    FontData FontAwesomeSolid = {this, Font_FontAwesome_Solid_compressed_data, Font_FontAwesome_Solid_compressed_size};
    FontData ForkAwesome = {this, Font_ForkAwesome_Regular_compressed_data, Font_ForkAwesome_Regular_compressed_size};

    Fonts()
    {
        OpenSans         = {this, Font_OpenSans_compressed_data, Font_OpenSans_compressed_size};
        RobotoMono       = {this, Font_RobotoMono_compressed_data, Font_RobotoMono_compressed_size};
        FontAwesomeReg   = {this, Font_ForkAwesome_Regular_compressed_data, Font_ForkAwesome_Regular_compressed_size};
        FontAwesomeSolid = {this, Font_FontAwesome_Solid_compressed_data, Font_FontAwesome_Solid_compressed_size};
        ForkAwesome      = {this, Font_ForkAwesome_Regular_compressed_data, Font_ForkAwesome_Regular_compressed_size};
    }

    bool dirty = false;
    void update()
    {
        if (!dirty) return;

        OpenSans.generate();
        RobotoMono.generate();
        FontAwesomeReg.generate();
        FontAwesomeSolid.generate();
        ForkAwesome.generate();

        dirty = false;
        ImGui_ImplOpenGL3_DestroyFontsTexture();
    };
};

std::shared_ptr<Fonts> init(std::span<uint8_t> sizes);

struct Font {
    int count = 0;
    ImFont* f{};

    Font(ImFont* font)
        : f(font)
    {
        push();
    }
    Font(FontData& font, uint8_t size)
        : f(font.get(size))
    {
        if (f) push();
    }
    void push()
    {
        ImGui::PushFont(f);
        count++;
    };
    void pop()
    {
        ImGui::PopFont();
        count--;
    }
    ~Font()
    {
        if (count > 0) pop();
    }
};

} // namespace iga::fonts

#include "fonts.h"
#include <memory>

namespace iga::fonts {


ImFont* FontData::get(uint8_t size)
{
    if (size == 0 || size >= 64) {
        return nullptr;
    }
    auto font = ptrs[size];
    if (font == nullptr) {
        needs.insert(size);
        fonts->dirty = true;
    }
    return font;
}

std::shared_ptr<Fonts> init(std::span<uint8_t> sizes)
{
    ImFontConfig icons_config;
    icons_config.MergeMode  = true;
    icons_config.PixelSnapH = true;

    auto fonts = std::make_shared<Fonts>();

    fonts->FontAwesomeReg.config   = icons_config;
    fonts->FontAwesomeSolid.config = icons_config;
    fonts->ForkAwesome.config      = icons_config;

    fonts->FontAwesomeReg.range   = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    fonts->FontAwesomeSolid.range = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    fonts->ForkAwesome.range      = {ICON_MIN_FK, ICON_MAX_16_FK, 0};

    for (auto s : sizes) {
        if (s <= 0) continue;

        fonts->OpenSans.get(s);
        fonts->RobotoMono.get(s);
        fonts->FontAwesomeReg.get(s);
        fonts->FontAwesomeSolid.get(s);
        fonts->ForkAwesome.get(s);
    }

    fonts->update();
    return fonts;
}


} // namespace iga::fonts

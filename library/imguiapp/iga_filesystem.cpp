#include "iga_filesystem.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "nfd.h"
#include "nfd_glfw3.h"

#include <fmt/format.h>
#include <fstream>


namespace iga::filesystem {

std::vector<std::byte> read(const std::filesystem::path& path, bool add_null_termination)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
        throw std::runtime_error(fmt::format("failed to open {}", path.string()));
    }

    ifs.seekg(0, std::ios::end);
    std::streampos length = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<std::byte> buffer(static_cast<int>(length) + (add_null_termination ? 1 : 0));
    if (length > 0) {
        ifs.read(reinterpret_cast<char*>(buffer.data()), length);
    }

    if (add_null_termination) {
        buffer.push_back(std::byte{0x00});
    }

    return buffer;
}

void prep_for_write(const std::filesystem::path& file, bool remove_if_exists)
{
    if (file.has_parent_path()) {
        auto parent = file.parent_path();
        if (!std::filesystem::exists(parent)) {
            std::filesystem::create_directories(parent);
        }
    }

    if (remove_if_exists && std::filesystem::exists(file) && std::filesystem::is_directory(file)) {
        std::filesystem::remove(file);
    }
}

void write(const std::filesystem::path& path, std::span<const std::byte> data)
{
    prep_for_write(path, true);

    std::ofstream ofs(path, std::ios::binary);
    if (!ofs.is_open()) {
        throw std::runtime_error(fmt::format("failed to open {}", path.string()));
    }
    ofs.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
}

std::optional<std::filesystem::path> select_folder_dialog(const std::filesystem::path& current, GLFWwindow* window)
{
    std::optional<std::filesystem::path> output;
    auto current_path = current.string();

    nfdu8char_t* outPath = nullptr;

    nfdpickfolderu8args_t args;
    args.defaultPath = current_path.c_str();

    NFD_GetNativeWindowFromGLFWWindow(window, &args.parentWindow);

    auto result = NFD_PickFolderU8_With(&outPath, &args);

    if (NFD_OKAY == result) {
        output = std::filesystem::path(outPath);
        NFD_FreePathU8(outPath);
    }
    return output;
}

void init()
{
    NFD_Init();
}
void deinit()
{
    NFD_Quit();
}

} // namespace iga::filesystem

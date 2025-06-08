#pragma once
#include "imgui.h"

#include "iga_stl.h"

#include <fmt/format.h>

#include <filesystem>
#include <variant>
#include <vector>
#include <span>
#include <cassert>

struct GLFWwindow;

namespace iga::filesystem {
struct Size {
    size_t bytes;
};
}; // namespace iga::filesystem

template <>
struct fmt::formatter<iga::filesystem::Size> : formatter<string_view> {

    inline auto format(iga::filesystem::Size size, format_context& ctx) const
    {
        if (size.bytes <= 1024) {
            return formatter<string_view>::format(fmt::format("{}", size.bytes), ctx);
        }

        const char* suffix[] = {"KiB", "MiB", "GiB"};

        auto index   = 0u;
        double value = static_cast<double>(size.bytes) / 1024.0;
        for (; index < 2 && value > 1024.0; index++) {
            value /= 1024.0;
        }

        return formatter<string_view>::format(fmt::format("{:.3f} {}", value, suffix[index]), ctx);
    }
};


namespace iga::filesystem {

void init();
void deinit();

std::vector<std::byte> read(const std::filesystem::path& path, bool add_null_termination);
void write(const std::filesystem::path& path, std::span<const std::byte> data);

std::optional<std::filesystem::path> select_folder_dialog(const std::filesystem::path& current,
                                                          GLFWwindow* window = nullptr);

struct Selection {
    std::filesystem::path selection;
    enum class Action { Single, Double } action;
};

struct Folder;
struct Root;

struct File {

    File() = default;
    File(std::filesystem::path path, Root* root)
        : path(std::move(path))
        , root(root)
    {
        assert(root != nullptr);
    }

    void refresh()
    {
        stem        = fmt::format("{}{}", path.stem().string(), path.extension().string());
        stem_no_ext = path.stem().string();
        size.bytes  = static_cast<size_t>(std::filesystem::file_size(path));
        dirty       = false;
    }
    std::filesystem::path path;
    std::string stem;
    std::string stem_no_ext;
    Size size;
    bool dirty = true;
    Root* root;

    std::optional<Selection> render();
};

struct ItemVisitor {
    std::optional<Selection> selection;

    template <typename T>
    void operator()(T& i)
    {
        auto f = i.render();
        if (f) {
            selection = f;
        }
    }
};

struct Folder {
    Folder() = default;
    Folder(std::filesystem::path path, Root* root)
        : path(std::move(path))
        , root(root)
    {
        assert(root != nullptr);
    }

    void set(std::filesystem::path path)
    {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            this->path = std::move(path);
            dirty      = true;
            refresh();
        }
    }

    void refresh()
    {
        stem = path.stem().string();
        children.clear();

        for (const std::filesystem::directory_entry& dir_entry : std::filesystem::directory_iterator(path)) {
            auto& i = dir_entry.path();
            if (std::filesystem::is_regular_file(i)) {
                children.push_back(File{i, root});
            }
            else if (std::filesystem::is_directory(i)) {
                children.push_back(Folder{i, root});
            }
        }

        dirty = false;
    }

    std::filesystem::path path;
    Root* root;

    std::string stem;
    std::vector<std::variant<Folder, File>> children;
    bool dirty = true;

    std::optional<Selection> render(bool* force_open = nullptr);
};

struct Root {
    Root() = default;
    Root(std::filesystem::path path)
        : folder(std::move(path), this)
    {}

    bool file_filter(const std::string& stem) const { return filter.PassFilter(stem.c_str()); }
    void set(std::filesystem::path path) { folder.set(std::move(path)); }
    std::optional<Selection> render(bool* force_open = nullptr) { return folder.render(force_open); }
    const std::filesystem::path& path() const { return folder.path; }

    Folder folder;
    std::optional<std::filesystem::path> extension;

    ImGuiTextFilter filter;
    std::optional<std::filesystem::path> next;
};


} // namespace iga::filesystem

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

struct File {

    void refresh()
    {
        stem       = fmt::format("{}{}", path.stem().string(), path.extension().string());
        size.bytes = static_cast<size_t>(std::filesystem::file_size(path));
        dirty      = false;
    }
    std::filesystem::path path;
    std::string stem;
    Size size;
    bool dirty = true;

    void render()
    {
        static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns;
        ImGuiTreeNodeFlags node_flags                  = tree_node_flags_base;

        if (dirty) refresh();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::TreeNodeEx(stem.c_str(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
        ImGui::TableNextColumn();
        ImGui::Text(fmt::format("{}", size));
    };
};

struct ItemVisitor {
    template <typename T>
    void operator()(T& i)
    {
        i.render();
    }
};

struct Folder {
    Folder() = default;
    Folder(std::filesystem::path path, Folder* root)
        : path(std::move(path))
        , root(root)
    {
        assert(root != nullptr);
    }

    Folder(std::filesystem::path path)
        : path(std::move(path))
    {
        root = this;
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
                children.push_back(File{i});
            }
            else if (std::filesystem::is_directory(i)) {
                children.push_back(Folder{i, root});
            }
        }

        dirty = false;
    }

    std::filesystem::path path;
    Folder* root;
    std::optional<std::filesystem::path> next;

    std::string stem;
    std::vector<std::variant<Folder, File>> children;
    bool dirty = true;

    void render(bool* force_open = nullptr)
    {
        static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns;
        ImGuiTreeNodeFlags node_flags                  = tree_node_flags_base;

        if (dirty) refresh();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (force_open != nullptr) {
            ImGui::SetNextItemOpen(*force_open);
        }

        bool open = ImGui::TreeNodeEx(stem.c_str(), node_flags);
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) root->next = path;
            if (ImGui::IsKeyPressed(ImGuiKey_AppBack) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
                root->next = path.parent_path();
        }

        ImGui::TableNextColumn();
        ImGui::TextDisabled("--");

        if (open) {
            ItemVisitor visitor{};
            for (auto& i : children) {
                std::visit(visitor, i);
            }
            ImGui::TreePop();
        }
    };
};

} // namespace iga::filesystem

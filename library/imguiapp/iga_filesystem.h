#pragma once
#include "imgui.h"

#include <fmt/format.h>

#include <filesystem>
#include <variant>
#include <vector>
#include <span>
#include <cassert>

namespace iga::filesystem {

std::vector<std::byte> read(const std::filesystem::path& path, bool add_null_termination);
void write(const std::filesystem::path& path, std::span<const std::byte> data);



struct File {
    void refresh() {
        stem = fmt::format("{}{}", path.stem().string(), path.extension().string());
        dirty = false;
    }
    std::filesystem::path path;
    std::string stem;
    bool dirty = true;

    void render() {
        static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns;
        ImGuiTreeNodeFlags node_flags = tree_node_flags_base;

        if (dirty) refresh();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        ImGui::TreeNodeEx(stem.c_str(), node_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
        ImGui::TableNextColumn();
        ImGui::Text("%d", 10);
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(path.extension().string().c_str());
    };
};

struct ItemVisitor {
    template <typename T>
    void operator()(T& i) { i.render(); }
};

struct Folder {
    Folder() = default;
    Folder(std::filesystem::path path, Folder* root)
        : path(std::move(path)), root(root)
    {
        assert(root != nullptr);
    }

    Folder(std::filesystem::path path): path(std::move(path))
    {
        root = this;
    }

    void set(std::filesystem::path path) {
        this->path = std::move(path);
        dirty = true;
        refresh();
    }

    void refresh() {
        stem = path.stem().string();
        children.clear();

        for (const std::filesystem::directory_entry& dir_entry :
            std::filesystem::directory_iterator(path)) {
            auto& i = dir_entry.path();
            if (std::filesystem::is_regular_file(i)) {
                children.push_back(File{ i });
            } else if (std::filesystem::is_directory(i)) {
                children.push_back(Folder{ i, root });
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

    void render(bool* force_open = nullptr) {
        static ImGuiTreeNodeFlags tree_node_flags_base = ImGuiTreeNodeFlags_SpanAllColumns;
        ImGuiTreeNodeFlags node_flags = tree_node_flags_base;

        if (dirty) refresh();

        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (force_open != nullptr) {
            ImGui::SetNextItemOpen(*force_open);
        }

        bool open = ImGui::TreeNodeEx(stem.c_str(), node_flags);
        if (ImGui::IsItemHovered()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
                root->next = path;
            if (ImGui::IsKeyPressed(ImGuiKey_AppBack) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
                root->next = path.parent_path();
        }

        ImGui::TableNextColumn();
        ImGui::TextDisabled("--");
        ImGui::TableNextColumn();
        ImGui::TextUnformatted("");

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

#pragma once

#include <filesystem>
#include <vector>
#include <span>

namespace iga::filesystem {

std::vector<std::byte> read(const std::filesystem::path& path, bool add_null_termination);
void write(const std::filesystem::path& path, std::span<const std::byte> data);

} // namespace iga::filesystem

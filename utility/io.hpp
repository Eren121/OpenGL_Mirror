#pragma once

#include <string>
#include <filesystem>

namespace io
{
    /// @brief Read all the content of a file into a string.
    /// @remarks Returns an empty string and log to std::err if there is an error.
    std::string readAll(const std::filesystem::path& path);
}


#pragma once

/**
 * @file plugin.hpp
 * @brief The C++ API exposed to the end user
 *
 */

#include <cstdint>
#include <filesystem>
#include <memory>
#include <plugin_api.h>
#include <string>
#include <variant>

using PluginResult = std::variant<int32_t, std::string>;

/**
 * @brief A C++ interface to the C plugin API.
 *
 * Resources are managed by RAII: whatever the constructor acquires is released
 * by the destructor.
 */
class Plugin {
  public:
    /**
     * @brief Open the shared library at @p path and create a plugin instance.
     *
     * Opens the library with dlopen(), finds the plugin functions with dlsym(),
     * then calls plugin_create().
     *
     * @throws std::runtime_error if the library cannot be opened, a required
     *         symbol is missing, or plugin_create() fails.
     */
    explicit Plugin(std::filesystem::path path);

    /**
     * @brief Destroy the plugin instance and close the shared library.
     */
    ~Plugin();

    /**
     * @brief Run the plugin on @p input.
     *
     * Converts the C result into a variant, so callers can use std::visit
     * instead of inspecting the plugin_type field themselves.
     *
     * @throws std::runtime_error if plugin_execute() returns a non-OK status.
     */
    PluginResult execute(const char *input) const;

  private:
    /// Private implementation ("pimpl") holding the plugin handle and the
    /// function pointers. It keeps <dlfcn.h> out of this header and out of the
    /// caller's source code.
    struct Impl;
    std::unique_ptr<Impl> _impl;
};

/**
 * @file plugin.hpp
 * @brief The C++ API exposed to the end user
 * 
 */

 #include <filesystem>
#include <plugin_api.h>
#include <variant>


using PluginResult = std::variant<int32_t, std::string>;

class Plugin {
public:
    explicit Plugin(std::filesystem::path path);
    ~Plugin();
    PluginResult execute(const char *input) const;

private:
    struct Impl;
    std::unique_ptr<Impl> _impl;

};


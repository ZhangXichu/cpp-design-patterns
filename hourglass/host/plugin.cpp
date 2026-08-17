#include "plugin_api.h"
#include <dlfcn.h>

#include <plugin.hpp>

using create_fn = plugin_handle *(*)(void);
using destroy_fn = void (*)(plugin_handle *);
using execute_fn = status (*)(plugin_handle *, const char *, plugin_result *);

struct Plugin::Impl {
    void *lib = nullptr;
    plugin_handle *h = nullptr;
    create_fn create = nullptr;
    destroy_fn destroy = nullptr;
    execute_fn execute = nullptr;

    ~Impl() {
        if (h && destroy) {
            destroy(h);
        }
        if (lib) {
            dlclose(lib);
        }
    }
};

Plugin::Plugin(std::filesystem::path path) : _impl(std::make_unique<Impl>()) {
    // RTLD_LOCAL keeps the plugin's symbols out of the global lookup scope, so
    // every plugin can export the same function names without collisions.
    _impl->lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!_impl->lib) {
        throw std::runtime_error(dlerror());
    }

    auto load_symbol = [](void *library, const char *name) {
        dlerror(); // Clear any previous error
        void *symbol = dlsym(library, name);
        if (const char *error = dlerror()) {
            throw std::runtime_error(std::string("Cannot load symbol ") + name +
                                     ": " + error);
        }
        return symbol;
    };

    _impl->create = (create_fn)load_symbol(_impl->lib, "plugin_create");
    _impl->destroy = (destroy_fn)load_symbol(_impl->lib, "plugin_destroy");
    _impl->execute = (execute_fn)load_symbol(_impl->lib, "plugin_execute");
    _impl->h = _impl->create();
    if (!_impl->h) {
        throw std::runtime_error("Plugin creation failed");
    }
}

Plugin::~Plugin() = default;

PluginResult Plugin::execute(const char *input) const {
    plugin_result result{};
    status s = _impl->execute(_impl->h, input, &result);
    switch (s) {
    case status::OK:
        break;
    case status::INVALID_INPUT:
        throw std::runtime_error("Invalid input. Should not be null ptr.");
    case status::OTHER_ERR:
        throw std::runtime_error("Plugin execution failed");
    default:
        throw std::runtime_error("Plugin returned an unknown status");
    }

    switch (result.type) {
    case PLUGIN_TYPE_INTEGER:
        return result.value.val_int;
    case PLUGIN_TYPE_STRING:
        return std::string(result.value.val_str.data, result.value.val_str.len);
    default:
        throw std::runtime_error("invalid plugin result type");
    }
    return {};
}
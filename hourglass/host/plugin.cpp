#include "plugin_api.h"
#include <dlfcn.h>

#include <plugin.hpp>

using create_fn = plugin_handle *(*)(void);
using destroy_fn = void(*)(plugin_handle *);
using execute_fn = int32_t(*)(plugin_handle *, const char *, plugin_result *);

struct Plugin::Impl {
    void* lib;
    plugin_handle* h;
    create_fn create;
    destroy_fn destroy;
    execute_fn execute;
};

Plugin::Plugin(std::filesystem::path path) : _impl(std::make_unique<Impl>()) {
    _impl->lib = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!_impl->lib) {
        throw std::runtime_error(dlerror());
    }
    _impl->create = (create_fn)dlsym(_impl->lib, "plugin_create");
    _impl->destroy = (destroy_fn)dlsym(_impl->lib, "plugin_destroy");
    _impl->execute = (execute_fn)dlsym(_impl->lib, "plugin_execute");
    _impl->h = _impl->create();
}

Plugin::~Plugin() {
    _impl->destroy(_impl->h);
    dlclose(_impl->lib);
}

PluginResult Plugin::execute(const char *input) const {
    plugin_result result{};
    _impl->execute(_impl->h, input, &result);
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
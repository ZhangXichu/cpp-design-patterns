#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif

#define PLUGIN_EXPORT __attribute__((visibility("default")))

typedef enum plugin_type {
    PLUGIN_TYPE_STRING,
    PLUGIN_TYPE_INTEGER
} plugin_type;

typedef struct plugin_result {
    plugin_type type;
    union {
        struct val_str {const char *data; int32_t len;} val_str;
        int32_t val_int;
    } value;
} plugin_result;

typedef struct plugin_handle plugin_handle;

PLUGIN_EXPORT plugin_type plugin_result_type(void);

PLUGIN_EXPORT plugin_handle *plugin_create(void);
PLUGIN_EXPORT void plugin_destroy(plugin_handle *h);
PLUGIN_EXPORT int32_t plugin_execute(plugin_handle *h, const char *input, plugin_result *out);

#ifdef __cplusplus
}
#endif

#endif

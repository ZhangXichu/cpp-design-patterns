#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PLUGIN_EXPORT __attribute__((visibility("default")))

typedef enum plugin_type {
    PLUGIN_TYPE_STRING,
    PLUGIN_TYPE_INTEGER
} plugin_type;

/**
 * @brief Outcome of a plugin call.
 *
 * Status codes are returned instead of exceptions, which must never cross the C
 * boundary.
 */
typedef enum status { INVALID_INPUT, OTHER_ERR, OK } status;

/**
 * @brief Result of a plugin call: either a string or a 32-bit integer.
 *
 * The @c type field identifies which union member is valid.
 *
 * Ownership: the plugin owns any returned string. It stays valid only until the
 * next plugin_execute() or plugin_destroy() call on the same handle, so the host
 * must copy it to keep it longer.
 */
typedef struct plugin_result {
    plugin_type type;
    union {
        struct val_str {
            const char *data;
            int32_t len;
        } val_str;
        int32_t val_int;
    } value;
} plugin_result;

/**
 * @brief Opaque plugin instance. Its layout is private to the plugin.
 */
typedef struct plugin_handle plugin_handle;

/**
 * @brief Create a plugin instance.
 * @return The new instance, or NULL on failure. Catches all C++ exceptions.
 */
PLUGIN_EXPORT plugin_handle *plugin_create(void);

/**
 * @brief Destroy an instance created by plugin_create().
 */
PLUGIN_EXPORT void plugin_destroy(plugin_handle *h);

/**
 * @brief Run the plugin on @p input and write the result to @p out.
 * @return #OK on success, #INVALID_INPUT if any argument is NULL, or #OTHER_ERR
 *         if an exception occurs or the result cannot be represented.
 *         Catches all C++ exceptions.
 */
PLUGIN_EXPORT status plugin_execute(plugin_handle *h, const char *input,
                                    plugin_result *out);

#ifdef __cplusplus
}
#endif

#endif

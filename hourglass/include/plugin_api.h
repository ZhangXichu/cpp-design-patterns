#ifndef PLUGIN_API_H
#define PLUGIN_API_H

struct plugin_handle;

#ifdef __cplusplus
extern "C" {
#endif

plugin_handle *plugin_create();
void destory_handle(plugin_handle *plugin);
int plugin_execute(plugin_handle *plugin, const char *input, char *output,
                   std::size_t output_size);

#ifdef __cplusplus
}
#endif

#endif

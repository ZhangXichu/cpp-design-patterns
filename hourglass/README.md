## Hourglass Pattern

A plugin loading program is used to show the usage of the hourglass pattern. The following is the high level structure of it:

```
      Host application
                       C++
                        │
                        │ nice C++ wrapper
                        ▼
              ┌───────────────────┐
              │   plugin_api.h    │
              │     PURE C API    │  ← narrow waist
              └───────────────────┘
                  ▲           ▲
                  │           │
          to_upper_case.so  word_count.so
                  │           │
                 C++         C++
```

where a C API layer is between the C++ interface for the user and the C++ internal implementation of the plugins. Therefore those two C++ parts do not need to agree on an ABI.

Use build.sh to compile the plugins into .so.
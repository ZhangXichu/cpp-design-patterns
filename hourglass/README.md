## Hourglass Pattern

A plugin-loading program is used to show the usage of the hourglass pattern. Its high-level structure is the following:

```
      Host application
                       C++
                        │
                        │ nice C++ wrapper
                        ▼
              ┌───────────────────┐
              │   plugin_api.h    │
              │     PURE C API    │  ← the narrow layer
              └───────────────────┘
                  ▲           ▲
                  │           │
          to_upper_case.so  word_count.so
                  │           │
                 C++         C++
```

where a C API layer sits between the C++ interface for the user and the C++ internal implementation of the plugins. Therefore, those two C++ parts do not need to agree on an ABI.

The C API [`include/plugin_api.h`](include/plugin_api.h) is included in both the plugins and the C++ interface for the user ([`host/plugin.cpp`](host/plugin.cpp)).

### What is the use of the C API layer?

C++ has no stable ABI. Name mangling, exception propagation, vtable layout and the
representation of standard library types all vary between compilers, compiler versions and build
flags, so none of them can be relied on across a dynamic-library boundary. C does have a stable
platform ABI, so the shared layer is deliberately kept to what C can express: plain functions,
opaque handles, simple structs and status codes.

That narrowness is what buys the freedom above and below it. The host is idiomatic modern C++,
each plugin is written in whatever C++ its author prefers, and neither side ever sees the other's
types. A plugin can be rebuilt with a different compiler, and the host keeps working.

### Layout

| Path | Role |
| --- | --- |
| [include/plugin_api.h](include/plugin_api.h) | The narrow layer: the pure C ABI, the only thing both sides share |
| [host/plugin.hpp](host/plugin.hpp) | The C++ API the end user programs against |
| [host/plugin.cpp](host/plugin.cpp) | The wrapper that loads a plugin and adapts C results to C++ types |
| [plugins/](plugins/) | `to_upper_case` (string result) and `word_count` (integer result) |
| [main.cpp](main.cpp) | Demo host application |
| [build.sh](build.sh) | Compiles every `plugins/*.cpp` into `libs/<name>.so` |

### The two sides

The C layer offers the minimum a plugin needs: create an instance, run it on some input, destroy
it. Results come back through a tagged union, and errors come back as status codes.

The wrapper hides all of that. `Plugin` loads a shared library in its constructor and releases it
in its destructor, and its `execute` returns a value the caller can inspect without touching the
raw union or checking return codes by hand. The user of the library never includes the C header
or deals with dynamic loading.

The doc comments in [plugin_api.h](include/plugin_api.h) and [plugin.hpp](host/plugin.hpp) cover
the contracts in detail: result ownership, status codes, and what throws when.

### Building and running

```bash
./build.sh                       # plugins -> libs/*.so
cmake -S . -B build && cmake --build build
./build/hourglass                # run from this directory
```

Output:

```
libs/to_upper_case.so -> string "THE QUICK BROWN FOX"
libs/word_count.so -> integer 4
```

The plugins are built by a plain `g++` script rather than by the CMake project on purpose: they
are separate artifacts the host knows nothing about at compile time, and it is easy to rebuild
one with a different compiler to check that the boundary really is stable.

### Known gaps

Since this is a demo project, there are still things to be improved for it to be more production-ready:

* **Versioning** — no version negotiation; a real hourglass exports `plugin_abi_version()` and
  refuses incompatible plugins.
* **Struct padding** — `plugin_result`'s layout is not pinned explicitly.

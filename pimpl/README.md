## Pimpl Pattern

A Linux file-watching program is used to show the usage of the Pimpl (pointer to implementation) pattern. Its high-level structure is the following:

```
             User application
                    │
                    │ includes
                    ▼
          ┌─────────────────────┐
          │  file_watcher.hpp   │
          │ FileWatcher + impl* │  <- stable public interface
          └─────────────────────┘
                    │
                    │ owns std::unique_ptr<impl>
                    ▼
          ┌─────────────────────┐
          │  file_watcher.cpp   │
          │ FileWatcher::impl   │  <- private implementation
          └─────────────────────┘
                    │
                    ▼
             Linux inotify API
```

The public `FileWatcher` class contains only a pointer to its implementation. The implementation type is forward-declared in [`file_watcher.hpp`](file_watcher.hpp) and defined in [`file_watcher.cpp`](file_watcher.cpp), so its data members and Linux-specific machinery remain outside the public class definition.

### What is the use of the Pimpl layer?

Ordinarily, changing a private data member changes the size and layout of a C++ class. Every source file that includes that class definition may then need to be recompiled, and a separately compiled client may no longer agree with the library about the class layout.

Pimpl gives `FileWatcher` a small, stable representation: one `std::unique_ptr<impl>`. Implementation details such as the callback, running state, inotify descriptors and polling loop can change without appearing in the public class definition. It also reduces the implementation details and platform headers exposed to users of the class.

The destructor is declared in the header and defined out of line in `file_watcher.cpp`. At that definition, `impl` is complete, allowing `std::unique_ptr<impl>` to destroy the hidden object correctly.

### Layout

| Path | Role |
| --- | --- |
| [file_watcher.hpp](file_watcher.hpp) | Public `FileWatcher` interface and forward declaration of `impl` |
| [file_watcher.cpp](file_watcher.cpp) | Hidden implementation based on Linux inotify and `poll()` |
| [main.cpp](main.cpp) | Demo application, event logging and signal-driven shutdown |
| [test_path/](test_path/) | Directory monitored by the example |
| [CMakeLists.txt](CMakeLists.txt) | Builds the file-watcher library and demo executable |

### The two sides

The public side offers three operations: install an event callback, start watching and stop watching. A caller can use those operations without knowing which descriptors, flags or buffers the watcher stores internally.

The private `FileWatcher::impl` creates a nonblocking inotify descriptor, watches `test_path`, and uses `poll()` to wait for readable events. The finite polling timeout lets the loop periodically check the atomic running flag. The demo blocks `SIGINT` and `SIGTERM`, waits for either signal with `sigwait()`, calls `stop()` from normal thread context, and then joins the worker thread.

### Building and running

Run the commands from this directory because the watched path is relative to it:

```bash
cmake -S . -B build && cmake --build build
./build/pimpl
```

Create, edit, move or remove files under `test_path` from another terminal. Example output:

```
created: example.txt
modified: example.txt
deleted: example.txt
```

Press Ctrl+C to stop the watcher:

```
^C
Stopping file watcher...
```

### Known gaps

Since this is a demo project, there are still things to be improved for it to be more production-ready:

* **Linux-only implementation** — the implementation uses inotify, and the callback exposes `inotify_event`; a fully platform-independent interface would translate it into a library-owned event type.
* **Fixed path and event mask** — `test_path` and the selected events are hard-coded instead of being supplied through the public API.
* **Error handling** — failures from inotify setup, `poll()`, `read()` and signal functions should be reported consistently.
* **Lifecycle contract** — repeated or concurrent calls to `start()` and `stop()` are not defined.
* **Move semantics** — move-in and move-out events are presented as separate directory-level changes rather than being paired using the inotify cookie.

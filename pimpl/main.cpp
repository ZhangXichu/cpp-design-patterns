#include "file_watcher.hpp"

#include <iostream>
#include <sys/inotify.h>

int main() {
    FileWatcher watcher;
    watcher.on_change([](struct inotify_event *event) {
        if (event->mask & IN_CREATE)
            std::cout << "created: ";

        if (event->mask & IN_MODIFY)
            std::cout << "modified: ";

        if (event->mask & IN_DELETE)
            std::cout << "deleted: ";

        if (event->mask & IN_MOVED_FROM)
            std::cout << "moved from: ";

        if (event->mask & IN_MOVED_TO)
            std::cout << "moved to: ";

        if (event->len > 0)
            std::cout << event->name;

        std::cout << '\n';
    });
    watcher.start();
    return 0;
}

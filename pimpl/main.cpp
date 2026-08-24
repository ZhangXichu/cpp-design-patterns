#include "file_watcher.hpp"

#include <csignal>
#include <iostream>
#include <sys/inotify.h>
#include <pthread.h>
#include <thread>

int main() {
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);

    pthread_sigmask(SIG_BLOCK, &signals, nullptr);
    
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

    std::thread worker([&watcher] {
        watcher.start();
    });

    int received_signal = 0;
    sigwait(&signals, &received_signal);

    std::cout << "\nStopping file watcher...\n";
    watcher.stop();
    worker.join();
    
    return 0;
}

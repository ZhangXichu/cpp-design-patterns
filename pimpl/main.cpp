#include "file_watcher.hpp"

#include <csignal>
#include <iostream>
#include <pthread.h>
#include <sys/inotify.h>
#include <thread>

int main() {
    // Block SIGINT and SIGTERM, wait for either with sigwait(), call stop()
    // from normal thread context, then join the worker thread.
    sigset_t signals;
    sigemptyset(&signals);
    sigaddset(&signals, SIGINT);
    sigaddset(&signals, SIGTERM);

    pthread_sigmask(SIG_BLOCK, &signals, nullptr);

    FileWatcher watcher;
    watcher.on_change([](struct inotify_event *event) {
        const char *action = nullptr;

        if (event->mask & IN_CREATE)
            action = "created";
        else if (event->mask & IN_CLOSE_WRITE)
            action = "modified";
        else if (event->mask & (IN_DELETE | IN_MOVED_FROM))
            action = "deleted";
        else if (event->mask & IN_MOVED_TO)
            action = "moved to";

        if (!action)
            return;

        std::cout << action;

        if (event->len > 0)
            std::cout << ": " << event->name;

        std::cout << '\n';
    });

    std::thread worker([&watcher] { watcher.start(); });

    int received_signal = 0;
    sigwait(&signals, &received_signal);

    std::cout << "\nStopping file watcher...\n";
    watcher.stop();
    worker.join();

    return 0;
}

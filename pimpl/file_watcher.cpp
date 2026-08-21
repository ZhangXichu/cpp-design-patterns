#include "file_watcher.hpp"

#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>

namespace {
constexpr std::size_t buffer_length =
    10 * (sizeof(inotify_event) + 16);
}

class FileWatcher::impl {
public:
    void start();
    void stop();
    void on_change(Callback cb);

private:
    Callback _cb;
};

FileWatcher::FileWatcher()
    : _impl(std::make_unique<impl>()) {
}

FileWatcher::~FileWatcher() = default;

void FileWatcher::start() {
    _impl->start();
}

void FileWatcher::stop() {
    _impl->stop();
}

void FileWatcher::on_change(Callback cb) {
    _impl->on_change(cb);
}


void FileWatcher::impl::start() {
    int fd, wd;
    char buf[buffer_length] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    fd = inotify_init();                 /* Create inotify instance */

    wd = inotify_add_watch(fd, "./test_path", IN_ALL_EVENTS);

    for (;;) {                                  /* Read events forever */
        numRead = read(fd, buf, buffer_length);

        std::cout << "Read" << (long) numRead << "bytes from inotify" << std::endl;

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead; ) {

            event = (struct inotify_event *) p;

            if (_cb)
                _cb(event);

            p += sizeof(inotify_event) + event->len;
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);

}

void FileWatcher::impl::stop() {

}

void FileWatcher::impl::on_change(Callback cb) {
    _cb = cb;
}

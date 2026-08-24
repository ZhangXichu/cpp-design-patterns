#include "file_watcher.hpp"

#include <poll.h>
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
    std::atomic_bool _running{false};
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
    _running.store(true);

    int fd, wd;
    char buf[buffer_length] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);  
    pollfd watched_fd{fd, POLLIN, 0};   

    wd = inotify_add_watch(fd, "./test_path", IN_ALL_EVENTS);

    while (_running.load()) {                             
        const int result = poll(&watched_fd, 1, 250);/* Create inotify instance */

        if (result < 0) {
            if (errno == EINTR)
                continue;
            break;
        }
        
        if (result == 0)
            continue; // Re-check _running every 250 ms.

        numRead = read(fd, buf, buffer_length);

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
    _running.store(false);
}

void FileWatcher::impl::on_change(Callback cb) {
    _cb = cb;
}

#include "file_watcher.hpp"

#include <cerrno>
#include <poll.h>
#include <sys/inotify.h>
#include <unistd.h>

namespace {
static constexpr int invalid = -1;
constexpr std::size_t buffer_length = 10 * (sizeof(inotify_event) + 16);
} // namespace

/**
 * @brief Private implementation of FileWatcher.
 *
 * Creates a nonblocking inotify descriptor, watches `test_path`, and uses
 * poll() to wait for readable events. The finite polling timeout lets the
 * loop periodically check the atomic running flag.
 */
class FileWatcher::impl {
  public:
    void start();
    void stop();
    void on_change(Callback cb);

  private:
    Callback _cb;
    int _fd{invalid}, _wd{invalid};
    std::atomic_bool _running{false};
};

FileWatcher::FileWatcher() : _impl(std::make_unique<impl>()) {}

FileWatcher::~FileWatcher() = default;

void FileWatcher::start() { _impl->start(); }

void FileWatcher::stop() { _impl->stop(); }

void FileWatcher::on_change(Callback cb) { _impl->on_change(cb); }

void FileWatcher::impl::start() {
    _running.store(true);

    char buf[buffer_length] __attribute__((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    _fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    pollfd watched_fd{_fd, POLLIN, 0};

    constexpr uint32_t events =
        IN_CREATE | IN_CLOSE_WRITE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO;

    _wd = inotify_add_watch(_fd, "./test_path", events);

    while (_running.load()) {
        const int result =
            poll(&watched_fd, 1, 250); /* Create inotify instance */

        if (result < 0) {
            if (errno == EINTR)
                continue;
            break;
        }

        if (result == 0)
            continue; // Re-check _running every 250 ms.

        numRead = read(_fd, buf, buffer_length);

        if (numRead < 0) {
            if (errno == EINTR || errno == EAGAIN)
                continue;
            break;
        }

        if (numRead == 0)
            continue;

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead;) {

            event = (struct inotify_event *)p;

            if (_cb)
                _cb(event);

            p += sizeof(inotify_event) + event->len;
        }
    }

    if (_wd >= 0) {
        inotify_rm_watch(_fd, _wd);
        _wd = invalid;
    }

    if (_fd >= 0) {
        close(_fd);
        _fd = invalid;
    }
}

void FileWatcher::impl::stop() { _running.store(false); }

void FileWatcher::impl::on_change(Callback cb) { _cb = cb; }

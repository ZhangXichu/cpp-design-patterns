#include <atomic>
#include <functional>
#include <memory>

struct inotify_event;

class FileWatcher {

using Callback = std::function<void(inotify_event*)>;

public:
    FileWatcher();
    ~FileWatcher();

    void start();
    void stop();
    void on_change(Callback cb);

private:
    struct impl;
    std::unique_ptr<impl> _impl;
};



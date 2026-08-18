#include <functional>
#include <memory>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + 16))
class FileWatcher {

using Callback = std::function<void(const std::string& path)>;

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



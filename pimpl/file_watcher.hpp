#include <functional>
#include <memory>

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



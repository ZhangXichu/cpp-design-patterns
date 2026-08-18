#include "file_watcher.hpp"

#include <sys/inotify.h>

class FileWatcher::impl {
    
    public:
        void start();
        void stop();
        void on_change(Callback cb);
    
};


void FileWatcher::impl::start() {
    
}

void FileWatcher::impl::stop() {
    
}

void FileWatcher::impl::on_change(Callback cb) {
    
}
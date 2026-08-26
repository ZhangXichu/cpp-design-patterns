#include <atomic>
#include <functional>
#include <memory>

static constexpr int invalid = -1;

struct inotify_event;

/**
 * @brief File watcher whose Linux-specific state is hidden behind a Pimpl
 * pointer.
 *
 * The public interface offers three operations: install an event callback,
 * start watching, and stop watching. A caller can use those without knowing
 * which descriptors, flags or buffers the watcher stores internally.
 */
class FileWatcher {

    using Callback = std::function<void(inotify_event *)>;

  public:
    FileWatcher();
    ~FileWatcher();

    /// Begin watching until stop() is called.
    void start();

    /// Request the watching loop to exit and release its descriptors.
    void stop();

    /// Install the callback invoked for each inotify event.
    void on_change(Callback cb);

  private:
    struct impl;
    std::unique_ptr<impl> _impl;
};

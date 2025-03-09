#pragma once

#include <gio/gio.h>

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace Util::Path {

enum PathWatcherOptions {
    WATCH_NONE   = 0b00,  // only watch the base path
    WATCH_FILE   = 0b01,  // watch for file changes in the path
    WATCH_FOLDER = 0b10   // watch for folder changes in the path
};

enum PathEvents {
    PATH_WRITTEN = 0b0001,
    PATH_MOVED   = 0b0010,
    PATH_DELETED = 0b0100,
    PATH_CREATED = 0b1000
};

typedef std::function<void(std::string path, PathEvents event)>
    WatcherCallbackFunction;

struct PathWatcherCallback {
    uint64_t watchEvents;
    WatcherCallbackFunction func;
};

class PathWatcher {
private:
    uint64_t _nextPathID = 0;
    std::unordered_map<uint64_t, PathWatcherCallback> _callbacks;

    std::string _basePath;

    uint64_t _options;
    bool _recursive;

    std::vector<GFileMonitor*> _monitors;

    void startMonitor(std::string path);
    void stopMonitor();

public:
    PathWatcher(std::string basePath, uint64_t options = WATCH_NONE,
                bool recursive = false);
    ~PathWatcher();

    uint64_t startWatching(uint64_t events, WatcherCallbackFunction func);

    void reloadMonitors();

    void stopWatching();
    void stopWatching(uint64_t callback);

    std::unordered_map<uint64_t, PathWatcherCallback> getCallbacks() const;
};

};  // namespace Util::Path
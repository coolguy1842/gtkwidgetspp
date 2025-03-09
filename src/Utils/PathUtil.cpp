#include <Utils/PathUtil.hpp>

using namespace Util::Path;

// static void changed(GFileMonitor* self, GFile* file, GFile* other_file, GFileMonitorEvent event_type, gpointer user_data) {
//     PathWatcher* watcher = (PathWatcher*)user_data;

//     PathEvents event;
//     switch(event_type) {
//         case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
//             event = PathEvents::PATH_WRITTEN;
//             break;
//         case G_FILE_MONITOR_EVENT_CREATED:
//             event = PathEvents::PATH_CREATED;
//             watcher->reloadMonitors();
//             break;
//         case G_FILE_MONITOR_EVENT_DELETED:
//             event = PathEvents::PATH_DELETED;
//             watcher->reloadMonitors();
//             break;
//         case G_FILE_MONITOR_EVENT_MOVED:
//         case G_FILE_MONITOR_EVENT_RENAMED:
//             event = PathEvents::PATH_MOVED;
//             break;
//         default:
//             return;
//     }

//     for(auto& pair : watcher->getCallbacks()) {
//         if(pair.second.watchEvents & event) {
//             pair.second.func(g_file_get_path(file), event);
//         }
//     }
// }

void Util::Path::PathWatcher::startMonitor(std::string path) {
    GFile* file     = g_file_new_for_path(path.c_str());
    GFileInfo* info = g_file_query_info(file, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, NULL);

    GFileType type        = g_file_info_get_file_type(info);
    GFileMonitor* monitor = g_file_monitor(file, GFileMonitorFlags::G_FILE_MONITOR_WATCH_MOVES, NULL, NULL);

    // if(_basePath == path) {
    //     g_signal_connect(instance, detailed_signal, c_handler, data)
    // }
    // else {

    // }

    _monitors.push_back(monitor);

    if(_recursive && type == G_FILE_TYPE_DIRECTORY && _options != WATCH_NONE) {
        GFileEnumerator* enumerator = g_file_enumerate_children(file, "standard::type", G_FILE_QUERY_INFO_NONE, NULL, NULL);
        while(true) {
            GFileInfo* nextFileInfo;
            GFile* nextFile;
            if(!g_file_enumerator_iterate(enumerator, &nextFileInfo, &nextFile, NULL, NULL)) {
                break;
            }

            if(nextFile == NULL) {
                break;
            }

            GFileType nextFileType = g_file_info_get_file_type(nextFileInfo);
            if(nextFileType == G_FILE_TYPE_DIRECTORY) {
                startMonitor(g_file_get_path(nextFile));
            }
        }

        g_file_enumerator_close(enumerator, NULL, NULL);
    }
}

void Util::Path::PathWatcher::stopMonitor() {
    for(GFileMonitor* monitor : _monitors) {
        g_file_monitor_cancel(monitor);
    }

    _monitors.clear();
}

Util::Path::PathWatcher::PathWatcher(std::string basePath, uint64_t options, bool recursive) : _basePath(basePath), _options(options), _recursive(recursive) {}
Util::Path::PathWatcher::~PathWatcher() {
    stopWatching();
}

uint64_t Util::Path::PathWatcher::startWatching(uint64_t events, WatcherCallbackFunction func) {
    uint64_t id    = _nextPathID++;
    _callbacks[id] = {
        .watchEvents = events,
        .func        = func
    };

    if(_callbacks.size() == 1) {
        startMonitor(_basePath);
    }

    return id;
}

void Util::Path::PathWatcher::stopWatching(uint64_t callback) {
    auto it = _callbacks.find(callback);
    if(it != _callbacks.end()) {
        _callbacks.erase(it);
    }

    if(_callbacks.size() <= 0) {
        stopMonitor();
    }
}

void Util::Path::PathWatcher::stopWatching() {
    _callbacks.clear();

    stopMonitor();
}

void Util::Path::PathWatcher::reloadMonitors() {
    stopMonitor();

    startMonitor(_basePath);
}

std::unordered_map<uint64_t, PathWatcherCallback> Util::Path::PathWatcher::getCallbacks() const { return this->_callbacks; }
#pragma once

#include <giomm/datainputstream.h>
#include <giomm/socket.h>
#include <giomm/socketclient.h>
#include <glibmm/property.h>
#include <gtkmm-4.0/gdkmm.h>

#include <Utils/GLibUtil.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <vector>

namespace Services::Hypr {

template <typename T>
struct Bounds {
    T x;
    T y;
};

struct Monitor : public Glib::Object {
    struct Reserved {
        uint64_t left;
        uint64_t top;
        uint64_t right;
        uint64_t bottom;
    };

    MAKE_PROPERTY(uint64_t, id);

    MAKE_PROPERTY(std::string, name);
    MAKE_PROPERTY(std::string, description);
    MAKE_PROPERTY(std::string, make);
    MAKE_PROPERTY(std::string, model);
    MAKE_PROPERTY(std::string, serial);

    MAKE_PROPERTY(uint64_t, width);
    MAKE_PROPERTY(uint64_t, height);

    MAKE_PROPERTY(float, refreshRate);

    MAKE_PROPERTY(int64_t, x);
    MAKE_PROPERTY(int64_t, y);

    MAKE_PROPERTY(int64_t, activeWorkspace);
    MAKE_PROPERTY(int64_t, specialWorkspace);

    MAKE_PROPERTY(Reserved, reserved);

    MAKE_PROPERTY(float, scale);
    MAKE_PROPERTY(uint64_t, transform);

    MAKE_PROPERTY(bool, focused);
    MAKE_PROPERTY(bool, dpmsStatus);
    MAKE_PROPERTY(bool, vrr);
    MAKE_PROPERTY(bool, activelyTearing);

    Monitor();

public:
    ~Monitor();

    static Monitor* createFromJSON(nlohmann::json json);
    void updateFromJSON(nlohmann::json json);

    Gdk::Monitor* getGDKMonitor();
};

class Workspace : public Glib::Object {
    MAKE_PROPERTY(int64_t, id);
    MAKE_PROPERTY(uint64_t, monitorID);
    MAKE_PROPERTY(uint64_t, windows);

    MAKE_PROPERTY(std::string, name);
    MAKE_PROPERTY(std::string, monitor);

    MAKE_PROPERTY(bool, hasfullscreen);

    MAKE_PROPERTY(std::string, lastwindow);
    MAKE_PROPERTY(std::string, lastwindowtitle);

    Workspace();

public:
    static Workspace* createFromJSON(nlohmann::json json);
    void updateFromJSON(nlohmann::json json);

    ~Workspace();
};

class Client : public Glib::Object {
    MAKE_PROPERTY(std::string, address);

    MAKE_PROPERTY(Bounds<uint64_t>, at);
    MAKE_PROPERTY(Bounds<uint64_t>, size);

    MAKE_PROPERTY(uint64_t, monitor);
    MAKE_PROPERTY(int64_t, workspace);

    MAKE_PROPERTY(uint64_t, pid);
    MAKE_PROPERTY(uint64_t, focusHistoryID);

    MAKE_PROPERTY(std::string, class);
    MAKE_PROPERTY(std::string, title);
    MAKE_PROPERTY(std::string, initialClass);
    MAKE_PROPERTY(std::string, initialTitle);

    MAKE_PROPERTY(bool, fullscreen);
    MAKE_PROPERTY(bool, xwayland);
    MAKE_PROPERTY(bool, floating);
    MAKE_PROPERTY(bool, mapped);
    MAKE_PROPERTY(bool, hidden);
    MAKE_PROPERTY(bool, pinned);

    MAKE_PROPERTY(std::vector<std::string>, grouped);
    MAKE_PROPERTY(std::string, swallowing);

    Client();

public:
    static Client* createFromJSON(nlohmann::json json);
    void updateFromJSON(nlohmann::json json);

    ~Client();
};

class Actives : public Glib::Object {
    MAKE_PROPERTY(Monitor*, active_monitor);
    MAKE_PROPERTY(Workspace*, active_workspace);
    MAKE_PROPERTY(Client*, active_client);

public:
    Actives(Monitor* activeMonitor = nullptr, Workspace* activeWorkspace = nullptr, Client* activeClient = nullptr);
    ~Actives();
};

// https://github.com/Aylur/ags/blob/v1/src/service/hyprland.ts used for reference
class Service : public Glib::Object {
    MAKE_SIGNAL(monitor_added, Monitor*);
    // monitor id
    MAKE_SIGNAL(monitor_removed, std::string);

    MAKE_SIGNAL(workspace_added, Workspace*);
    // workspace id
    MAKE_SIGNAL(workspace_removed, std::string);

    MAKE_SIGNAL(client_added, Client*);
    // client id
    MAKE_SIGNAL(client_removed, std::string);

    MAKE_SIGNAL(fullscreen, bool);
    MAKE_SIGNAL(urgent_client, Client*);

    // keyboardname, layoutname
    MAKE_SIGNAL(keyboard_layout, std::string, std::string);
    // submap name
    MAKE_SIGNAL(submap, std::string);

    MAKE_PROPERTY(std::vector<Glib::RefPtr<Monitor>>, monitors);
    MAKE_PROPERTY(std::vector<Glib::RefPtr<Workspace>>, workspaces);
    MAKE_PROPERTY(std::vector<Glib::RefPtr<Client>>, clients);

private:
    Glib::RefPtr<Gio::SocketClient> _socketClient;
    Glib::RefPtr<Gio::SocketAddress> _dispatchSocketAddress, _eventSocketAddress;

    std::string _dispatchSocketPath, _eventSocketPath;
    Actives* _actives;

    void loadSocketPaths();
    enum SocketType {
        DISPATCH = 0,
        EVENT
    };

    Glib::RefPtr<Gio::SocketConnection> connect(SocketType type);
    std::pair<Glib::RefPtr<Gio::SocketConnection>, Glib::RefPtr<Gio::DataInputStream>> stream(SocketType type, std::string msg = "");

    void syncMonitors();
    void syncWorkspaces();
    void syncClients();

    void syncActives();

    void onEvent(std::string event);
    void watchStream(const Glib::RefPtr<Gio::DataInputStream>& stream);

    Service();
    ~Service();

public:
    std::string message(std::string msg);

    static Service* getInstance();
    static void closeInstance();

    Actives* getActives();

    Client* getClient(std::string address);

    Workspace* getWorkspace(int64_t id);
    Workspace* getWorkspace(std::string name);
    std::vector<Workspace*> getWorkspaces(uint64_t monitorID);

    Monitor* getMonitor(std::string name);
    Monitor* getMonitor(uint64_t id);
};

};  // namespace Services::Hypr
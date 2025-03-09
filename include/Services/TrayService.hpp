#pragma once

#include <fmt/format.h>
#include <gtkmm-4.0/gtkmm.h>
#include <status-notifier-watcher-interface_stub.h>
#include <status-notifier-item-interface_proxy.h>

#include <Utils/GLibUtil.hpp>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class TrayItem : public org::kde::StatusNotifierItemProxy, virtual Glib::Object {
protected:
    Glib::RefPtr<Gio::DBus::Connection> _connection;

    TrayItem(Glib::RefPtr<Gio::DBus::Proxy> proxy);

public:
    using org::kde::StatusNotifierItemProxy::unreference;

    virtual ~TrayItem();

    std::string getID();
    std::string getTitle();
    std::string getIconName();
    std::string getIconThemePath();

    void resetProperties();

    static void createForBus(
        Gio::DBus::BusType busType,
        Gio::DBus::ProxyFlags proxyFlags,
        const std::string& name,
        const std::string& objectPath,
        const Gio::SlotAsyncReady& slot,
        const Glib::RefPtr<Gio::Cancellable>& cancellable
    ) {
        Gio::DBus::Proxy::create_for_bus(busType, name, objectPath, "org.kde.StatusNotifierItem", slot, cancellable, Glib::RefPtr<Gio::DBus::InterfaceInfo>(), proxyFlags);
    }

    static Glib::RefPtr<TrayItem> createForBusFinish(const Glib::RefPtr<Gio::AsyncResult>& result) {
        Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_finish(result);
        TrayItem* p                          = new TrayItem(proxy);

        return Glib::RefPtr<TrayItem>(p);
    }

    static Glib::RefPtr<TrayItem> createForBus_sync(
        Gio::DBus::BusType busType,
        Gio::DBus::ProxyFlags proxyFlags,
        const std::string& name,
        const std::string& objectPath,
        const Glib::RefPtr<Gio::Cancellable>& cancellable
    ) {
        Glib::RefPtr<Gio::DBus::Proxy> proxy = Gio::DBus::Proxy::create_for_bus_sync(busType, name, objectPath, "org.kde.StatusNotifierItem", cancellable, Glib::RefPtr<Gio::DBus::InterfaceInfo>(), proxyFlags);
        TrayItem* p                          = new TrayItem(proxy);

        return Glib::RefPtr<TrayItem>(p);
    }
};

#define DEBUG_DBUS
class StatusNotifierWatcher : public org::kde::StatusNotifierWatcherStub {
    MAKE_SIGNAL(item_registered, std::string);
    MAKE_SIGNAL(item_unregistered, std::string);

private:
    // busname, busname + objectpath
    std::unordered_map<std::string, std::string> _items;
    Glib::RefPtr<Gio::DBus::Connection> _connection;

public:
    ~StatusNotifierWatcher();
    StatusNotifierWatcher();

    void RegisterStatusNotifierItem(const Glib::ustring& service, MethodInvocation& invocation);
    std::vector<Glib::ustring> RegisteredStatusNotifierItems_get();

    void RegisterStatusNotifierHost(const Glib::ustring& service, MethodInvocation& invocation);
    bool IsStatusNotifierHostRegistered_get();
    gint32 ProtocolVersion_get();
};

namespace Services::Tray {

class Service : public Glib::Object {
private:
    class Watcher : public Glib::Object {
        MAKE_PROPERTY(std::vector<Glib::RefPtr<TrayItem>>, items);

        MAKE_SIGNAL(item_registered, Glib::RefPtr<TrayItem>);
        MAKE_SIGNAL(item_unregistered, Glib::RefPtr<TrayItem>);

        StatusNotifierWatcher _watcher;
        std::map<std::string, TrayItem*> _pathLookups;

    private:
        void on_item_registered(std::string path);
        void on_item_unregistered(std::string path);

    public:
        ~Watcher();
        Watcher();
    };

    std::map<TrayItem*, std::string> pathLookups;
    Watcher _watcher;

    Service();

public:
    ~Service();

    static Service* getInstance();
    static void closeInstance();

    Watcher::type_signal_item_registered signal_item_registered() { return _watcher.signal_item_registered(); }
    Watcher::type_signal_item_registered signal_item_unregistered() { return _watcher.signal_item_unregistered(); }

    Glib::PropertyProxy<std::vector<Glib::RefPtr<TrayItem>>> property_items() { return _watcher.property_items(); }
    std::vector<Glib::RefPtr<TrayItem>> get_items() { return _watcher.get_items(); }
};

};  // namespace Services::Tray
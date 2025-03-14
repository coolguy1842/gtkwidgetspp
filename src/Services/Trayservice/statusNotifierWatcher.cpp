#include <Services/TrayService.hpp>
#include <algorithm>
#include <iterator>

#include "giomm/asyncresult.h"
#include "giomm/dbusconnection.h"
#include "glibmm/refptr.h"

Services::Tray::StatusNotifierWatcher::~StatusNotifierWatcher() {}
Services::Tray::StatusNotifierWatcher::StatusNotifierWatcher() {
    Gio::DBus::Connection::get(Gio::DBus::BusType::SESSION, [&](const Glib::RefPtr<Gio::AsyncResult>& res) {
        _connection = Gio::DBus::Connection::get_finish(res);

        _connection->signal_subscribe(
            [&](
                const Glib::RefPtr<Gio::DBus::Connection>& connection,
                const Glib::ustring& sender_name,
                const Glib::ustring& object_path,
                const Glib::ustring& interface_name,
                const Glib::ustring& signal_name,
                const Glib::VariantContainerBase& parameters
            ) {
                std::string name      = parameters.get_child(0).get_dynamic<std::string>();
                std::string old_owner = parameters.get_child(1).get_dynamic<std::string>();
                std::string new_owner = parameters.get_child(2).get_dynamic<std::string>();

                if(new_owner.empty() && _items.contains(old_owner)) {
                    std::string path = _items[old_owner];

                    _items.erase(old_owner);

                    StatusNotifierItemUnregistered_signal.emit(path);
                    _signal_item_unregistered.emit(path);
                }
            },
            "",
            "org.freedesktop.DBus",
            "NameOwnerChanged",
            "",
            "",
            Gio::DBus::SignalFlags::NONE
        );
    });
}

void Services::Tray::StatusNotifierWatcher::RegisterStatusNotifierItem(const Glib::ustring& service, MethodInvocation& invocation) {
    std::string busName = invocation.getMessage()->get_sender(), objectPath = service;
    if(!objectPath.starts_with("/")) {
        busName    = service;
        objectPath = "/StatusNotifierItem";
    }

    std::string path = fmt::format("{}{}", busName, objectPath);
    _items[busName]  = path;

    StatusNotifierItemRegistered_signal.emit(path);
    _signal_item_registered.emit(path);

    invocation.ret();
}

std::vector<Glib::ustring> Services::Tray::StatusNotifierWatcher::RegisteredStatusNotifierItems_get() {
    std::vector<Glib::ustring> items(_items.size());
    std::transform(_items.begin(), _items.end(), std::back_inserter(items), [](const auto& it) { return it.second; });

    return items;
}

void Services::Tray::StatusNotifierWatcher::RegisterStatusNotifierHost(const Glib::ustring& service, MethodInvocation& invocation) { invocation.ret(); }
bool Services::Tray::StatusNotifierWatcher::IsStatusNotifierHostRegistered_get() { return true; }
gint32 Services::Tray::StatusNotifierWatcher::ProtocolVersion_get() { return 0; }
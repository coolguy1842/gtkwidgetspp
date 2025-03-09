#include <Services/TrayService.hpp>

StatusNotifierWatcher::~StatusNotifierWatcher() {}
StatusNotifierWatcher::StatusNotifierWatcher() {
    _connection = Gio::DBus::Connection::get_sync(Gio::DBus::BusType::SESSION);
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
}

void StatusNotifierWatcher::RegisterStatusNotifierItem(const Glib::ustring& service, MethodInvocation& invocation) {
    std::string busName    = invocation.getMessage()->get_sender();
    std::string objectPath = service;

    if(!objectPath.starts_with("/")) {
        busName    = service;
        objectPath = "/StatusNotifierItem";
    }

    std::string path = fmt::format("{}{}", busName, objectPath);

    _items[busName] = path;
    StatusNotifierItemRegistered_signal.emit(path);
    _signal_item_registered.emit(path);

    invocation.ret();
}

std::vector<Glib::ustring> StatusNotifierWatcher::RegisteredStatusNotifierItems_get() {
    std::vector<Glib::ustring> items;
    for(const auto& pair : _items) {
        items.push_back(pair.second);
    }

    return items;
}

void StatusNotifierWatcher::RegisterStatusNotifierHost(const Glib::ustring& service, MethodInvocation& invocation) { invocation.ret(); }
bool StatusNotifierWatcher::IsStatusNotifierHostRegistered_get() { return true; }
gint32 StatusNotifierWatcher::ProtocolVersion_get() { return 0; }
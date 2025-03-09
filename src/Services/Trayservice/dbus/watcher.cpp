#include <Services/TrayService.hpp>
#include <algorithm>

void Services::Tray::Service::Watcher::on_item_registered(std::string path) {
    printf("%s registered\n", path.c_str());

    auto it                = path.find_first_of('/');
    std::string busName    = path.substr(0, it);
    std::string objectPath = path.substr(it);

    printf("busName: %s, objectPath: %s\n", busName.c_str(), objectPath.c_str());

    auto proxy = TrayItem::createForBus_sync(
        Gio::DBus::BusType::SESSION,
        Gio::DBus::ProxyFlags::NONE,
        busName,
        objectPath,
        nullptr
    );

    _pathLookups[path] = proxy.get();

    auto items = get_items();
    items.push_back(proxy);

    _property_items.set_value(items);
    _signal_item_registered.emit(proxy);
}

void Services::Tray::Service::Watcher::on_item_unregistered(std::string path) {
    printf("%s unregistered\n", path.c_str());

    TrayItem* proxy = _pathLookups[path];
    if(proxy == nullptr) {
        return;
    }

    auto items = get_items();
    auto it    = std::find_if(items.begin(), items.end(), [&](const Glib::RefPtr<TrayItem>& item) {
        return item.get() == proxy;
    });

    _pathLookups.erase(path);
    if(it == items.end()) {
        return;
    }

    items.erase(it);

    _property_items.set_value(items);
    _signal_item_unregistered.emit(Glib::make_refptr_for_instance(proxy));
}

Services::Tray::Service::Watcher::~Watcher() {}
Services::Tray::Service::Watcher::Watcher()
    : Glib::ObjectBase(typeid(Services::Tray::Service::Watcher))
    , _property_items(*this, "items") {
    uint32_t owner_id = Gio::DBus::own_name(
        Gio::DBus::BusType::SESSION,
        "org.kde.StatusNotifierWatcher",
        [&](const Glib::RefPtr<Gio::DBus::Connection>& con, Glib::ustring name) {},
        [&](const Glib::RefPtr<Gio::DBus::Connection>& con, Glib::ustring name) {
            printf("name acquired on %s\n", name.c_str());

            if(_watcher.register_object(con, "/StatusNotifierWatcher") == 0) {
                printf("ERROR: Failed to register object on /StatusNotifierWatcher\n");

                Gio::DBus::unown_name(owner_id);
            }
            else {
                printf("successfully registered object\n");
            }
        },
        [&](const Glib::RefPtr<Gio::DBus::Connection>& con, Glib::ustring name) {
            printf("ERROR: Lost name for %s\n", name.c_str());
        },
        Gio::DBus::BusNameOwnerFlags::REPLACE
    );

    _watcher.signal_item_registered().connect(sigc::mem_fun(*this, &Watcher::on_item_registered));
    _watcher.signal_item_unregistered().connect(sigc::mem_fun(*this, &Watcher::on_item_unregistered));
}
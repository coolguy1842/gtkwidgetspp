#include <glibmm/property.h>

#include <Services/HyprService.hpp>
#include <nlohmann/json_fwd.hpp>

#define INIT_PROPERTY(name) _property_##name(*this, #name)

Services::Hypr::Monitor::Monitor()
    : Glib::ObjectBase(typeid(Monitor))
    , INIT_PROPERTY(id)
    , INIT_PROPERTY(name)
    , INIT_PROPERTY(description)
    , INIT_PROPERTY(make)
    , INIT_PROPERTY(model)
    , INIT_PROPERTY(serial)
    , INIT_PROPERTY(width)
    , INIT_PROPERTY(height)
    , INIT_PROPERTY(refreshRate)
    , INIT_PROPERTY(x)
    , INIT_PROPERTY(y)
    , INIT_PROPERTY(activeWorkspace)
    , INIT_PROPERTY(specialWorkspace)
    , INIT_PROPERTY(reserved)
    , INIT_PROPERTY(scale)
    , INIT_PROPERTY(transform)
    , INIT_PROPERTY(focused)
    , INIT_PROPERTY(dpmsStatus)
    , INIT_PROPERTY(vrr)
    , INIT_PROPERTY(activelyTearing) {}

Services::Hypr::Monitor* Services::Hypr::Monitor::createFromJSON(nlohmann::json json) {
    Monitor* monitor = new Monitor();
    monitor->updateFromJSON(json);

    return monitor;
}

#define ARG__(name, val)                                      \
    fakeMonitor.property_##name().set_value(val);             \
    if(get_##name() != fakeMonitor.get_##name()) {            \
        _property_##name.set_value(fakeMonitor.get_##name()); \
    }

#define ARG(name) ARG__(name, json[#name])
#define ARG_V(name, value) ARG__(name, value)

void Services::Hypr::Monitor::updateFromJSON(nlohmann::json json) {
    Monitor fakeMonitor;

    ARG(id);

    ARG(name);
    ARG(description);
    ARG(make);
    ARG(model);
    ARG(serial);

    ARG(width);
    ARG(height);

    ARG(refreshRate);

    ARG(x);
    ARG(y);

    ARG_V(activeWorkspace, json["activeWorkspace"]["id"]);
    ARG_V(specialWorkspace, json["specialWorkspace"]["id"]);

    Reserved reserved = { json["reserved"][0], json["reserved"][1], json["reserved"][2], json["reserved"][3] };
    if(get_reserved().left != reserved.left || get_reserved().top != reserved.top || get_reserved().right != reserved.right || get_reserved().bottom != reserved.bottom) {
        property_reserved().set_value(reserved);
    }

    ARG(scale);
    ARG(transform);

    ARG(focused);
    ARG(dpmsStatus);
    ARG(vrr);
    ARG(activelyTearing);
}

Services::Hypr::Monitor::~Monitor() {
}

Services::Hypr::Monitor* Services::Hypr::Service::getMonitor(std::string name) {
    for(Glib::RefPtr<Monitor>& monitor : get_monitors()) {
        if(monitor->get_name() == name) {
            return monitor.get();
        }
    }

    return nullptr;
}

Services::Hypr::Monitor* Services::Hypr::Service::getMonitor(uint64_t id) {
    for(Glib::RefPtr<Monitor>& monitor : get_monitors()) {
        if(monitor->get_id() == id) {
            return monitor.get();
        }
    }

    return nullptr;
}

Gdk::Monitor* Services::Hypr::Monitor::getGDKMonitor() {
    auto monitors = Gdk::Display::get_default()->get_monitors();
    for(uint64_t i = 0; i < monitors->get_n_items(); i++) {
        Gdk::Monitor* monitor = monitors->get_typed_object<Gdk::Monitor>(i).get();
        if(std::string(monitor->get_connector()) == get_name()) {
            return monitor;
        }
    }

    return nullptr;
}

void Services::Hypr::Service::syncMonitors() {
    const nlohmann::json json = nlohmann::json::parse(message("j/monitors"));
    std::vector<Glib::RefPtr<Monitor>> monitors, monitorList = get_monitors();

    for(const nlohmann::json& monitorJSON : json) {
        auto it = std::find_if(monitorList.begin(), monitorList.end(), [&](const Glib::RefPtr<Monitor>& x) {
            return monitorJSON["id"] == x->get_id();
        });

        monitors.push_back(it != monitorList.end() ? *(it.base()) : Glib::make_refptr_for_instance(Monitor::createFromJSON(monitorJSON)));
    }

    if(!std::equal(std::begin(monitors), std::end(monitors), std::begin(monitorList), std::end(monitorList))) {
        _property_monitors.set_value(monitors);
    }
}

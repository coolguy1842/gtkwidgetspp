#include <Services/HyprService.hpp>

Services::Hypr::Actives::Actives(Monitor* activeMonitor, Workspace* activeWorkspace, Client* activeClient)
    : Glib::ObjectBase(typeid(Actives))
    , _property_active_monitor(*this, "active_monitor")
    , _property_active_workspace(*this, "active_workspace")
    , _property_active_client(*this, "active_client") {}

Services::Hypr::Actives::~Actives() {}

void Services::Hypr::Service::syncActives() {
    for(Glib::RefPtr<Client>& client : get_clients()) {
        if(client->get_focusHistoryID() == 0 && _actives->get_active_client() != client.get()) {
            _actives->property_active_client().set_value(client.get());

            break;
        }
    }

    for(Glib::RefPtr<Monitor>& monitor : get_monitors()) {
        if(!monitor->get_focused()) {
            continue;
        }

        if(_actives->get_active_monitor() != monitor.get()) {
            _actives->property_active_monitor().set_value(monitor.get());
        }

        if(
            monitor->get_activeWorkspace() == 0 ||
            (_actives->get_active_workspace() != nullptr && _actives->get_active_workspace()->get_id() == monitor->get_activeWorkspace())
        ) {
            break;
        }

        _actives->property_active_workspace().set_value(getWorkspace(monitor->get_activeWorkspace()));
    }
}

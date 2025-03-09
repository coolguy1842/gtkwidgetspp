#include <fmt/format.h>

#include <Services/HyprService.hpp>
#include <Utils/StringUtil.hpp>
#include <vector>

#define VERBOSE_EVENTS false
void Services::Hypr::Service::onEvent(std::string event) {
    std::vector<std::string> splitStr = Util::String::split(event, ">>");

    std::string key               = splitStr[0];
    std::vector<std::string> args = Util::String::split(splitStr[1], ",");

    switch(Util::String::hashFunc(key.c_str())) {
    case "workspace"_hash: {
        if(VERBOSE_EVENTS) printf("workspace called with args: %s\n", splitStr[1].c_str());

        Workspace* workspace = getWorkspace(args[0]);
        if(workspace == nullptr) {
            syncWorkspaces();

            workspace = getWorkspace(args[0]);
            if(workspace == nullptr) {
                break;
            }
        }

        _actives->get_active_monitor()->property_activeWorkspace().set_value(workspace->get_id());

        break;
    }
    case "focusedmon"_hash: {
        if(VERBOSE_EVENTS) printf("focusedmon called with args: %s\n", splitStr[1].c_str());

        Monitor* monitor = getMonitor(args[0]);
        _actives->get_active_monitor()->property_focused().set_value(false);
        monitor->property_focused().set_value(true);

        _actives->property_active_monitor().set_value(monitor);

        break;
    }
    case "monitoradded"_hash:
        if(VERBOSE_EVENTS) printf("monitor added called with args: %s\n", splitStr[1].c_str());

        syncMonitors();
        _signal_monitor_added.emit(getMonitor(args[0]));

        break;
    case "monitorremoved"_hash: {
        if(VERBOSE_EVENTS) printf("monitor removed called with args: %s\n", splitStr[1].c_str());

        std::vector<Glib::RefPtr<Monitor>> monitors;
        for(Glib::RefPtr<Monitor>& monitor : get_monitors()) {
            if(monitor->get_name() == args[0]) {
                continue;
            }

            monitors.push_back(monitor);
        }

        _property_monitors.set_value(monitors);
        _signal_monitor_removed.emit(args[0]);

        break;
    }
    case "createworkspace"_hash:
        if(VERBOSE_EVENTS) printf("create workspace called with args: %s\n", splitStr[1].c_str());

        syncWorkspaces();
        _signal_workspace_added.emit(getWorkspace(args[0]));

        break;
    case "destroyworkspace"_hash: {
        if(VERBOSE_EVENTS) printf("destroy workspace called with args: %s\n", splitStr[1].c_str());

        std::vector<Glib::RefPtr<Workspace>> workspaces;
        for(Glib::RefPtr<Workspace>& workspace : get_workspaces()) {
            if(workspace->get_name() == args[0]) {
                continue;
            }

            workspaces.push_back(workspace);
        }

        _property_workspaces.set_value(workspaces);
        _signal_workspace_removed.emit(args[0]);

        break;
    }
    case "openwindow"_hash:
        if(VERBOSE_EVENTS) printf("open window called with args: %s\n", splitStr[1].c_str());

        syncClients();
        _signal_client_added.emit(getClient(fmt::format("0x{}", args[0])));

        break;
    case "closewindow"_hash: {
        if(VERBOSE_EVENTS) printf("close window called with args: %s\n", splitStr[1].c_str());

        std::string address = fmt::format("0x{}", args[0]);
        std::vector<Glib::RefPtr<Client>> clients;
        for(Glib::RefPtr<Client>& client : get_clients()) {
            if(client->get_address() == address) {
                continue;
            }

            clients.push_back(client);
        }

        _property_clients.set_value(clients);
        _signal_client_removed.emit(fmt::format("0x{}", args[0]));

        break;
    }
    case "movewindow"_hash:
        if(VERBOSE_EVENTS) printf("move window called with args: %s\n", splitStr[1].c_str());

        getClient(fmt::format("0x{}", args[0]))->property_workspace().set_value(getWorkspace(args[1])->get_id());

        break;
    case "moveworkspace"_hash: {
        if(VERBOSE_EVENTS) printf("moveworkspace called with args: %s\n", splitStr[1].c_str());

        Monitor* monitor = getMonitor(args[1]);
        if(monitor == nullptr) {
            syncMonitors();

            monitor = getMonitor(args[1]);
            if(monitor == nullptr) {
                break;
            }
        }

        getWorkspace(args[0])->property_monitor().set_value(monitor->get_name());
        getWorkspace(args[0])->property_monitorID().set_value(monitor->get_id());

        break;
    }
    case "fullscreen"_hash: {
        if(VERBOSE_EVENTS) printf("fullscreen called with args: %s\n", splitStr[1].c_str());

        bool fullscreen = args[0][0] == '1';
        _actives->get_active_workspace()->property_hasfullscreen().set_value(fullscreen);
        _actives->get_active_client()->property_fullscreen().set_value(fullscreen);

        _signal_fullscreen.emit(args[0][0] == '1');
        break;
    }
    case "activewindow"_hash:
        if(VERBOSE_EVENTS) printf("activewindow called with args: %s\n", splitStr[1].c_str());

        _actives->get_active_workspace()->property_lastwindowtitle().set_value(args[1]);

        break;
    case "activewindowv2"_hash: {
        if(VERBOSE_EVENTS) printf("activewindowv2 called with args: %s\n", splitStr[1].c_str());

        std::string address = fmt::format("0x{}", args[0]);
        _actives->get_active_workspace()->property_lastwindow().set_value(address);
        _actives->property_active_client().set_value(getClient(address));

        break;
    }
    default: break;
    }
}
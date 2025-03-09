#include <glibmm/property.h>

#include <Services/HyprService.hpp>
#include <nlohmann/json_fwd.hpp>
#include <vector>

#define INIT_PROPERTY(name) _property_##name(*this, #name)

Services::Hypr::Workspace::Workspace()
    : Glib::ObjectBase(typeid(Workspace))
    , INIT_PROPERTY(id)
    , INIT_PROPERTY(monitorID)
    , INIT_PROPERTY(windows)
    , INIT_PROPERTY(name)
    , INIT_PROPERTY(monitor)
    , INIT_PROPERTY(hasfullscreen)
    , INIT_PROPERTY(lastwindow)
    , INIT_PROPERTY(lastwindowtitle) {}

Services::Hypr::Workspace* Services::Hypr::Workspace::createFromJSON(nlohmann::json json) {
    Workspace* workspace = new Workspace();
    workspace->updateFromJSON(json);

    return workspace;
}

#define ARG__(name, val)                                        \
    fakeWorkspace.property_##name().set_value(val);             \
    if(get_##name() != fakeWorkspace.get_##name()) {            \
        _property_##name.set_value(fakeWorkspace.get_##name()); \
    }

#define ARG(name) ARG__(name, json[#name])

void Services::Hypr::Workspace::updateFromJSON(nlohmann::json json) {
    Workspace fakeWorkspace;

    ARG(id);
    ARG(monitorID);
    ARG(windows);

    ARG(name);
    ARG(monitor);

    ARG(hasfullscreen);

    ARG(lastwindow);
    ARG(lastwindowtitle);
}

Services::Hypr::Workspace::~Workspace() {
}

Services::Hypr::Workspace* Services::Hypr::Service::getWorkspace(int64_t id) {
    for(Glib::RefPtr<Workspace>& workspace : get_workspaces()) {
        if(workspace->get_id() == id) {
            return workspace.get();
        }
    }

    return nullptr;
}

Services::Hypr::Workspace* Services::Hypr::Service::getWorkspace(std::string name) {
    for(Glib::RefPtr<Workspace>& workspace : get_workspaces()) {
        if(workspace->get_name() == name) {
            return workspace.get();
        }
    }

    return nullptr;
}

std::vector<Services::Hypr::Workspace*> Services::Hypr::Service::getWorkspaces(uint64_t monitorID) {
    std::vector<Workspace*> workspaces = {};
    for(Glib::RefPtr<Workspace>& workspace : get_workspaces()) {
        if(workspace->get_monitorID() == monitorID) {
            workspaces.push_back(workspace.get());
        }
    }

    return workspaces;
}

void Services::Hypr::Service::syncWorkspaces() {
    const nlohmann::json json = nlohmann::json::parse(message("j/workspaces"));
    std::vector<Glib::RefPtr<Workspace>> workspaces, workspaceList = get_workspaces();

    for(const nlohmann::json& workspaceJSON : json) {
        auto it = std::find_if(workspaceList.begin(), workspaceList.end(), [&](const Glib::RefPtr<Workspace>& x) {
            return workspaceJSON["id"] == x->get_id();
        });

        workspaces.push_back(it != workspaceList.end() ? *(it.base()) : Glib::make_refptr_for_instance(Workspace::createFromJSON(workspaceJSON)));
    }

    if(!std::equal(std::begin(workspaces), std::end(workspaces), std::begin(workspaceList), std::end(workspaceList))) {
        _property_workspaces.set_value(workspaces);
    }
}

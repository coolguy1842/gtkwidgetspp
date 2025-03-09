#include <fmt/format.h>
#include <giomm-2.68/giomm.h>
#include <unistd.h>

#include <Services/HyprService.hpp>
#include <Utils/StringUtil.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <utility>

#include "glibmm/refptr.h"

Services::Hypr::Service::~Service() {}
Services::Hypr::Service::Service()
    : Glib::ObjectBase(typeid(Services::Hypr::Service))
    , _property_monitors(*this, "monitors")
    , _property_workspaces(*this, "workspaces")
    , _property_clients(*this, "clients") {
    printf("creating socket\n");

    loadSocketPaths();

    _actives = new Actives();

    _socketClient          = Gio::SocketClient::create();
    _dispatchSocketAddress = Gio::UnixSocketAddress::create(_dispatchSocketPath.c_str());
    _eventSocketAddress    = Gio::UnixSocketAddress::create(_eventSocketPath.c_str());

    auto pair = stream(EVENT);

    syncClients();
    syncWorkspaces();
    syncMonitors();
    syncActives();

    this->watchStream(pair.second);
    printf("created socket\n");
}

#pragma region __GETTERS__

Services::Hypr::Actives* Services::Hypr::Service::getActives() { return _actives; }

#pragma endregion
#pragma region __SINGLETON__

static Services::Hypr::Service* instance = nullptr;
Services::Hypr::Service* Services::Hypr::Service::getInstance() {
    if(instance != nullptr) {
        return instance;
    }

    instance = new Services::Hypr::Service();
    return instance;
}

void Services::Hypr::Service::closeInstance() {
    if(instance == nullptr) {
        return;
    }

    delete instance;
    instance = nullptr;
}

#pragma endregion
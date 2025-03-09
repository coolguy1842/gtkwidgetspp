#include <glibmm/property.h>

#include <Services/HyprService.hpp>
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <nlohmann/json_fwd.hpp>

#include "glibmm/refptr.h"

#define INIT_PROPERTY(name) _property_##name(*this, #name)

Services::Hypr::Client::Client()
    : Glib::ObjectBase(typeid(Client))
    , INIT_PROPERTY(address)
    , INIT_PROPERTY(at)
    , INIT_PROPERTY(size)
    , INIT_PROPERTY(monitor)
    , INIT_PROPERTY(workspace)
    , INIT_PROPERTY(pid)
    , INIT_PROPERTY(focusHistoryID)
    , INIT_PROPERTY(class)
    , INIT_PROPERTY(title)
    , INIT_PROPERTY(initialClass)
    , INIT_PROPERTY(initialTitle)
    , INIT_PROPERTY(fullscreen)
    , INIT_PROPERTY(xwayland)
    , INIT_PROPERTY(floating)
    , INIT_PROPERTY(mapped)
    , INIT_PROPERTY(hidden)
    , INIT_PROPERTY(pinned)
    , INIT_PROPERTY(grouped)
    , INIT_PROPERTY(swallowing) {}

Services::Hypr::Client* Services::Hypr::Client::createFromJSON(nlohmann::json json) {
    Client* client = new Client();
    client->updateFromJSON(json);

    return client;
}

#define ARG__(name, val)                                     \
    fakeClient.property_##name().set_value(val);             \
    if(get_##name() != fakeClient.get_##name()) {            \
        _property_##name.set_value(fakeClient.get_##name()); \
    }

#define ARG(name) ARG__(name, json[#name])
#define ARG_V(name, value) ARG__(name, value)

void Services::Hypr::Client::updateFromJSON(nlohmann::json json) {
    Client fakeClient;

    ARG(address);

    Bounds<uint64_t> at = { json["at"][0], json["at"][1] };
    if(get_at().x != at.x || get_at().y != at.y) {
        _property_at.set_value(at);
    }

    Bounds<uint64_t> size = { json["size"][0], json["size"][1] };
    if(this->get_size().x != size.x || this->get_size().y != size.y) {
        _property_size.set_value(size);
    }

    ARG(monitor);
    ARG_V(workspace, json["workspace"]["id"]);
    ARG(pid);

    ARG(focusHistoryID);

    ARG(class);
    ARG(title);
    ARG(initialClass);
    ARG(initialTitle);

    ARG_V(fullscreen, json["fullscreen"].get<int>() != 0);

    ARG(xwayland);
    ARG(floating);
    ARG(mapped);
    ARG(hidden);
    ARG(pinned);

    fakeClient.property_grouped().set_value(json["grouped"]);
    std::vector<std::string> a = get_grouped(), b = fakeClient.get_grouped();
    if(!std::equal(std::begin(a), std::end(a), std::begin(b), std::end(b))) {
        _property_grouped.set_value(fakeClient.get_grouped());
    }

    ARG(swallowing);
}

Services::Hypr::Client::~Client() {
}

Services::Hypr::Client* Services::Hypr::Service::getClient(std::string address) {
    for(Glib::RefPtr<Client>& client : get_clients()) {
        if(client->get_address() == address || client->get_address().substr(2) == address) {
            return client.get();
        }
    }

    return nullptr;
}

void Services::Hypr::Service::syncClients() {
    const nlohmann::json json = nlohmann::json::parse(message("j/clients"));
    std::vector<Glib::RefPtr<Client>> clients, clientList = get_clients();

    for(const nlohmann::json& clientJSON : json) {
        auto it = std::find_if(clientList.begin(), clientList.end(), [&](const Glib::RefPtr<Client>& x) {
            return clientJSON["address"] == x->get_address();
        });

        clients.push_back(it != clientList.end() ? *(it.base()) : Glib::make_refptr_for_instance(Client::createFromJSON(clientJSON)));
    }

    if(!std::equal(std::begin(clients), std::end(clients), std::begin(clientList), std::end(clientList))) {
        _property_clients.set_value(clients);
    }
}

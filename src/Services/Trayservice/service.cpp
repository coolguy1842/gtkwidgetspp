#include <Services/TrayService.hpp>

Services::Tray::Service::~Service() {}
Services::Tray::Service::Service()
    : Glib::ObjectBase(typeid(Services::Tray::Service)) {
}

#pragma region __SINGLETON__

static Services::Tray::Service* instance = nullptr;
Services::Tray::Service* Services::Tray::Service::getInstance() {
    if(instance != nullptr) {
        return instance;
    }

    instance = new Services::Tray::Service();
    return instance;
}

void Services::Tray::Service::closeInstance() {
    if(instance == nullptr) {
        return;
    }

    delete instance;
    instance = nullptr;
}

#pragma endregion
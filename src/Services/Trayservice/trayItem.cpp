#include <Services/TrayService.hpp>
#include <Utils/StringUtil.hpp>
#include <Widgets/Box.hpp>
#include <Widgets/Label.hpp>
#include <algorithm>
#include <cstring>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

static int id = 0;
void Services::Tray::TrayItem::loadMenuLayout(Gio::Menu* model, std::tuple<gint32, std::map<Glib::ustring, Glib::VariantBase>, std::vector<Glib::VariantBase>> layout) {
    auto [layer, properties, children] = layout;
    printf("loading menu layout at layer: %u, properties: %zu\n", layer, properties.size());

    for(auto it : properties) {
        printf("property: %s, property_type: %s\n", it.first.c_str(), it.second.get_type_string().c_str());

        switch(Util::String::hashFunc(it.second.get_type_string().c_str())) {
        case "s"_hash: printf("  val: %s\n", it.second.get_dynamic<std::string>().c_str()); break;
        case "i"_hash: printf("  val: %d\n", it.second.get_dynamic<int>()); break;
        case "b"_hash: printf("  val: %s\n", it.second.get_dynamic<bool>() ? "true" : "false"); break;
        default:       printf("  couldn't view type info\n"); break;
        }
    }

    if(properties.contains("label")) {
        if(properties.contains("children-display") && properties["children-display"].get_dynamic<std::string>() == "submenu") {
            auto newModel = Gio::Menu::create();

            model->append_submenu(properties["label"].get_dynamic<std::string>(), newModel);
            model = newModel.get();
        }
        else {
            std::string actionName = fmt::format("app.toggle", id++);
            if(!Gtk::Application::get_default()->has_action(actionName)) {
                auto action = Gio::SimpleAction::create(actionName, Glib::VARIANT_TYPE_BOOL, Glib::Variant<bool>::create(false));
                action->signal_activate().connect([action](const Glib::VariantBase& val) {
                    // action->set_state(Glib::Variant<bool>::create(!val.get_dynamic<bool>()));
                    printf("toggled: %d\n", val.get_dynamic<bool>());
                });

                Gtk::Application::get_default()->add_action(action);
                printf("%s\n", action->get_name().c_str());
            }

            auto item = Gio::MenuItem::create(properties["label"].get_dynamic<std::string>(), actionName);
            item->set_action_and_target(actionName, Glib::Variant<bool>::create(false));

            model->append_item(item);
        }
    }

    printf("\n");
    for(auto it : children) {
        loadMenuLayout(model, it.get_dynamic<std::tuple<gint32, std::map<Glib::ustring, Glib::VariantBase>, std::vector<Glib::VariantBase>>>());
    }
}

Glib::RefPtr<Gdk::Pixbuf> Services::Tray::TrayItem::loadPixmap(std::vector<std::tuple<int, int, std::vector<guint8>>> pixmap) {
    if(pixmap.size() <= 0) {
        return nullptr;
    }

    std::sort(pixmap.begin(), pixmap.end(), [](const auto& a, const auto& b) { return std::get<0>(a) > std::get<1>(b); });
    auto& [width, height, data] = pixmap[0];

    const int channels = 4;
    if(data.size() < width * height * channels) {
        return nullptr;
    }

    guint8* buf = (guint8*)malloc(data.size());
    memcpy(buf, data.data(), data.size());

    for(size_t i = 0; i < data.size(); i += channels) {
        std::rotate(buf + i, buf + i + 1, buf + i + channels);
    }

    return Gdk::Pixbuf::create_from_data(
        buf,
        Gdk::Colorspace::RGB, true,
        8, width, height, width * channels,
        [buf](const guint8*) {
            free(buf);
        }
    );
}

void Services::Tray::TrayItem::reloadIconTheme() {
    bool hasThemePath     = false;
    std::string themePath = IconThemePath_get(&hasThemePath);

    if(!hasThemePath || themePath.empty()) {
        _iconTheme = nullptr;
    }
    else {
        _iconTheme = Gtk::IconTheme::create();
        _iconTheme->set_search_path({ themePath });
    }
}

void Services::Tray::TrayItem::reloadIcon() {
    bool hasStatus, hasIconName, hasIconPixmap;
    std::string status = Status_get(&hasStatus), iconName = IconName_get(&hasIconName);

    if(hasStatus && status == "NeedsAttention") {
        iconName = AttentionIconName_get(&hasIconName);
    }

    if(hasIconName && !iconName.empty()) {
        if(_iconTheme) {
            auto sizes = _iconTheme->get_icon_sizes(iconName);

            auto it  = std::max_element(sizes.begin(), sizes.end());
            int size = it != sizes.end() ? *it : 128;

            _icon = _iconTheme->lookup_icon(iconName, size);
            goto end;
        }

        _icon = iconName;
        goto end;
    }
    else {
        auto iconPixmap = IconPixmap_get(&hasIconPixmap);
        if(hasStatus && status == "NeedsAttention") {
            iconPixmap = AttentionIconPixmap_get(&hasIconPixmap);
        }

        auto pixbuf = loadPixmap(iconPixmap);
        if(pixbuf == nullptr) {
            _icon = "";
            goto end;
        }

        _icon = pixbuf;
        goto end;
    }

end:
    _signal_icon.emit();
}

void Services::Tray::TrayItem::reloadMenu() {
    std::string menuPath = Menu_get();
    if(menuPath.empty()) {
        _menu = nullptr;
        return;
    }

    if(_menuCancellable != nullptr) {
        _menuCancellable->cancel();
    }

    _menuCancellable = Gio::Cancellable::create();
    com::canonical::dbusmenuProxy::createForBus(
        Gio::DBus::BusType::SESSION,
        Gio::DBus::ProxyFlags::NONE,
        dbusProxy()->get_name(),
        menuPath,
        [&](const Glib::RefPtr<Gio::AsyncResult>& res) {
            _menuProxy = com::canonical::dbusmenuProxy::createForBusFinish(res);
            _menuProxy->GetLayout(
                0,
                -1,
                {},
                [&](const Glib::RefPtr<Gio::AsyncResult>& res) {
                    guint32 revision;
                    std::tuple<gint32, std::map<Glib::ustring, Glib::VariantBase>, std::vector<Glib::VariantBase>> out;

                    _menuProxy->GetLayout_finish(revision, out, res);
                    _menu = Gio::Menu::create();

                    id = 0;
                    loadMenuLayout(_menu.get(), out);

                    _signal_menu.emit();
                },
                _menuCancellable
            );
        },
        _menuCancellable
    );

    _menuCancellable->connect([&]() {
        _menu = nullptr;
    });
}

void Services::Tray::TrayItem::handle_properties_changed(const Gio::DBus::Proxy::MapChangedProperties& changed_properties, const std::vector<Glib::ustring>& invalidated_properties) {
    org::kde::StatusNotifierItemProxy::handle_properties_changed(changed_properties, invalidated_properties);

    if(
        changed_properties.contains("Status") ||
        changed_properties.contains("IconName") ||
        changed_properties.contains("IconPixmap") ||
        changed_properties.contains("AttentionIconName") ||
        changed_properties.contains("AttentionIconPixmap")
    ) {
        if(changed_properties.contains("Status")) {
            reloadIconTheme();
        }

        reloadIcon();
    }

    if(changed_properties.contains("Menu")) {
        reloadMenu();
    }
}

Services::Tray::TrayItem::~TrayItem() {}
Services::Tray::TrayItem::TrayItem(Glib::RefPtr<Gio::DBus::Proxy> proxy)
    : Glib::ObjectBase(typeid(TrayItem))
    , org::kde::StatusNotifierItemProxy(proxy) {
    resetProperties();
    m_proxy->signal_signal().connect([this](const Glib::ustring&, const Glib::ustring&, const Glib::VariantContainerBase&) { resetProperties(); });
}

std::string Services::Tray::TrayItem::getID() { return Id_get(); }
std::string Services::Tray::TrayItem::getTitle() { return Title_get(); }
std::variant<std::string, Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gtk::IconPaintable>> Services::Tray::TrayItem::getIcon() { return _icon; }
Glib::RefPtr<Gio::Menu> Services::Tray::TrayItem::getMenu() { return _menu; }

void Services::Tray::TrayItem::resetProperties() {
    dbusProxy()->get_connection()->call(
        dbusProxy()->get_object_path(),
        "org.freedesktop.DBus.Properties",
        "GetAll",
        Glib::VariantContainerBase::create_tuple(Glib::Variant<Glib::ustring>::create(dbusProxy()->get_interface_name().c_str())),
        [&](const Glib::RefPtr<Gio::AsyncResult>& res) {
            auto out  = dbusProxy()->get_connection()->call_finish(res);
            auto dict = out.get_child(0).get_dynamic<std::map<Glib::ustring, Glib::VariantBase>>();

            std::vector<Glib::ustring> invalidated;
            Gio::DBus::Proxy::MapChangedProperties changed;

            for(const auto& [key, value] : dict) {
                // printf("key: %s, value: %s\n", key.c_str(), value.get_type_string().c_str());
                dbusProxy()->set_cached_property(key, value);

                invalidated.push_back(key);
                changed[key] = value;
            }

            handle_properties_changed(changed, invalidated);
        },
        dbusProxy()->get_name(),
        -1,
        Gio::DBus::CallFlags::NONE,
        Glib::VariantType::create_tuple({ Glib::VariantType::create_array(Glib::VariantType::create_dict_entry(Glib::VARIANT_TYPE_STRING, Glib::VARIANT_TYPE_VARIANT)) })
    );
}
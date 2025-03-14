#include <Services/TrayService.hpp>
#include <Utils/StringUtil.hpp>
#include <Widgets/Box.hpp>
#include <Widgets/Label.hpp>
#include <cstring>

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
        _menu = libdbusmenu::Menu::create(dbusProxy()->get_name(), Menu_get());
    }
}

Services::Tray::TrayItem::~TrayItem() {}
Services::Tray::TrayItem::TrayItem(Glib::RefPtr<Gio::DBus::Proxy> proxy)
    : Glib::ObjectBase(typeid(TrayItem))
    , org::kde::StatusNotifierItemProxy(proxy) {
    _menu = libdbusmenu::Menu::create(dbusProxy()->get_name(), Menu_get());

    resetProperties();
    m_proxy->signal_signal().connect([this](const Glib::ustring&, const Glib::ustring&, const Glib::VariantContainerBase&) { resetProperties(); });
}

std::string Services::Tray::TrayItem::getID() { return Id_get(); }
std::string Services::Tray::TrayItem::getTitle() { return Title_get(); }
std::variant<std::string, Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gtk::IconPaintable>> Services::Tray::TrayItem::getIcon() { return _icon; }

Glib::RefPtr<libdbusmenu::Menu> Services::Tray::TrayItem::getMenuModel() { return _menu; }
Glib::RefPtr<Gtk::PopoverMenu> Services::Tray::TrayItem::getMenuWidget() {
    auto popover = Glib::make_refptr_for_instance(new Gtk::PopoverMenu(_menu));

    popover->insert_action_group(_menu->get_actionGroupPrefix(), _menu->get_actionGroup());

    return popover;
}

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
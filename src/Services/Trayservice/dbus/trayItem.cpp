#include <Services/TrayService.hpp>
#include <vector>

#include "giomm/asyncresult.h"
#include "giomm/dbusconnection.h"
#include "glibmm/objectbase.h"
#include "glibmm/refptr.h"
#include "glibmm/ustring.h"
#include "glibmm/variant.h"
#include "glibmm/variantiter.h"
#include "glibmm/varianttype.h"

TrayItem::~TrayItem() {}
TrayItem::TrayItem(Glib::RefPtr<Gio::DBus::Proxy> proxy)
    : Glib::ObjectBase(typeid(TrayItem))
    , org::kde::StatusNotifierItemProxy(proxy) {
    resetProperties();
}

std::string TrayItem::getID() { return Id_get(); }
std::string TrayItem::getTitle() { return Title_get(); }
std::string TrayItem::getIconName() { return IconName_get(); }
std::string TrayItem::getIconThemePath() { return IconThemePath_get(); }

void TrayItem::resetProperties() {
    dbusProxy()->get_connection()->call(
        dbusProxy()->get_object_path(),
        "org.freedesktop.DBus.Properties",
        "GetAll",
        Glib::VariantContainerBase::create_tuple(Glib::Variant<Glib::ustring>::create(dbusProxy()->get_interface_name().c_str())),
        [&](const Glib::RefPtr<Gio::AsyncResult>& res) {
            auto out = dbusProxy()->get_connection()->call_finish(res);
            printf("%s\n%s\n", out.get_type_string().c_str(), out.get_child(0).get_type_string().c_str());

            auto dict = out.get_child(0).get_dynamic<std::map<Glib::ustring, Glib::VariantBase>>();
            std::vector<Glib::ustring> invalidated;
            Gio::DBus::Proxy::MapChangedProperties changed;

            for(const auto& [key, value] : dict) {
                printf("key: %s, value: %s\n", key.c_str(), value.get_type_string().c_str());

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
#include <Widgets/Icon.hpp>
#include <variant>

void Widgets::Icon::applyIcon() {
    if(std::holds_alternative<std::string>(getIconProps().icon)) {
        if(Gtk::IconTheme::get_for_display(Gdk::Display::get_default())->has_icon(std::get<std::string>(getIconProps().icon))) {
            set_from_icon_name(std::get<std::string>(getIconProps().icon));
        }
        else {
            set_from_resource(std::get<std::string>(getIconProps().icon));
        }
    }
    else if(std::holds_alternative<Glib::RefPtr<Gdk::Pixbuf>>(getIconProps().icon)) {
        set(std::get<Glib::RefPtr<Gdk::Pixbuf>>(getIconProps().icon));
    }
    else if(std::holds_alternative<Glib::RefPtr<Gtk::IconPaintable>>(getIconProps().icon)) {
        set(std::get<Glib::RefPtr<Gtk::IconPaintable>>(getIconProps().icon));
    }
}

void Widgets::Icon::applyIconSize() {
    set_icon_size(getIconProps().iconSize);
}

void Widgets::Icon::applyPixelSize() {
    set_pixel_size(getIconProps().pixelSize);
}

void Widgets::Icon::applyProps() {
    Widgets::Widget::applyProps();

    applyIcon();
    applyIconSize();
    applyPixelSize();
}

void Widgets::Icon::__init() {
    Widgets::Widget::__init();
}

Widgets::Icon::Icon()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::Icon::Icon(IconProps props)
    : Widgets::Widget(this)
    , _props(props) {}

Widgets::Icon::~Icon() {}
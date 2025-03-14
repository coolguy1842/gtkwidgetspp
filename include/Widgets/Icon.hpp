#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include <Widgets/Widget.hpp>
#include <variant>

struct IconProps {
    WidgetProps widget = {};

    std::variant<std::string, Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gtk::IconPaintable>> icon;

    Gtk::IconSize iconSize = Gtk::IconSize::INHERIT;
    int pixelSize          = -1;
};

namespace Widgets {

class Icon : public Gtk::Image, public Widgets::Widget {
protected:
    IconProps _props;

    virtual WidgetProps& getWidgetProps() { return _props.widget; }
    virtual IconProps& getIconProps() { return _props; }

    virtual void applyIcon();
    virtual void applyIconSize();
    virtual void applyPixelSize();
    virtual void applyProps();

    Icon();
    Icon(IconProps props);

public:
    static Widgets::Icon* create(IconProps props) {
        Widgets::Icon* icon = new Widgets::Icon(props);
        icon->__init();

        return icon;
    }

    // dont call manually
    void __init();
    virtual ~Icon();

    void setIcon(std::variant<std::string, Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gtk::IconPaintable>> icon);
};

};  // namespace Widgets
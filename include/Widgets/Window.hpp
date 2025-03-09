#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include <Widgets/Widget.hpp>
#include <bitset>

#include "gdkmm/monitor.h"

enum AnchorEdge {
    NONE   = 0b0000,
    LEFT   = 0b0001,
    RIGHT  = 0b0010,
    TOP    = 0b0100,
    BOTTOM = 0b1000
};

struct WindowProps {
    WidgetProps widget = {};

    Gdk::Monitor* monitor = nullptr;

    Gtk::Widget* child = nullptr;
    std::bitset<4> anchor{ NONE };

    // -1 is auto, anything below -1 will make it none and above 0 will set a
    // specific exclusive zone, 0 does none
    int64_t exclusive = 0;
};

namespace Widgets {

class Window : public Gtk::Window, public Widgets::Widget {
protected:
    WindowProps _props;

    virtual WidgetProps& getWidgetProps() { return _props.widget; }
    virtual WindowProps& getWindowProps() { return _props; }

    virtual void applyChild();
    virtual void applyAnchor();
    virtual void applyExclusive();

    virtual void applyProps();

    Window();
    Window(WindowProps props);

public:
    static Widgets::Window* create(WindowProps props = {}) {
        Widgets::Window* window = new Widgets::Window(props);
        window->__init();

        return window;
    }

    // dont call manually
    void __init();
    virtual ~Window();

    virtual std::bitset<4> getAnchor();
    virtual int64_t getExclusive();

    virtual void setAnchor(std::bitset<4> anchor);
    virtual void setExclusive(int64_t exclusive);
};

};  // namespace Widgets
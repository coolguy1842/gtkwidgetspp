#include <gtk4-layer-shell/gtk4-layer-shell.h>
#include <gtkmm-4.0/gtkmm.h>

#include <Widgets/Window.hpp>
#include <bitset>
#include <cstdint>

void Widgets::Window::applyChild() {
    this->set_child(*getWindowProps().child);
}

void Widgets::Window::applyAnchor() {
    for(uint64_t i = 0; i < 4; i++) {
        gtk_layer_set_anchor(this->gobj(), (GtkLayerShellEdge)i, getWindowProps().anchor.test(i));
    }
}

void Widgets::Window::applyExclusive() {
    if(getWindowProps().exclusive == -1) {
        gtk_layer_auto_exclusive_zone_enable(this->gobj());

        return;
    }

    gtk_layer_set_exclusive_zone(this->gobj(), getWindowProps().exclusive);
}

void Widgets::Window::applyProps() {
    Widgets::Widget::applyProps();

    applyChild();
    applyAnchor();
    applyExclusive();
}

void Widgets::Window::__init() {
    gtk_layer_init_for_window(this->gobj());

    Widgets::Widget::__init();

    if(getWindowProps().monitor != nullptr) {
        gtk_layer_set_monitor(gobj(), getWindowProps().monitor->gobj());
    }
}

Widgets::Window::Window()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::Window::Window(WindowProps props)
    : Widgets::Widget(this)
    , _props(props) {}

Widgets::Window::~Window() {}

std::bitset<4> Widgets::Window::getAnchor() {
    for(uint i = 0; i < 4; i++) {
        getWindowProps().anchor.set(i, gtk_layer_get_anchor(this->gobj(), (GtkLayerShellEdge)i));
    }

    return getWindowProps().anchor;
}

int64_t Widgets::Window::getExclusive() {
    if(gtk_layer_auto_exclusive_zone_is_enabled(this->gobj())) {
        return (getWindowProps().exclusive = -1);
    }

    return (getWindowProps().exclusive = gtk_layer_get_exclusive_zone(this->gobj()));
}

void Widgets::Window::setAnchor(std::bitset<4> anchor) {
    getWindowProps().anchor = anchor;
    applyAnchor();
}
void Widgets::Window::setExclusive(int64_t exclusive) {
    getWindowProps().exclusive = exclusive;
    applyExclusive();
}
#include <Widgets/Button.hpp>
#include <Widgets/Icon.hpp>
#include <variant>

#include "glibmm/refptr.h"
#include "gtkmm/iconpaintable.h"

void Widgets::Button::applyChild() {
    if(std::holds_alternative<std::string>(getButtonProps().child)) {
        this->set_label(std::get<std::string>(getButtonProps().child));
    }
    else if(std::holds_alternative<Gtk::Widget*>(getButtonProps().child)) {
        set_child(*std::get<Gtk::Widget*>(getButtonProps().child));
    }
    else if(std::holds_alternative<Glib::RefPtr<Gdk::Pixbuf>>(getButtonProps().child)) {
        set_child(*Widgets::Icon::create({ .icon = std::get<Glib::RefPtr<Gdk::Pixbuf>>(getButtonProps().child) }));
    }
    else if(std::holds_alternative<Glib::RefPtr<Gtk::IconPaintable>>(getButtonProps().child)) {
        set_child(*Widgets::Icon::create({ .icon = std::get<Glib::RefPtr<Gtk::IconPaintable>>(getButtonProps().child) }));
    }
}

void Widgets::Button::applyOnClick() {
    if(getButtonProps().on_click) {
        _callbacks.push_back(getButtonProps().on_click);
    }
}

void Widgets::Button::applyProps() {
    Widgets::Widget::applyProps();

    applyChild();
    applyOnClick();
}

void Widgets::Button::__init() {
    Widgets::Widget::__init();
}

Widgets::Button::Button()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::Button::Button(ButtonProps props)
    : Widgets::Widget(this)
    , _props(props) {}
Widgets::Button::~Button() {}

void Widgets::Button::on_clicked() {
    for(auto it = _callbacks.begin(); it != _callbacks.end(); it++) {
        if(!(*it)(this)) {
            _callbacks.erase(it++);
        }
    }
}

void Widgets::Button::setChild(std::variant<std::string, Gtk::Widget*, Glib::RefPtr<Gdk::Pixbuf>, Glib::RefPtr<Gtk::IconPaintable>> child) {
    getButtonProps().child = child;
    applyChild();
}
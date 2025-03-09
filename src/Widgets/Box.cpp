#include <Widgets/Box.hpp>

void Widgets::Box::applyChildren() {
    for(Gtk::Widget* widget : this->get_children()) {
        this->remove(*widget);
    }

    for(Gtk::Widget* widget : getBoxProps().children) {
        this->append(*widget);
    }
}

void Widgets::Box::applyGap() {
    set_spacing(getBoxProps().spacing);
}

void Widgets::Box::applyProps() {
    Widgets::Widget::applyProps();

    applyChildren();
    applyGap();
}

void Widgets::Box::__init() {
    Widgets::Widget::__init();
}

Widgets::Box::Box()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::Box::Box(BoxProps props)
    : Widgets::Widget(this)
    , _props(props) {}

Widgets::Box::~Box() {}

void Widgets::Box::setChildren(std::vector<Gtk::Widget*> children) {
    getBoxProps().children = children;
    applyChildren();
}
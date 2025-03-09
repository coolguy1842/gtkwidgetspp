#include <Widgets/Label.hpp>

void Widgets::Label::applyText() {
    this->set_text(getLabelProps().text);
}

void Widgets::Label::applyProps() {
    Widgets::Widget::applyProps();

    applyText();
}

void Widgets::Label::__init() {
    Widgets::Widget::__init();
}

Widgets::Label::Label()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::Label::Label(LabelProps props)
    : Widgets::Widget(this)
    , _props(props) {}

Widgets::Label::~Label() {}
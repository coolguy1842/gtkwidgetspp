#include <Widgets/CenterBox.hpp>

void Widgets::CenterBox::applyLeft() {
    if(getCenterBoxProps().left == nullptr) {
        return;
    }

    this->set_start_widget(*getCenterBoxProps().left);
}

void Widgets::CenterBox::applyCenter() {
    if(getCenterBoxProps().center == nullptr) {
        return;
    }

    this->set_center_widget(*getCenterBoxProps().center);
}

void Widgets::CenterBox::applyRight() {
    if(getCenterBoxProps().right == nullptr) {
        return;
    }

    this->set_end_widget(*getCenterBoxProps().right);
}

void Widgets::CenterBox::applyProps() {
    Widgets::Widget::applyProps();

    applyLeft();
    applyCenter();
    applyRight();
}

void Widgets::CenterBox::__init() {
    Widgets::Widget::__init();
}

Widgets::CenterBox::CenterBox()
    : Widgets::Widget(this)
    , _props({}) {}
Widgets::CenterBox::CenterBox(CenterBoxProps props)
    : Widgets::Widget(this)
    , _props(props) {}

Widgets::CenterBox::~CenterBox() {}
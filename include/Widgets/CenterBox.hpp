#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include <Widgets/Widget.hpp>

struct CenterBoxProps {
    WidgetProps widget;

    Gtk::Widget* left;
    Gtk::Widget* center;
    Gtk::Widget* right;
};

namespace Widgets {

class CenterBox : public Gtk::CenterBox, public Widgets::Widget {
protected:
    CenterBoxProps _props;

    virtual WidgetProps& getWidgetProps() { return _props.widget; }
    virtual CenterBoxProps& getCenterBoxProps() { return _props; }

    virtual void applyLeft();
    virtual void applyCenter();
    virtual void applyRight();
    virtual void applyProps();

    CenterBox();
    CenterBox(CenterBoxProps props);

public:
    static Widgets::CenterBox* create(CenterBoxProps props = {}) {
        Widgets::CenterBox* centerBox = new Widgets::CenterBox(props);
        centerBox->__init();

        return centerBox;
    }

    // dont call manually
    void __init();
    virtual ~CenterBox();
};

};  // namespace Widgets
#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include <Widgets/Widget.hpp>

struct LabelProps {
    WidgetProps widget = {};

    std::string text = "";
};

namespace Widgets {

class Label : public Gtk::Label, public Widgets::Widget {
protected:
    LabelProps _props;

    virtual WidgetProps& getWidgetProps() { return _props.widget; }
    virtual LabelProps& getLabelProps() { return _props; }

    virtual void applyText();
    virtual void applyProps();

    Label();
    Label(LabelProps props);

public:
    static Widgets::Label* create(LabelProps props = {}) {
        Widgets::Label* label = new Widgets::Label(props);
        label->__init();

        return label;
    }

    static Widgets::Label* create(std::string text) {
        Widgets::Label* label = new Widgets::Label({ .text = text });
        label->__init();

        return label;
    }

    // dont call manually
    void __init();
    virtual ~Label();
};

};  // namespace Widgets
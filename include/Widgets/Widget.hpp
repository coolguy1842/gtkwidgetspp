#pragma once

#include <gtkmm-4.0/gtkmm.h>

#include <variant>

struct WidgetProps {
    struct Margin {
        int start  = 0;
        int end    = 0;
        int top    = 0;
        int bottom = 0;
    };

    std::string name = "";

    bool hExpand = false;
    bool vExpand = false;

    Gtk::Align hAlign = Gtk::Align::FILL;
    Gtk::Align vAlign = Gtk::Align::FILL;

    std::variant<Margin, int> margin = 0;

    int widthRequest  = -1;
    int heightRequest = -1;

    bool visible = true;

    // css can be scss
    std::string css                     = "";
    std::vector<std::string> classNames = {};
};

namespace Widgets {

class Widget {
protected:
    Gtk::Widget* _widget;
    WidgetProps _props;

    Glib::RefPtr<Gtk::CssProvider> _cssProvider;

    virtual void applyName();
    virtual void applyExpands();
    virtual void applyAligns();
    virtual void applyMargin();
    virtual void applySizeRequests();
    virtual void applyVisible();
    virtual void applyCSS();
    virtual void applyClassNames();

    // must call previous if override, must call in child class
    virtual void applyProps();
    // must call previous if override, must call in child class
    virtual void initSignals();

    // to get around diamond inheritance
    virtual WidgetProps& getWidgetProps();

    Widget(Gtk::Widget* widget);
    Widget(Gtk::Widget* widget, WidgetProps props);

public:
    // please call in parent static create function, dont use from constructor
    // to avoid inheritance issues PS: dont call manually outside of the create
    // function
    virtual void __init();
    virtual ~Widget();

    // can be a scss string
    std::string getCSS();

    void setWidthRequest(int widthRequest);
    void setHeightRequest(int heightRequest);

    // can be scss
    void setCSS(std::string css);

    void toggleClassName(std::string className);
    void toggleClassName(std::string className, bool active);
};

};  // namespace Widgets
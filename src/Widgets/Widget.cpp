#include <Utils/CSSUtil.hpp>
#include <Widgets/Widget.hpp>
#include <variant>

void Widgets::Widget::applyName() {
    _widget->set_name(getWidgetProps().name);
}

void Widgets::Widget::applyExpands() {
    _widget->set_hexpand(getWidgetProps().hExpand);
    _widget->set_vexpand(getWidgetProps().vExpand);
}

void Widgets::Widget::applyAligns() {
    _widget->set_halign(getWidgetProps().hAlign);
    _widget->set_valign(getWidgetProps().vAlign);
}

void Widgets::Widget::applySizeRequests() {
    _widget->property_width_request().set_value(getWidgetProps().widthRequest);
    _widget->property_width_request().set_value(getWidgetProps().heightRequest);
}

void Widgets::Widget::applyVisible() {
    _widget->set_visible(getWidgetProps().visible);
}

void Widgets::Widget::applyCSS() {
    std::optional<std::string> css = Util::CSS::SCSStoCSS(getWidgetProps().css);

    if(_cssProvider != nullptr) {
        this->_widget->get_style_context()->remove_provider(_cssProvider);
        _cssProvider.reset();
    }

    _cssProvider = Gtk::CssProvider::create();

    if(css.has_value()) {
        _cssProvider->load_from_string(css.value());
    }

    this->_widget->get_style_context()->add_provider(_cssProvider, GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void Widgets::Widget::applyClassNames() {
    for(const std::string& className : getWidgetProps().classNames) {
        _widget->add_css_class(className);
    }
}

void Widgets::Widget::applyMargin() {
    if(std::holds_alternative<int>(getWidgetProps().margin)) {
        _widget->set_margin(std::get<int>(getWidgetProps().margin));
    }
    else if(std::holds_alternative<WidgetProps::Margin>(getWidgetProps().margin)) {
        WidgetProps::Margin margin = std::get<WidgetProps::Margin>(getWidgetProps().margin);

        _widget->set_margin_start(margin.start);
        _widget->set_margin_end(margin.end);
        _widget->set_margin_top(margin.top);
        _widget->set_margin_bottom(margin.bottom);
    }
}

void Widgets::Widget::applyProps() {
    applyName();
    applyExpands();
    applyAligns();
    applyMargin();
    applySizeRequests();
    applyVisible();
    applyCSS();
    applyClassNames();
};

void Widgets::Widget::initSignals() {
    _widget->property_hexpand().signal_changed().connect([&]() { getWidgetProps().hExpand = _widget->get_hexpand(); });
    _widget->property_vexpand().signal_changed().connect([&]() { getWidgetProps().vExpand = _widget->get_vexpand(); });

    _widget->property_width_request().signal_changed().connect([&]() { getWidgetProps().widthRequest = _widget->property_width_request().get_value(); });
    _widget->property_height_request().signal_changed().connect([&]() { getWidgetProps().heightRequest = _widget->property_height_request().get_value(); });

    _widget->property_visible().signal_changed().connect([&]() { getWidgetProps().visible = _widget->is_visible(); });
}

void Widgets::Widget::__init() {
    initSignals();
    applyProps();
}

Widgets::Widget::Widget(Gtk::Widget* widget)
    : _widget(widget) {}
Widgets::Widget::Widget(Gtk::Widget* widget, WidgetProps props)
    : _widget(widget)
    , _props(props) {}

Widgets::Widget::~Widget() {}

WidgetProps& Widgets::Widget::getWidgetProps() { return _props; }

std::string Widgets::Widget::getCSS() { return getWidgetProps().css; };

void Widgets::Widget::setWidthRequest(int widthRequest) {
    getWidgetProps().widthRequest = widthRequest;
    applySizeRequests();
}
void Widgets::Widget::setHeightRequest(int heightRequest) {
    getWidgetProps().heightRequest = heightRequest;
    applySizeRequests();
}

void Widgets::Widget::setCSS(std::string css) {
    getWidgetProps().css = css;
    applyCSS();
};

void Widgets::Widget::toggleClassName(std::string className) {
    if(_widget->has_css_class(className)) {
        _widget->remove_css_class(className);
    }
    else {
        _widget->add_css_class(className);
    }
};

void Widgets::Widget::toggleClassName(std::string className, bool active) {
    if(active) {
        _widget->add_css_class(className);
    }
    else {
        _widget->remove_css_class(className);
    }
};
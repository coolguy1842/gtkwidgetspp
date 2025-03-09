project('gtkwidgetspp', [ 'cpp', 'c' ], default_options : ['c_std=c23', 'cpp_std=c++23'])

fs = import('fs')

if(fs.exists('generated') != true)
    run_command(
        meson.current_source_dir() + '/gdbus-codegen-glibmm/gdbus-codegen-glibmm.py',
        '--generate-cpp-code=' + meson.current_source_dir() + '/generated/status-notifier-watcher-interface',
        meson.current_source_dir() + '/data/introspect/org.kde.StatusNotifierWatcher.xml',
        check: true
    )

    run_command(
        meson.current_source_dir() + '/gdbus-codegen-glibmm/gdbus-codegen-glibmm.py',
        '--generate-cpp-code=' + meson.current_source_dir() + '/generated/status-notifier-item-interface',
        meson.current_source_dir() + '/data/introspect/org.kde.StatusNotifierItem.xml',
        check: true
    )
endif

inc = include_directories('generated', 'include')

libgtkwidgetspp = static_library(
    'gtkwidgetspp',
    sources: [
        files(
            'generated/status-notifier-item-interface_common.cpp',
            'generated/status-notifier-item-interface_proxy.cpp',
            'generated/status-notifier-item-interface_stub.cpp',

            'generated/status-notifier-watcher-interface_common.cpp',
            'generated/status-notifier-watcher-interface_proxy.cpp',
            'generated/status-notifier-watcher-interface_stub.cpp'
        ),
        files(
            'src/Utils/CSSUtil.cpp',
            'src/Utils/PathUtil.cpp',
            'src/Utils/ProcessUtil.cpp',
            'src/Utils/StringUtil.cpp',

            'src/Widgets/Widget.cpp',
            'src/Widgets/Label.cpp',
            'src/Widgets/Icon.cpp',
            'src/Widgets/Button.cpp',
            'src/Widgets/Box.cpp',
            'src/Widgets/CenterBox.cpp',
            'src/Widgets/Window.cpp',

            'src/Services/Hyprservice/client.cpp',
            'src/Services/Hyprservice/workspace.cpp',
            'src/Services/Hyprservice/monitor.cpp',
            'src/Services/Hyprservice/actives.cpp',
            'src/Services/Hyprservice/socket.cpp',
            'src/Services/Hyprservice/event.cpp',
            'src/Services/Hyprservice/service.cpp',

            'src/Services/Trayservice/dbus/trayItem.cpp',
            'src/Services/Trayservice/dbus/statusNotifierWatcher.cpp',
            'src/Services/Trayservice/dbus/watcher.cpp',
            'src/Services/Trayservice/service.cpp'
        )
    ],
    include_directories : inc,
    install : false,
    dependencies: [
        dependency('gtk4'),
        dependency('gtkmm-4.0'),
        dependency('gtk4-layer-shell-0'),
        dependency('nlohmann_json'),
    ]
)

libgtkwidgetspp_dep = declare_dependency(
    include_directories : inc,

    link_with : libgtkwidgetspp,
    dependencies: [
        dependency('gtk4'),
        dependency('gtkmm-4.0'),
        dependency('gtk4-layer-shell-0'),
        dependency('nlohmann_json'),
    ]
)

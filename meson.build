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

    run_command(
        meson.current_source_dir() + '/gdbus-codegen-glibmm/gdbus-codegen-glibmm.py',
        '--generate-cpp-code=' + meson.current_source_dir() + '/generated/dbusmenu-interface',
        meson.current_source_dir() + '/data/introspect/com.canonical.dbusmenu.xml',
        check: true
    )
endif

libdbusmenugtk4_proj = subproject('libdbusmenugtk4')
libdbusmenugtk4_include = libdbusmenugtk4_proj.get_variable('libdbusmenugtk4_include')
libdbusmenugtk4_lib = libdbusmenugtk4_proj.get_variable('libdbusmenugtk4')


libgtkwidgetspp_include = [ include_directories('generated', 'include'), libdbusmenugtk4_include ]
libgtkwidgetspp = static_library(
    'gtkwidgetspp',
    sources: [
        files(
            'generated/status-notifier-item-interface_common.cpp',
            'generated/status-notifier-item-interface_proxy.cpp',
            'generated/status-notifier-item-interface_stub.cpp',

            'generated/status-notifier-watcher-interface_common.cpp',
            'generated/status-notifier-watcher-interface_proxy.cpp',
            'generated/status-notifier-watcher-interface_stub.cpp',

            'generated/dbusmenu-interface_common.cpp',
            'generated/dbusmenu-interface_proxy.cpp',
            'generated/dbusmenu-interface_stub.cpp'
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

            'src/Services/Trayservice/trayItem.cpp',
            'src/Services/Trayservice/statusNotifierWatcher.cpp',
            'src/Services/Trayservice/watcher.cpp',
            'src/Services/Trayservice/service.cpp'
        )
    ],
    include_directories : libgtkwidgetspp_include,
    install : false,
    dependencies: [
        dependency('gtk4'),
        dependency('gtkmm-4.0'),
        dependency('gtk4-layer-shell-0'),
        dependency('nlohmann_json')
    ],
    link_with: [
        libdbusmenugtk4_lib
    ]
)

libgtkwidgetspp_dep = declare_dependency(
    include_directories: libgtkwidgetspp_include,
    link_with : [ libgtkwidgetspp, libdbusmenugtk4_lib ],
    dependencies: [
        dependency('gtk4'),
        dependency('gtkmm-4.0'),
        dependency('gtk4-layer-shell-0'),
        dependency('nlohmann_json')
    ]
)

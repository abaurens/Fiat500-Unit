cmake_minimum_required(VERSION 3.16)
project(Fiat500-Unit  VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(
    NO_FULLSCREEN
    "Disable fullscreen launch of the app (usefull when debuging the app on a PC)"
    OFF
)

option(
    DEBUG_PANEL
    "Build the debug panel instead of the actual headunit app"
    OFF
)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Gui
    Core
    DBus
    Widgets
    Bluetooth
    SvgWidgets
    Concurrent
    SerialPort
    LinguistTools
)

qt_standard_project_setup()

qt_add_executable(Fiat500-Unit
    WIN32 MACOSX_BUNDLE

    resources.qrc

    src/main.cpp

    src/Ref.hpp
    src/Scope.hpp
    src/MainWindow.cpp
    src/MainWindow.hpp

    # Settings system
    src/settings/Setting.hpp

    # DBus & Bluez API
    src/dbus/Types.hpp src/dbus/Types.cpp
    src/dbus/Object.hpp src/dbus/Object.cpp

    src/dbus/bluez/Types.hpp

    src/dbus/bluez/Device.hpp src/dbus/bluez/Device.cpp
    src/dbus/bluez/Adapter.hpp src/dbus/bluez/Adapter.cpp
    src/dbus/bluez/MediaPlayer.hpp src/dbus/bluez/MediaPlayer.cpp
    src/dbus/bluez/MediaControl.hpp src/dbus/bluez/MediaControl.cpp
    src/dbus/bluez/Manager.hpp src/dbus/bluez/Manager.cpp src/dbus/bluez/Manager.tpp

    # Custom widgets
    src/widgets/sizes.hpp
    src/widgets/SideBar.hpp src/widgets/SideBar.cpp

    # Main pages
    src/pages/MusicPage.hpp src/pages/MusicPage.cpp
    src/pages/SettingsPage.hpp src/pages/SettingsPage.cpp

    # Menu system
    src/menu/Entry.hpp src/menu/Entry.cpp
    src/menu/Item.hpp src/menu/Item.cpp
    src/menu/Page.hpp src/menu/Page.cpp
    src/menu/Menu.hpp src/menu/Menu.cpp

    src/menu/widgets/Navigator.hpp src/menu/widgets/Navigator.cpp
    src/menu/widgets/Breadcrumbs.hpp src/menu/widgets/Breadcrumbs.cpp
)

qt_add_translations(
    TARGETS Fiat500-Unit
    TS_FILES lang/Fiat500-Unit_fr_FR.ts
    TS_FILES lang/Fiat500_Unit_en_US.ts
)

target_include_directories(Fiat500-Unit PRIVATE
    src
    include
)

target_link_libraries(Fiat500-Unit PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Widgets
    Qt6::Bluetooth
    Qt6::SvgWidgets
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DEBUG_PANEL ON)
endif()

# Add Windows specific sources
if (WIN32)
    set(NO_FULLSCREEN ON)

    target_sources(Fiat500-Unit PUBLIC)

    target_link_libraries(Fiat500-Unit PRIVATE user32)

    target_compile_definitions(Fiat500-Unit
        PRIVATE
        "NO_DBUS"
    )

elseif(UNIX AND NOT MACOS)
    target_sources(Fiat500-Unit PUBLIC
    )
endif()

# Add debug panel sources and macro
if (DEBUG_PANEL)
    target_compile_definitions(Fiat500-Unit PRIVATE "DEBUG_PANEL")

    target_sources(Fiat500-Unit PUBLIC
        src/pages/debugPanel/DebugPanel.hpp src/pages/debugPanel/DebugPanel.cpp
        src/pages/debugPanel/MediaPanel.hpp src/pages/debugPanel/MediaPanel.cpp
        src/pages/debugPanel/DevicePanel.hpp src/pages/debugPanel/DevicePanel.cpp
        src/pages/debugPanel/AdapterPanel.hpp src/pages/debugPanel/AdapterPanel.cpp
    )
endif()

# Set the fullscreen allowed macro
if (NO_FULLSCREEN)
    target_compile_definitions(Fiat500-Unit
        PRIVATE
        "FULLSCREEN_ALLOWED=false"
    )
else()
    target_compile_definitions(Fiat500-Unit
        PRIVATE
        "FULLSCREEN_ALLOWED=true"
    )
endif()

install(TARGETS Fiat500-Unit
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Fiat500-Unit
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

cmake_minimum_required(VERSION 3.16)
project(Fiat500-Unit  VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)


if(NOT LINUX)
    message(FATAL_ERROR "${PROJECT_NAME} only supports Linux target")
endif()

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

find_package(PkgConfig REQUIRED)
pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE

    resources.qrc

    src/pch.hpp
    src/main.cpp

    src/Log.hpp
    src/Ref.hpp
    src/Enum.hpp
    src/Local.hpp
    src/Scope.hpp
    src/AbstractManager.hpp
    src/CaseInsensitiveCharTrait.hpp

    src/App.hpp src/App.cpp
    src/MainWindow.hpp src/MainWindow.cpp

    # Settings system
    src/settings/Setting.hpp

    # DBus & Bluez API
    src/dbus/Log.hpp
    src/dbus/Types.hpp src/dbus/Types.cpp
    src/dbus/Object.hpp src/dbus/Object.cpp

    src/dbus/bluez/Log.hpp
    src/dbus/bluez/Types.hpp

    src/dbus/bluez/Device.hpp src/dbus/bluez/Device.cpp
    src/dbus/bluez/Adapter.hpp src/dbus/bluez/Adapter.cpp
    src/dbus/bluez/MediaPlayer.hpp src/dbus/bluez/MediaPlayer.cpp
    src/dbus/bluez/MediaControl.hpp src/dbus/bluez/MediaControl.cpp
    src/dbus/bluez/Manager.hpp src/dbus/bluez/Manager.cpp src/dbus/bluez/Manager.tpp

    # Custom widgets
    src/widgets/sizes.hpp
    src/widgets/SideBar.hpp src/widgets/SideBar.cpp

    # Main panels
    src/panels/MusicPanel.hpp src/panels/MusicPanel.cpp
    src/panels/SettingsPanel.hpp src/panels/SettingsPanel.cpp

    # Menu system
    src/menu/core/Entry.hpp src/menu/core/Entry.cpp
    src/menu/core/Item.hpp  src/menu/core/Item.cpp
    src/menu/core/Menu.hpp  src/menu/core/Menu.cpp
    src/menu/core/Page.hpp  src/menu/core/Page.cpp src/menu/core/Page.tpp

    src/menu/widgets/Navigator.hpp src/menu/widgets/Navigator.cpp
    src/menu/widgets/Breadcrumbs.hpp src/menu/widgets/Breadcrumbs.cpp

    src/menu/widgets/ButtonItem.hpp src/menu/widgets/ButtonItem.cpp
    src/menu/widgets/ActionItem.hpp src/menu/widgets/ActionItem.cpp

    src/menu/SystemPage.hpp src/menu/SystemPage.cpp

    # Media stack
    src/media/Log.hpp
    src/media/Sink.hpp
    src/media/Source.hpp

    src/media/Manager.hpp src/media/Manager.cpp

    src/media/audio/Log.hpp
    src/media/audio/Sink.hpp src/media/audio/Sink.cpp
    src/media/audio/Source.hpp src/media/audio/Source.cpp

    src/media/video/Log.hpp
    src/media/video/Sink.hpp src/media/video/Sink.cpp
    src/media/video/Source.hpp src/media/video/Source.cpp

    src/pipewire/Log.hpp
    src/pipewire/Bindable.hpp
    src/pipewire/ParamInfo.hpp
    src/pipewire/Direction.hpp
    src/pipewire/MediaClass.hpp
    src/pipewire/AudioFormat.hpp
    src/pipewire/Link.hpp src/pipewire/Link.cpp
    src/pipewire/Node.hpp src/pipewire/Node.cpp
    src/pipewire/Port.hpp src/pipewire/Port.cpp
    src/pipewire/Device.hpp src/pipewire/Device.cpp
    src/pipewire/Client.hpp src/pipewire/Client.cpp
    src/pipewire/Object.hpp src/pipewire/Object.cpp
    src/pipewire/Context.hpp src/pipewire/Context.cpp

)

target_precompile_headers(${PROJECT_NAME}
    PRIVATE
    src/pch.hpp
)

qt_add_translations(
    TARGETS ${PROJECT_NAME}
    TS_FILES lang/Fiat500-Unit_fr_FR.ts
    TS_FILES lang/Fiat500-Unit_en_US.ts
)

target_include_directories(${PROJECT_NAME} PRIVATE
    src
    include
    libs/frozen/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Widgets
    Qt6::Bluetooth
    Qt6::SvgWidgets

    PkgConfig::PIPEWIRE
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(DEBUG_PANEL ON)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    _REENTRANT
)

# Add debug panel sources and macro
if (DEBUG_PANEL)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "DEBUG_PANEL")

    target_sources(${PROJECT_NAME} PUBLIC
        src/panels/debugPanel/DebugPanel.hpp src/panels/debugPanel/DebugPanel.cpp
        src/panels/debugPanel/MediaPanel.hpp src/panels/debugPanel/MediaPanel.cpp
        src/panels/debugPanel/DevicePanel.hpp src/panels/debugPanel/DevicePanel.cpp
        src/panels/debugPanel/AdapterPanel.hpp src/panels/debugPanel/AdapterPanel.cpp
    )
endif()

# Set the fullscreen allowed macro
if (NO_FULLSCREEN)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        "FULLSCREEN_ALLOWED=false"
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        "FULLSCREEN_ALLOWED=true"
    )
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ${PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

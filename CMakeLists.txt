cmake_minimum_required(VERSION 3.16)
project(Fiat500-Unit  VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_AUTORCC ON)
set(CMAKE_C_STANDARD C11)
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

    src/main.cpp

    src/Log.hpp
    src/Ref.hpp
    src/Enum.hpp
    src/Scope.hpp

    src/MainWindow.cpp
    src/MainWindow.hpp

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

    # Audio stack
    src/audio/Log.hpp
    src/audio/Sink.hpp
    src/audio/Source.hpp
    src/audio/Backend.hpp
    src/audio/Manager.hpp src/audio/Manager.cpp

    src/audio/backend/pipewire/Log.hpp
    src/audio/backend/pipewire/Object.hpp src/audio/backend/pipewire/Object.cpp
    src/audio/backend/pipewire/Backend.hpp src/audio/backend/pipewire/Backend.cpp
    src/App.hpp src/App.cpp
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
